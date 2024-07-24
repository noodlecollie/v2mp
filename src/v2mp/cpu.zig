const std = @import("std");
const v2mp = @import("v2mp.zig");

pub const Cpu = struct {
    const InstructionResult = struct {
        pc: ?v2mp.Word = null,
        sr: ?v2mp.Word = null,
        lr: ?v2mp.Word = null,
        r0: ?v2mp.Word = null,
        r1: ?v2mp.Word = null,
        sp: ?v2mp.Word = null,
        fault: ?v2mp.Fault = null,

        pub fn setRegisterValue(this: *InstructionResult, reg: v2mp.RegisterIndex, value: v2mp.Word) void {
            switch (reg) {
                .r0 => this.r0 = value,
                .r1 => this.r1 = value,
                .lr => this.lr = value,
                .pc => this.pc = value,
            }
        }
    };

    const FetchInstructionFn = *const fn (address: v2mp.Word) v2mp.InstructionFetchError!v2mp.Word;
    const ExecInstructionFn = *const fn (this: *Cpu) InstructionResult;

    fetch_callback: FetchInstructionFn,

    _pc: v2mp.Word = 0,
    _sr: v2mp.Word = 0,
    _lr: v2mp.Word = 0,
    _r0: v2mp.Word = 0,
    _r1: v2mp.Word = 0,
    _ir: v2mp.Word = 0,
    _sp: v2mp.Word = 0,
    _fault: v2mp.Word = @intFromEnum(v2mp.Fault.none),

    _exec_callbacks: [v2mp.max_instruction_opcodes]ExecInstructionFn = .{
        &executeNop, // 0x00
        &executeAdd, // 0x01
        &executeSub, // 0x02
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
        &executeUnassigned,
    },

    pub fn getRegisterValue(this: *const Cpu, reg: v2mp.RegisterIndex) v2mp.Word {
        return switch (reg) {
            .r0 => this._r0,
            .r1 => this._r1,
            .lr => this._lr,
            .pc => this._pc,
        };
    }

    pub fn fetchDecodeExecute(this: *Cpu) void {
        if (this.fetch_callback(this._pc)) |instruction| {
            this._ir = instruction;
        } else |err| {
            this._fault = switch (err) {
                v2mp.InstructionFetchError.UnalignedMemoryAccess => v2mp.makeFaultWord(v2mp.Fault.algn, 0),
            };

            return;
        }

        // Increment PC now, as some instructions rely on it holding the next address
        this._pc +%= @as(v2mp.Word, v2mp.size_of_word);

        const instruction_op: usize = @intFromEnum(v2mp.instructionOpCodeFromWord(this._ir));
        std.debug.assert(instruction_op <= this._exec_callbacks.len);

        const result = this._exec_callbacks[instruction_op](this);
        this.acceptResult(result);
    }

    fn acceptResult(this: *Cpu, result: InstructionResult) void {
        const fault: v2mp.Fault = choose_fault: {
            if (result.fault) |res_fault| {
                // If the instruction caused a fault, this should take precedence.
                break :choose_fault res_fault;
            } else if (result.pc == null and this._pc -% @as(v2mp.Word, v2mp.size_of_word) > this._pc) {
                // The instruction did not set the program counter, and the increment
                // that happened earlier caused an overflow. We should catch this case.
                break :choose_fault .seg;
            } else {
                // Nothing went wrong.
                break :choose_fault .none;
            }
        };

        this._sr = result.sr orelse this._sr;
        this._lr = result.lr orelse this._lr;
        this._r0 = result.r0 orelse this._r0;
        this._r1 = result.r1 orelse this._r1;
        this._sp = result.sp orelse this._sp;
        this._fault = @intFromEnum(fault);
    }

    fn executeNop(this: *Cpu) InstructionResult {
        // NOP - Do nothing
        return .{ .fault = this.faultIfReservedBitsSet(v2mp.instruction_arg_field_mask) };
    }

    fn executeAdd(this: *Cpu) InstructionResult {
        return executeAddOrSub(this, true);
    }

    fn executeSub(this: *Cpu) InstructionResult {
        return executeAddOrSub(this, false);
    }

    fn executeAddOrSub(this: *Cpu, is_add: bool) InstructionResult {
        const Layout = struct {
            fn sourceRegIndex(instr: v2mp.Word) v2mp.RegisterIndex {
                return @enumFromInt((instr & 0x0C00) >> 10);
            }

            fn destRegIndex(instr: v2mp.Word) v2mp.RegisterIndex {
                return @enumFromInt((instr & 0x0300) >> 8);
            }

            fn value(instr: v2mp.Word) v2mp.Word {
                return instr & 0x00FF;
            }
        };

        const Result = struct {
            value: v2mp.Word,
            overflowed: bool,
        };

        const src_reg = Layout.sourceRegIndex(this._ir);
        const dest_reg = Layout.destRegIndex(this._ir);

        if (src_reg != dest_reg and Layout.value(this._ir) != 0) {
            // Should not have a literal value here, since
            // the source register is being used as a value.
            return .{ .fault = .res };
        }

        const stride: v2mp.Word = if (dest_reg == .pc) v2mp.size_of_word else v2mp.size_of_byte;
        const value: v2mp.Word = if (src_reg != dest_reg) this.getRegisterValue(src_reg) else Layout.value(this._ir);

        const op_result: Result = compute: {
            if (is_add) {
                const result_value: v2mp.Word = value +% (stride * value);
                break :compute .{ .value = result_value, .overflowed = result_value < this.getRegisterValue(dest_reg) };
            } else {
                const result_value: v2mp.Word = value -% (stride * value);
                break :compute .{ .value = result_value, .overflowed = result_value > this.getRegisterValue(dest_reg) };
            }
        };

        var sr: v2mp.Word = 0;

        if (op_result.overflowed) {
            sr |= v2mp.StatusRegFlag.c;
        }

        if (op_result.value == 0) {
            sr |= v2mp.StatusRegFlag.z;
        }

        var result: InstructionResult = .{ .sr = sr };
        result.setRegisterValue(dest_reg, op_result.value);

        return result;
    }

    fn executeUnassigned(_: *Cpu) InstructionResult {
        return .{ .fault = .ini };
    }

    fn faultIfReservedBitsSet(this: *const Cpu, reserved_mask: v2mp.Word) v2mp.Fault {
        return if (v2mp.reservedBitsSet(this._ir, reserved_mask)) .res else .none;
    }
};
