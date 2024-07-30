const std = @import("std");

pub usingnamespace @import("defs.zig");

pub fn makeFaultWord(fault: Fault, args: InstructionArgField) Word {
    return (@as(Word, @intFromEnum(fault)) << @bitSizeOf(InstructionArgField)) | @as(Word, args);
}

pub fn faultCodeFromWord(fault_word: Word) Fault {
    return @enumFromInt(fault_word >> @bitSizeOf(InstructionArgField));
}

pub fn faultArgsFromWord(fault_word: Word) InstructionArgField {
    return fault_word & instruction_arg_field_mask;
}

pub fn faultNameFromWord(fault_word: Word) [:0]const u8 {
    return @tagName(faultCodeFromWord(fault_word));
}

pub fn instructionOpCodeFromWord(instruction: Word) Instruction {
    return @enumFromInt((instruction & instruction_opcode_field_mask) >> @bitSizeOf(InstructionArgField));
}

pub fn instructionArgsFromWord(instruction: Word) InstructionArgField {
    return instruction & instruction_arg_field_mask;
}

pub fn reservedBitsSet(instruction: Word, reserved_mask: Word) bool {
    return instruction & (instruction_arg_field_mask & reserved_mask) != 0;
}
