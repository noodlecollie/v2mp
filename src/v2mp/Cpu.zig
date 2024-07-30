const std = @import("std");
const defs = @import("defs.zig");
const utils = @import("utils.zig");
const Cpu = @This();

const InstructionResult = struct {
    pc: ?defs.Word = null,
    sr: ?defs.Word = null,
    lr: ?defs.Word = null,
    r0: ?defs.Word = null,
    r1: ?defs.Word = null,
    sp: ?defs.Word = null,
    fault: ?defs.Fault = null,

    pub fn setRegisterValue(this: *InstructionResult, reg: defs.RegisterIndex, value: defs.Word) void {
        switch (reg) {
            .r0 => this.r0 = value,
            .r1 => this.r1 = value,
            .lr => this.lr = value,
            .pc => this.pc = value,
        }
    }
};

const FetchInstructionFn = *const fn (address: defs.Word) defs.InstructionFetchError!defs.Word;
const ExecInstructionFn = *const fn (this: *Cpu) InstructionResult;

fetch_callback: FetchInstructionFn,

_r0: defs.Word = 0,
_r1: defs.Word = 0,
_lr: defs.Word = 0,
_pc: defs.Word = 0,
_sp: defs.Word = 0,
_sr: defs.Word = 0,
_ir: defs.Word = 0,
_fault: defs.Word = @intFromEnum(defs.Fault.none),

_exec_callbacks: [defs.max_instruction_opcodes]ExecInstructionFn = .{
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

pub fn getRegisterValue(this: *const Cpu, reg: defs.RegisterIndex) defs.Word {
    return switch (reg) {
        .r0 => this._r0,
        .r1 => this._r1,
        .lr => this._lr,
        .pc => this._pc,
    };
}

pub fn getR0(this: *const Cpu) defs.Word {
    return this._r0;
}

pub fn getR1(this: *const Cpu) defs.Word {
    return this._r1;
}

pub fn getLr(this: *const Cpu) defs.Word {
    return this._lr;
}

pub fn getPc(this: *const Cpu) defs.Word {
    return this._pc;
}

pub fn getSr(this: *const Cpu) defs.Word {
    return this._sr;
}

pub fn getSp(this: *const Cpu) defs.Word {
    return this._sp;
}

pub fn getIr(this: *const Cpu) defs.Word {
    return this._ir;
}

pub fn getFault(this: *const Cpu) defs.Word {
    return this._fault;
}

pub fn reset(this: *Cpu) void {
    this._r0 = 0;
    this._r1 = 0;
    this._lr = 0;
    this._pc = 0;
    this._sp = 0;
    this._sr = 0;
    this._ir = 0;
    this._fault = 0;
}

pub fn fetchDecodeExecute(this: *Cpu) void {
    if (this.fetch_callback(this._pc)) |instruction| {
        this._ir = instruction;
    } else |err| {
        this._fault = switch (err) {
            defs.InstructionFetchError.UnalignedMemoryAccess => utils.makeFaultWord(defs.Fault.algn, 0),
            defs.InstructionFetchError.SegmentationFault => utils.makeFaultWord(defs.Fault.seg, 0),
        };

        return;
    }

    // Increment PC now, as some instructions rely on it holding the next address
    this._pc +%= @as(defs.Word, defs.size_of_word);

    const instruction_op: usize = @intFromEnum(utils.instructionOpCodeFromWord(this._ir));
    std.debug.assert(instruction_op <= this._exec_callbacks.len);

    const result = this._exec_callbacks[instruction_op](this);
    this.acceptResult(result);
}

fn acceptResult(this: *Cpu, result: InstructionResult) void {
    const fault: defs.Fault = choose_fault: {
        if (result.fault) |res_fault| {
            // If the instruction caused a fault, this should take precedence.
            break :choose_fault res_fault;
        } else if (result.pc == null and this._pc -% @as(defs.Word, defs.size_of_word) > this._pc) {
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
    return .{ .fault = this.faultIfReservedBitsSet(defs.instruction_arg_field_mask) };
}

fn executeAdd(this: *Cpu) InstructionResult {
    return executeAddOrSub(this, true);
}

fn executeSub(this: *Cpu) InstructionResult {
    return executeAddOrSub(this, false);
}

fn executeAddOrSub(this: *Cpu, is_add: bool) InstructionResult {
    const Layout = struct {
        pub fn sourceRegIndex(instr: defs.Word) defs.RegisterIndex {
            return @enumFromInt((instr & 0x0C00) >> 10);
        }

        pub fn destRegIndex(instr: defs.Word) defs.RegisterIndex {
            return @enumFromInt((instr & 0x0300) >> 8);
        }

        pub fn value(instr: defs.Word) defs.Word {
            return instr & 0x00FF;
        }
    };

    const Result = struct {
        value: defs.Word,
        overflowed: bool,
    };

    const src_reg = Layout.sourceRegIndex(this._ir);
    const dest_reg = Layout.destRegIndex(this._ir);

    if (src_reg != dest_reg and Layout.value(this._ir) != 0) {
        // Should not have a literal value here, since
        // the source register is being used as a value.
        return .{ .fault = .res };
    }

    const stride: defs.Word = if (dest_reg == .pc) defs.size_of_word else defs.size_of_byte;
    const value: defs.Word = if (src_reg != dest_reg) this.getRegisterValue(src_reg) else Layout.value(this._ir);

    const op_result: Result = compute: {
        if (is_add) {
            const result_value: defs.Word = value +% (stride * value);
            break :compute .{ .value = result_value, .overflowed = result_value < this.getRegisterValue(dest_reg) };
        } else {
            const result_value: defs.Word = value -% (stride * value);
            break :compute .{ .value = result_value, .overflowed = result_value > this.getRegisterValue(dest_reg) };
        }
    };

    var sr: defs.Word = 0;

    if (op_result.overflowed) {
        sr |= defs.StatusRegFlag.c;
    }

    if (op_result.value == 0) {
        sr |= defs.StatusRegFlag.z;
    }

    var result: InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, op_result.value);

    return result;
}

fn executeUnassigned(_: *Cpu) InstructionResult {
    return .{ .fault = .ini };
}

fn faultIfReservedBitsSet(this: *const Cpu, reserved_mask: defs.Word) defs.Fault {
    return if (utils.reservedBitsSet(this._ir, reserved_mask)) .res else .none;
}
