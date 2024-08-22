const std = @import("std");
const defs = @import("defs.zig");
const utils = @import("utils.zig");
const Cpu = @This();

const Internal = struct {
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

    const ExecInstructionFn = *const fn (this: *const Cpu) Internal.InstructionResult;

    r0: defs.Word = 0,
    r1: defs.Word = 0,
    lr: defs.Word = 0,
    pc: defs.Word = 0,
    sp: defs.Word = 0,
    sr: defs.Word = 0,
    ir: defs.Word = 0,
    fault: defs.Word = @intFromEnum(defs.Fault.none),

    exec_callbacks: [defs.max_instruction_opcodes]Internal.ExecInstructionFn = .{
        &executeNop, // 0x00
        &executeAdd, // 0x01
        &executeSub, // 0x02
        &executeMul, // 0x03
        &executeDiv, // 0x04
        &executeAsgn, // 0x05
        &executeShft, // 0x06
        &executeBitw, // 0x07
        &executeCbx, // 0x08
        &executeLdst, // 0x09
        &executeStk, // 0x0a
        &executeSig, // 0x0b
        &executeUnassigned, // 0x0c
        &executeUnassigned, // 0x0d
        &executeUnassigned, // 0x0e
        &executeUnassigned, // 0x0f
    },
};

const Callbacks = struct {
    const FetchInstructionFn = *const fn (address: defs.Word) defs.InstructionFetchError!defs.Word;
    const RequestLoadWordFromDsFn = *const fn (address: defs.Word, dest_reg: defs.RegisterIndex) void;
    const RequestStoreWordToDsFn = *const fn (address: defs.Word, word_to_store: defs.Word) void;
    const RequestStackPushFn = *const fn (reg_flags: u4) void;
    const RequestStackPopFn = *const fn (reg_flags: u4) void;
    const RaiseSignalFn = *const fn (signal: defs.Word, r1: defs.Word, lr: defs.Word, sp: defs.Word) void;

    fetch: FetchInstructionFn,
    request_load_word_from_ds: RequestLoadWordFromDsFn,
    request_store_word_to_ds: RequestStoreWordToDsFn,
    request_stack_push: RequestStackPushFn,
    request_stack_pop: RequestStackPopFn,
    raise_signal: RaiseSignalFn,
};

callbacks: Callbacks,
priv: Internal = .{},

pub fn getRegisterValue(this: *const Cpu, reg: defs.RegisterIndex) defs.Word {
    return switch (reg) {
        .r0 => this.priv.r0,
        .r1 => this.priv.r1,
        .lr => this.priv.lr,
        .pc => this.priv.pc,
    };
}

pub fn getR0(this: *const Cpu) defs.Word {
    return this.priv.r0;
}

pub fn getR1(this: *const Cpu) defs.Word {
    return this.priv.r1;
}

pub fn getLr(this: *const Cpu) defs.Word {
    return this.priv.lr;
}

pub fn getPc(this: *const Cpu) defs.Word {
    return this.priv.pc;
}

pub fn getSr(this: *const Cpu) defs.Word {
    return this.priv.sr;
}

pub fn getSp(this: *const Cpu) defs.Word {
    return this.priv.sp;
}

pub fn getIr(this: *const Cpu) defs.Word {
    return this.priv.ir;
}

pub fn getFault(this: *const Cpu) defs.Word {
    return this.priv.fault;
}

pub fn reset(this: *Cpu) void {
    this.priv.r0 = 0;
    this.priv.r1 = 0;
    this.priv.lr = 0;
    this.priv.pc = 0;
    this.priv.sp = 0;
    this.priv.sr = 0;
    this.priv.ir = 0;
    this.priv.fault = 0;
}

pub fn fetchDecodeExecute(this: *Cpu) void {
    if (this.callbacks.fetch(this.priv.pc)) |instruction| {
        this.priv.ir = instruction;
    } else |err| {
        this.priv.fault = switch (err) {
            defs.InstructionFetchError.UnalignedMemoryAccess => utils.makeFaultWord(defs.Fault.algn, 0),
            defs.InstructionFetchError.SegmentationFault => utils.makeFaultWord(defs.Fault.seg, 0),
        };

        return;
    }

    // Increment PC now, as some instructions rely on it holding the next address
    this.priv.pc +%= @as(defs.Word, @sizeOf(defs.Word));

    const instruction_op: usize = @intFromEnum(utils.instructionOpCodeFromWord(this.priv.ir));
    std.debug.assert(instruction_op <= this.priv.exec_callbacks.len);

    const result = this.priv.exec_callbacks[instruction_op](this);
    this.acceptResult(result);
}

