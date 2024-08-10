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
    &executeShft, // 0x06
    &executeBitw, // 0x07
    &executeCbx, // 0x08
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

fn executeShft(this: *const Cpu) InstructionResult {
    const Layout = struct {
        pub const mask_resbits: defs.Word = 0x00E0;

        pub fn sourceRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_source_reg_index: defs.Word = 0x0C00;
            return @enumFromInt((instr & mask_source_reg_index) >> 10);
        }

        pub fn destRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_dest_reg_index: defs.Word = 0x0300;
            return @enumFromInt((instr & mask_dest_reg_index) >> 8);
        }

        pub fn value(instr: defs.Word) i5 {
            const masked_value: u5 = @truncate(instr & 0x001F);
            return @bitCast(masked_value);
        }
    };

    if (utils.reservedBitsSet(this._ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const src_reg = Layout.sourceRegIndex(this._ir);
    const dest_reg = Layout.destRegIndex(this._ir);

    const signed_shift_value: i5 = get_signed_shift_value: {
        if (src_reg == dest_reg) {
            break :get_signed_shift_value Layout.value(this._ir);
        } else {
            const reg_value: u5 = @truncate(this.getRegisterValue(src_reg));
            break :get_signed_shift_value @bitCast(reg_value);
        }
    };

    if (src_reg != dest_reg and signed_shift_value != 0) {
        return .{ .fault = .res };
    }

    const dest_reg_value: defs.Word = this.getRegisterValue(dest_reg);

    const shifted_value: defs.Word = do_shift: {
        if (signed_shift_value > 0) {
            const truncated_signed_shift_value: i4 = @truncate(signed_shift_value);
            const unsigned_shift_value: u4 = @bitCast(truncated_signed_shift_value);
            break :do_shift dest_reg_value << unsigned_shift_value;
        } else if (signed_shift_value < 0) {
            const truncated_signed_shift_value: i4 = @truncate(-1 * signed_shift_value);
            const unsigned_shift_value: u4 = @bitCast(truncated_signed_shift_value);
            break :do_shift dest_reg_value >> unsigned_shift_value;
        } else {
            break :do_shift dest_reg_value;
        }
    };

    // To check whether any bits got shifted off the end,
    // we can mask the original value with the span of bits
    // that did get shifted off the end. If the intersection
    // of these two values is not zero, bits in the register
    // were shifted off the end.
    // This block is done in very specific steps because
    // the Zig compiler is incredibly pedantic about bit
    // casts and the magnitude of shift operations.
    const shifted_off_end: bool = check_shift: {
        if (signed_shift_value > 0) {
            const magnitude: u5 = @bitCast(signed_shift_value);
            const all_ones: defs.Word = 0xFFFF;
            const check_mask: defs.Word = all_ones << @truncate(16 - magnitude);
            break :check_shift (dest_reg_value & check_mask) != 0;
        } else if (signed_shift_value < 0) {
            const magnitude: u5 = @bitCast(-1 * signed_shift_value);
            const all_ones: defs.Word = 0xFFFF;
            const check_mask = all_ones >> @truncate(16 - magnitude);
            break :check_shift (dest_reg_value & check_mask) != 0;
        } else {
            break :check_shift false;
        }
    };

    var sr: defs.Word = 0;

    if (shifted_off_end) {
        sr |= defs.StatusRegFlag.c;
    }

    if (shifted_value == 0) {
        sr |= defs.StatusRegFlag.z;
    }

    var result: InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, shifted_value);

    return result;
}

