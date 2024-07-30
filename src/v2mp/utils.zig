const std = @import("std");

const defs = @import("defs.zig");

pub fn makeFaultWord(fault: defs.Fault, args: defs.InstructionArgField) defs.Word {
    return (@as(defs.Word, @intFromEnum(fault)) << @bitSizeOf(defs.InstructionArgField)) | @as(defs.Word, args);
}

pub fn faultCodeFromWord(fault_word: defs.Word) defs.Fault {
    return @enumFromInt(fault_word >> @bitSizeOf(defs.InstructionArgField));
}

pub fn faultArgsFromWord(fault_word: defs.Word) defs.InstructionArgField {
    return fault_word & defs.instruction_arg_field_mask;
}

pub fn faultNameFromWord(fault_word: defs.Word) [:0]const u8 {
    return @tagName(faultCodeFromWord(fault_word));
}

pub fn instructionOpCodeFromWord(instruction: defs.Word) defs.Instruction {
    return @enumFromInt((instruction & defs.instruction_opcode_field_mask) >> @bitSizeOf(defs.InstructionArgField));
}

pub fn instructionArgsFromWord(instruction: defs.Word) defs.InstructionArgField {
    return instruction & defs.instruction_arg_field_mask;
}

pub fn reservedBitsSet(instruction: defs.Word, reserved_mask: defs.Word) bool {
    return instruction & (defs.instruction_arg_field_mask & reserved_mask) != 0;
}
