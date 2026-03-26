use crate::arch::*;

const fn instruction(opcode: OpCode) -> Word
{
	return opcode.as_value() << 12;
}

pub const fn addr(source_reg: RegisterIndex, dest_reg: RegisterIndex) -> Word
{
	return instruction(OpCode::Add) | (source_reg.as_value() << 10) | (dest_reg.as_value() << 8);
}
