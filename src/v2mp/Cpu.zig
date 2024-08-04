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

const MulDivLayout = struct {
    pub const mask_resbits: defs.Word = 0x0100;

    pub fn destRegIndex(instr: defs.Word) defs.RegisterIndex {
        const mask_dest_reg_is_r1: defs.Word = 0x0800;
        return if ((instr & mask_dest_reg_is_r1) != 0) .r1 else .r0;
    }

    pub fn sourceIsStatic(instr: defs.Word) bool {
        const mask_static: defs.Word = 0x0400;
        return (instr & mask_static) != 0;
    }

    pub fn isSigned(instr: defs.Word) bool {
        const mask_signed: defs.Word = 0x0200;
        return (instr & mask_signed) != 0;
    }

    pub fn unsignedValue(instr: defs.Word) u8 {
        const mask_value: defs.Word = 0x00FF;
        return @truncate(instr & mask_value);
    }

    pub fn signedValue(instr: defs.Word) i8 {
        return @bitCast(unsignedValue(instr));
    }
};

const FetchInstructionFn = *const fn (address: defs.Word) defs.InstructionFetchError!defs.Word;
const ExecInstructionFn = *const fn (this: *const Cpu) InstructionResult;

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
    &executeMul, // 0x03
    &executeDiv, // 0x04
    &executeAsgn, // 0x05
    &executeUnassigned, // 0x06
    &executeUnassigned, // 0x07
    &executeUnassigned, // 0x08
    &executeUnassigned, // 0x09
    &executeUnassigned, // 0x0a
    &executeUnassigned, // 0x0b
    &executeUnassigned, // 0x0c
    &executeUnassigned, // 0x0d
    &executeUnassigned, // 0x0e
    &executeUnassigned, // 0x0f
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

fn executeNop(this: *const Cpu) InstructionResult {
    // NOP - Do nothing
    return .{ .fault = this.faultIfReservedBitsSet(defs.instruction_arg_field_mask) };
}

fn executeAdd(this: *const Cpu) InstructionResult {
    return executeAddOrSub(this, true);
}

fn executeSub(this: *const Cpu) InstructionResult {
    return executeAddOrSub(this, false);
}