fn acceptResult(this: *Cpu, result: Internal.InstructionResult) void {
    const fault: defs.Fault = choose_fault: {
        if (result.fault) |res_fault| {
            // If the instruction caused a fault, this should take precedence.
            break :choose_fault res_fault;
        } else if (result.pc == null and this.priv.pc -% @as(defs.Word, @sizeOf(defs.Word)) > this.priv.pc) {
            // The instruction did not set the program counter, and the increment
            // that happened earlier caused an overflow. We should catch this case.
            break :choose_fault .seg;
        } else {
            // Nothing went wrong.
            break :choose_fault .none;
        }
    };

    this.priv.sr = result.sr orelse this.priv.sr;
    this.priv.lr = result.lr orelse this.priv.lr;
    this.priv.r0 = result.r0 orelse this.priv.r0;
    this.priv.r1 = result.r1 orelse this.priv.r1;
    this.priv.sp = result.sp orelse this.priv.sp;
    this.priv.fault = @intFromEnum(fault);
}

fn executeNop(this: *const Cpu) Internal.InstructionResult {
    // NOP - Do nothing
    return .{ .fault = this.faultIfReservedBitsSet(defs.instruction_arg_field_mask) };
}

fn executeAdd(this: *const Cpu) Internal.InstructionResult {
    return executeAddOrSub(this, true);
}

fn executeSub(this: *const Cpu) Internal.InstructionResult {
    return executeAddOrSub(this, false);
}

fn executeAddOrSub(this: *const Cpu, is_add: bool) Internal.InstructionResult {
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

    const src_reg = Layout.sourceRegIndex(this.priv.ir);
    const dest_reg = Layout.destRegIndex(this.priv.ir);

    if (src_reg != dest_reg and Layout.value(this.priv.ir) != 0) {
        // Should not have a literal value here, since
        // the source register is being used as a value.
        return .{ .fault = .res };
    }

    const stride: defs.Word = if (dest_reg == .pc) @sizeOf(defs.Word) else @sizeOf(defs.Byte);
    const value: defs.Word = if (src_reg != dest_reg) this.getRegisterValue(src_reg) else Layout.value(this.priv.ir);

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

    var result: Internal.InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, op_result.value);

    return result;
}

