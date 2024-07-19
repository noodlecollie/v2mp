const std = @import("std");

pub const Word = u16;
pub const Byte = u8;

// Because @sizeOf() may contain padding depending on the target being compiled for,
// we don't use it here. Instead, this is an alias for a tedious builtin to have to
// type out all the time.
pub const size_of_word: usize = @TypeOf(Word).Int.bits / 8;

pub const InstructionField = u4;
pub const InstructionArgField = u12;
pub const FaultField = u4;
pub const SignalField = Word;
pub const RegisterIndexField = u2;
pub const BitwiseOpField = u2;

pub const instruction_opcode_field_mask: Word = std.math.maxInt(Word) << @bitSizeOf(InstructionArgField);
pub const instruction_arg_field_mask: Word = std.math.maxInt(Word) >> (@bitSizeOf(Word) - @bitSizeOf(InstructionArgField));

pub const Instruction = enum(InstructionField) {
    nop = 0x0,
    add = 0x1,
    sub = 0x2,
    mul = 0x3,
    div = 0x4,
    asgn = 0x5,
    shft = 0x6,
    bitw = 0x7,
    cbx = 0x8,
    ldst = 0x9,
    stk = 0xA,
    sig = 0xB,
    unassigned0 = 0xC,
    unassigned1 = 0xD,
    unassigned2 = 0xE,
    unassigned3 = 0xF,
};

pub const Fault = enum(FaultField) {
    none = 0x0,
    res = 0x1,
    algn = 0x2,
    seg = 0x3,
    ini = 0x4,
    sof = 0x5,
    div = 0x6,
    ins = 0x7,
    spv = 0x8,
    unassigned0 = 0x9,
    unassigned1 = 0xA,
    unassigned2 = 0xB,
    unassigned3 = 0xC,
    unassigned4 = 0xD,
    unassigned5 = 0xE,
    unassigned6 = 0xF,
};

pub const Signal = enum(SignalField) {
    end_program = 0x0000,
};

pub const RegisterIndex = enum(RegisterIndexField) {
    r0 = 0x0,
    r1 = 0x1,
    lr = 0x2,
    pc = 0x3,
};

pub const BitwiseOp = enum(BitwiseOpField) {
    op_and = 0x0,
    op_or = 0x1,
    op_xor = 0x2,
    op_not = 0x3,
};

pub const StatusRegFlag = struct {
    const z: Word = 1 << 0;
    const c: Word = 1 << 1;
};

pub fn makeFaultWord(fault: Fault, args: InstructionArgField) Word {
    return (@intFromEnum(fault) << @bitSizeOf(InstructionArgField)) | @intFromEnum(args);
}

pub fn faultCodeFromWord(fault_word: Word) Fault {
    return @enumFromInt(fault_word >> @bitSizeOf(InstructionArgField));
}

pub fn faultArgsFromWord(fault_word: Word) InstructionArgField {
    return fault_word & instruction_arg_field_mask;
}

pub fn instructionOpCodeFromWord(instruction: Word) Instruction {
    return @enumFromInt((instruction & instruction_opcode_field_mask) >> @bitSizeOf(InstructionArgField));
}

pub fn instructionArgsFromWord(instruction: Word) InstructionArgField {
    return instruction & instruction_arg_field_mask;
}