fn executeAddOrSub(this: *const Cpu, is_add: bool) InstructionResult {
    const Layout = struct {
        pub fn sourceRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_source_reg_index: defs.Word = 0x0C00;
            return @enumFromInt((instr & mask_source_reg_index) >> 10);
        }

        pub fn destRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_dest_reg_index: defs.Word = 0x0300;
            return @enumFromInt((instr & mask_dest_reg_index) >> 8);
        }

        pub fn value(instr: defs.Word) defs.Word {
            const mask_value: defs.Word = 0x00FF;
            return instr & mask_value;
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

fn executeMul(this: *const Cpu) InstructionResult {
    const Result = struct {
        upper: defs.Word,
        lower: defs.Word,
        overflowed: bool,
    };

    if (utils.reservedBitsSet(this._ir, MulDivLayout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const dest_reg: defs.RegisterIndex = MulDivLayout.destRegIndex(this._ir);
    const src_value: defs.Word = this.getMulDivSourceValue(dest_reg);

    const op_result: Result = compute: {
        if (MulDivLayout.isSigned(this._ir)) {
            const result: i32 = @as(i32, src_value) * @as(i32, this.getRegisterValue(dest_reg));
            const unsigned_result: u32 = @bitCast(result);
            const overflowed: bool = if (result > 0) result > std.math.maxInt(i16) else result < std.math.minInt(i16);
            const upper: defs.Word = @truncate((unsigned_result & 0xFFFF0000) >> 16);
            const lower: defs.Word = @truncate(unsigned_result & 0x0000FFFF);

            break :compute .{ .upper = upper, .lower = lower, .overflowed = overflowed };
        } else {
            const result: u32 = @as(u32, src_value) * @as(u32, this.getRegisterValue(dest_reg));
            const upper: defs.Word = @truncate((result & 0xFFFF0000) >> 16);
            const lower: defs.Word = @truncate(result & 0x0000FFFF);
            const overflowed = upper != 0;

            break :compute .{ .upper = upper, .lower = lower, .overflowed = overflowed };
        }
    };

    var sr: defs.Word = 0;

    if (op_result.overflowed) {
        sr |= defs.StatusRegFlag.c;
    }

    if (op_result.upper == 0 and op_result.lower == 0) {
        sr |= defs.StatusRegFlag.z;
    }

    var result: InstructionResult = .{ .sr = sr, .lr = op_result.upper };
    result.setRegisterValue(dest_reg, op_result.lower);

    return result;
}

fn executeDiv(this: *const Cpu) InstructionResult {
    const Result = struct {
        upper: defs.Word,
        lower: defs.Word,
    };

    if (utils.reservedBitsSet(this._ir, MulDivLayout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const dest_reg: defs.RegisterIndex = MulDivLayout.destRegIndex(this._ir);
    const src_value: defs.Word = this.getMulDivSourceValue(dest_reg);

    // Make sure we don't divide by zero!
    if (src_value == 0) {
        return .{ .fault = .div };
    }

    const op_result: Result = compute: {
        if (MulDivLayout.isSigned(this._ir)) {
            const signed_dest_value: i16 = @bitCast(this.getRegisterValue(dest_reg));
            const signed_src_value: i16 = @bitCast(src_value);
            const lower: defs.Word = @bitCast(@rem(signed_dest_value, signed_src_value));
            const upper: defs.Word = @bitCast(@divTrunc(signed_dest_value, signed_src_value));

            break :compute .{ .upper = upper, .lower = lower };
        } else {
            const dest_value: defs.Word = this.getRegisterValue(dest_reg);
            break :compute .{ .upper = dest_value % src_value, .lower = dest_value / src_value };
        }
    };

    var sr: defs.Word = 0;

    if (op_result.upper != 0) {
        sr |= defs.StatusRegFlag.c;
    }

    if (op_result.lower == 0) {
        sr |= defs.StatusRegFlag.z;
    }

    var result: InstructionResult = .{ .sr = sr, .lr = op_result.upper };
    result.setRegisterValue(dest_reg, op_result.lower);

    return result;
}

fn executeAsgn(this: *const Cpu) InstructionResult {
    const Layout = struct {
        pub fn sourceRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_source_reg_index: defs.Word = 0x0C00;
            return @enumFromInt((instr & mask_source_reg_index) >> 10);
        }

        pub fn destRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_dest_reg_index: defs.Word = 0x0300;
            return @enumFromInt((instr & mask_dest_reg_index) >> 8);
        }

        pub fn value(instr: defs.Word) defs.Word {
            const mask_value: defs.Word = 0x00FF;
            return instr & mask_value;
        }
    };

    const src_reg = Layout.sourceRegIndex(this._ir);
    const dest_reg = Layout.destRegIndex(this._ir);
    const value = if (src_reg == dest_reg) Layout.value(this._ir) else this.getRegisterValue(src_reg);

    if (src_reg != dest_reg and value != 0) {
        return .{ .fault = .res };
    }

    // Don't allow assigning to PC via an instruction literal,
    // since the range of the literal is too small to be useful.
    // We return a RES fault here since this combination of bits
    // could potentially be useful in the future for some other
    // operation, so we don't want to allow people to rely
    // on it for anything right now.
    if (src_reg == dest_reg and dest_reg == .pc) {
        return .{ .fault = .res };
    }

    const sr: defs.Word = if (value == 0) defs.StatusRegFlag.z else 0;

    var result: InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, value);

    return result;
}

fn executeUnassigned(_: *const Cpu) InstructionResult {
    return .{ .fault = .ini };
}

fn faultIfReservedBitsSet(this: *const Cpu, reserved_mask: defs.Word) defs.Fault {
    return if (utils.reservedBitsSet(this._ir, reserved_mask)) .res else .none;
}

fn getMulDivSourceValue(this: *const Cpu, dest_reg: defs.RegisterIndex) defs.Word {
    if (!MulDivLayout.sourceIsStatic(this._ir)) {
        // We take the value from a register.
        return this.getRegisterValue(if (dest_reg == .r1) .r0 else .r1);
    }

    // Otherwise, we take the value from the instruction word.
    if (MulDivLayout.isSigned(this._ir)) {
        // Make absolutely sure that the value is a 16-bit signed
        // type, then cast it back to an opaque word.
        const signed_value: i8 = MulDivLayout.signedValue(this._ir);
        const wide_signed_value: i16 = signed_value;
        return @bitCast(wide_signed_value);
    } else {
        return @as(defs.Word, MulDivLayout.unsignedValue(this._ir));
    }
}