fn executeBitw(this: *const Cpu) InstructionResult {
    const Layout = struct {
        pub const mask_resbits: defs.Word = 0x0010;

        pub fn sourceRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_source_reg_index: defs.Word = 0x0C00;
            return @enumFromInt((instr & mask_source_reg_index) >> 10);
        }

        pub fn destRegIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_dest_reg_index: defs.Word = 0x0300;
            return @enumFromInt((instr & mask_dest_reg_index) >> 8);
        }

        pub fn operationType(instr: defs.Word) defs.BitwiseOp {
            const mask_operation_type: defs.Word = 0x00C0;
            return @enumFromInt((instr & mask_operation_type) >> 6);
        }

        pub fn shouldFlipMask(instr: defs.Word) bool {
            const mask_flip: defs.Word = 0x0020;
            return (instr & mask_flip) != 0;
        }

        pub fn maskShift(instr: defs.Word) u4 {
            // Masking is technically redundant here since we
            // just truncate the high bits, but it makes the
            // intent clearer.
            const mask_shift: defs.Word = 0x000F;
            return @truncate(instr & mask_shift);
        }
    };

    if (utils.reservedBitsSet(this._ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const src_reg: defs.RegisterIndex = Layout.sourceRegIndex(this._ir);
    const dest_reg: defs.RegisterIndex = Layout.destRegIndex(this._ir);
    const flip_or_shift_set: bool = Layout.maskShift(this._ir) != 0 or Layout.shouldFlipMask(this._ir);

    const reserved_bits_set: bool = //
        (src_reg != dest_reg and flip_or_shift_set) or //
        (src_reg == dest_reg and Layout.operationType(this._ir) == .bit_not and flip_or_shift_set);

    if (reserved_bits_set) {
        return .{ .fault = .res };
    }

    const bitmask: defs.Word = create_bitmask: {
        if (src_reg != dest_reg) {
            break :create_bitmask this.getRegisterValue(src_reg);
        } else {
            const mask_shift: u4 = Layout.maskShift(this._ir);
            const initial_bit: defs.Word = 1;

            if (Layout.shouldFlipMask(this._ir)) {
                break :create_bitmask ~(initial_bit << mask_shift);
            } else {
                break :create_bitmask (initial_bit << mask_shift);
            }
        }
    };

    const dest_reg_value: defs.Word = this.getRegisterValue(dest_reg);

    const op_result = switch (Layout.operationType(this._ir)) {
        .bit_and => dest_reg_value & bitmask,
        .bit_or => dest_reg_value | bitmask,
        .bit_xor => dest_reg_value ^ bitmask,
        .bit_not => ~dest_reg_value,
    };

    var sr: defs.Word = 0;

    if (op_result == 0) {
        sr |= defs.StatusRegFlag.z;
    }

    var result: InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, op_result);

    return result;
}

fn executeCbx(this: *const Cpu) InstructionResult {
    const Layout = struct {
        pub const mask_resbits: defs.Word = 0x0300;

        pub fn lrIsTarget(instr: defs.Word) bool {
            const mask_lr_is_target: defs.Word = 0x0800;
            return (instr & mask_lr_is_target) != 0;
        }

        pub fn branchOnSrZ(instr: defs.Word) bool {
            const mask_branch_on_sr_z: defs.Word = 0x0400;
            return (instr & mask_branch_on_sr_z) == 0;
        }

        pub fn branchOnSrC(instr: defs.Word) bool {
            return !branchOnSrZ(instr);
        }

        pub fn offset(instr: defs.Word) u8 {
            // Masking is technically redundant here since we
            // just truncate the high bits, but it makes the
            // intent clearer.
            const mask_offset: defs.Word = 0x00FF;
            return @truncate(instr & mask_offset);
        }
    };

    if (utils.reservedBitsSet(this._ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    if (Layout.lrIsTarget(this._ir) and Layout.offset(this._ir) != 0) {
        return .{ .fault = .res };
    }

    const should_branch: bool = //
        (Layout.branchOnSrZ(this._ir) and (this._sr & defs.StatusRegFlag.z) != 0) or //
        (Layout.branchOnSrC(this._ir) and (this._sr & defs.StatusRegFlag.c) != 0);

    if (should_branch) {
        if (Layout.lrIsTarget(this._ir)) {
            return .{ .sr = 0, .pc = this.getLr() };
        } else {
            const pc_offset: defs.Word = @as(u8, defs.size_of_word) * Layout.offset(this._ir);
            return .{ .sr = 0, .pc = this.getPc() +% pc_offset };
        }
    }

    return .{ .sr = defs.StatusRegFlag.z };
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