fn executeMul(this: *const Cpu) Internal.InstructionResult {
    const Result = struct {
        upper: defs.Word,
        lower: defs.Word,
        overflowed: bool,
    };

    if (utils.reservedBitsSet(this.priv.ir, Internal.MulDivLayout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const dest_reg: defs.RegisterIndex = Internal.MulDivLayout.destRegIndex(this.priv.ir);
    const src_value: defs.Word = this.getMulDivSourceValue(dest_reg);

    const op_result: Result = compute: {
        if (Internal.MulDivLayout.isSigned(this.priv.ir)) {
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

    var result: Internal.InstructionResult = .{ .sr = sr, .lr = op_result.upper };
    result.setRegisterValue(dest_reg, op_result.lower);

    return result;
}

fn executeDiv(this: *const Cpu) Internal.InstructionResult {
    const Result = struct {
        upper: defs.Word,
        lower: defs.Word,
    };

    if (utils.reservedBitsSet(this.priv.ir, Internal.MulDivLayout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const dest_reg: defs.RegisterIndex = Internal.MulDivLayout.destRegIndex(this.priv.ir);
    const src_value: defs.Word = this.getMulDivSourceValue(dest_reg);

    // Make sure we don't divide by zero!
    if (src_value == 0) {
        return .{ .fault = .div };
    }

    const op_result: Result = compute: {
        if (Internal.MulDivLayout.isSigned(this.priv.ir)) {
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

    var result: Internal.InstructionResult = .{ .sr = sr, .lr = op_result.upper };
    result.setRegisterValue(dest_reg, op_result.lower);

    return result;
}

fn executeAsgn(this: *const Cpu) Internal.InstructionResult {
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

    const src_reg = Layout.sourceRegIndex(this.priv.ir);
    const dest_reg = Layout.destRegIndex(this.priv.ir);
    const value = if (src_reg == dest_reg) Layout.value(this.priv.ir) else this.getRegisterValue(src_reg);

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

    var result: Internal.InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, value);

    return result;
}

fn executeShft(this: *const Cpu) Internal.InstructionResult {
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

    if (utils.reservedBitsSet(this.priv.ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const src_reg = Layout.sourceRegIndex(this.priv.ir);
    const dest_reg = Layout.destRegIndex(this.priv.ir);

    const signed_shift_value: i5 = get_signed_shift_value: {
        if (src_reg == dest_reg) {
            break :get_signed_shift_value Layout.value(this.priv.ir);
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

    var result: Internal.InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, shifted_value);

    return result;
}

fn executeBitw(this: *const Cpu) Internal.InstructionResult {
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

    if (utils.reservedBitsSet(this.priv.ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const src_reg: defs.RegisterIndex = Layout.sourceRegIndex(this.priv.ir);
    const dest_reg: defs.RegisterIndex = Layout.destRegIndex(this.priv.ir);
    const flip_or_shift_set: bool = Layout.maskShift(this.priv.ir) != 0 or Layout.shouldFlipMask(this.priv.ir);

    const reserved_bits_set: bool = //
        (src_reg != dest_reg and flip_or_shift_set) or //
        (src_reg == dest_reg and Layout.operationType(this.priv.ir) == .bit_not and flip_or_shift_set);

    if (reserved_bits_set) {
        return .{ .fault = .res };
    }

    const bitmask: defs.Word = create_bitmask: {
        if (src_reg != dest_reg) {
            break :create_bitmask this.getRegisterValue(src_reg);
        } else {
            const mask_shift: u4 = Layout.maskShift(this.priv.ir);
            const initial_bit: defs.Word = 1;

            if (Layout.shouldFlipMask(this.priv.ir)) {
                break :create_bitmask ~(initial_bit << mask_shift);
            } else {
                break :create_bitmask (initial_bit << mask_shift);
            }
        }
    };

    const dest_reg_value: defs.Word = this.getRegisterValue(dest_reg);

    const op_result = switch (Layout.operationType(this.priv.ir)) {
        .bit_and => dest_reg_value & bitmask,
        .bit_or => dest_reg_value | bitmask,
        .bit_xor => dest_reg_value ^ bitmask,
        .bit_not => ~dest_reg_value,
    };

    var sr: defs.Word = 0;

    if (op_result == 0) {
        sr |= defs.StatusRegFlag.z;
    }

    var result: Internal.InstructionResult = .{ .sr = sr };
    result.setRegisterValue(dest_reg, op_result);

    return result;
}

fn executeCbx(this: *const Cpu) Internal.InstructionResult {
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

    if (utils.reservedBitsSet(this.priv.ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    if (Layout.lrIsTarget(this.priv.ir) and Layout.offset(this.priv.ir) != 0) {
        return .{ .fault = .res };
    }

    const should_branch: bool = //
        (Layout.branchOnSrZ(this.priv.ir) and (this.priv.sr & defs.StatusRegFlag.z) != 0) or //
        (Layout.branchOnSrC(this.priv.ir) and (this.priv.sr & defs.StatusRegFlag.c) != 0);

    if (should_branch) {
        if (Layout.lrIsTarget(this.priv.ir)) {
            return .{ .sr = 0, .pc = this.getLr() };
        } else {
            const pc_offset: defs.Word = @as(u8, @sizeOf(defs.Word)) * Layout.offset(this.priv.ir);
            return .{ .sr = 0, .pc = this.getPc() +% pc_offset };
        }
    }

    return .{ .sr = defs.StatusRegFlag.z };
}

fn executeLdst(this: *const Cpu) Internal.InstructionResult {
    const Layout = struct {
        pub const mask_resbits: defs.Word = 0x01FF;

        pub fn operationIsStore(instr: defs.Word) bool {
            const mask_operation_is_store: defs.Word = 0x0800;
            return (instr & mask_operation_is_store) != 0;
        }

        pub fn registerIndex(instr: defs.Word) defs.RegisterIndex {
            const mask_register_index: defs.Word = 0x0600;
            return @enumFromInt((instr & mask_register_index) >> 9);
        }
    };

    if (utils.reservedBitsSet(this.priv.ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    const target_reg: defs.RegisterIndex = Layout.registerIndex(this.priv.ir);

    return result: {
        if (Layout.operationIsStore(this.priv.ir)) {
            const value: defs.Word = this.getRegisterValue(target_reg);
            this.callbacks.request_store_word_to_ds(this.priv.lr, value);

            break :result .{ .sr = if (value == 0) defs.StatusRegFlag.z else 0 };
        } else {
            this.callbacks.request_load_word_from_ds(this.priv.lr, Layout.registerIndex(this.priv.ir));

            // The status register will be updated later.
            break :result .{};
        }
    };
}

fn executeStk(this: *const Cpu) Internal.InstructionResult {
    const Layout = struct {
        pub const mask_resbits: defs.Word = 0x07F0;

        pub fn operationIsPush(instr: defs.Word) bool {
            const mask_operation_is_push: defs.Word = 0x0800;
            return (instr & mask_operation_is_push) != 0;
        }

        pub fn includesR0(instr: defs.Word) bool {
            const mask: defs.Word = 0x0001;
            return (instr & mask) != 0;
        }

        pub fn includesR1(instr: defs.Word) bool {
            const mask: defs.Word = 0x0002;
            return (instr & mask) != 0;
        }

        pub fn includesLr(instr: defs.Word) bool {
            const mask: defs.Word = 0x0004;
            return (instr & mask) != 0;
        }

        pub fn includesPc(instr: defs.Word) bool {
            const mask: defs.Word = 0x0008;
            return (instr & mask) != 0;
        }
    };

    if (utils.reservedBitsSet(this.priv.ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    var reg_flags: u4 = 0;

    if (Layout.includesR0(this.priv.ir)) {
        reg_flags |= 1 << @intFromEnum(defs.RegisterIndex.r0);
    }

    if (Layout.includesR1(this.priv.ir)) {
        reg_flags |= 1 << @intFromEnum(defs.RegisterIndex.r1);
    }

    if (Layout.includesLr(this.priv.ir)) {
        reg_flags |= 1 << @intFromEnum(defs.RegisterIndex.lr);
    }

    if (Layout.includesPc(this.priv.ir)) {
        reg_flags |= 1 << @intFromEnum(defs.RegisterIndex.pc);
    }

    if (Layout.operationIsPush(this.priv.ir)) {
        this.callbacks.request_stack_push(reg_flags);
    } else {
        this.callbacks.request_stack_pop(reg_flags);
    }

    return .{};
}

fn executeSig(this: *const Cpu) Internal.InstructionResult {
    const Layout = struct {
        const mask_resbits: defs.Word = 0x0FFF;
    };

    if (utils.reservedBitsSet(this.priv.ir, Layout.mask_resbits)) {
        return .{ .fault = .res };
    }

    this.callbacks.raise_signal(this.priv.r0, this.priv.r1, this.priv.lr, this.priv.sp);
    return .{};
}

fn executeUnassigned(_: *const Cpu) Internal.InstructionResult {
    return .{ .fault = .ini };
}

fn faultIfReservedBitsSet(this: *const Cpu, reserved_mask: defs.Word) defs.Fault {
    return if (utils.reservedBitsSet(this.priv.ir, reserved_mask)) .res else .none;
}

fn getMulDivSourceValue(this: *const Cpu, dest_reg: defs.RegisterIndex) defs.Word {
    if (!Internal.MulDivLayout.sourceIsStatic(this.priv.ir)) {
        // We take the value from a register.
        return this.getRegisterValue(if (dest_reg == .r1) .r0 else .r1);
    }

    // Otherwise, we take the value from the instruction word.
    if (Internal.MulDivLayout.isSigned(this.priv.ir)) {
        // Make absolutely sure that the value is a 16-bit signed
        // type, then cast it back to an opaque word.
        const signed_value: i8 = Internal.MulDivLayout.signedValue(this.priv.ir);
        const wide_signed_value: i16 = signed_value;
        return @bitCast(wide_signed_value);
    } else {
        return @as(defs.Word, Internal.MulDivLayout.unsignedValue(this.priv.ir));
    }
}
