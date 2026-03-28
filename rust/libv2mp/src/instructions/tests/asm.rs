use crate::arch::*;

const fn instruction(opcode: OpCode) -> Word
{
	return opcode.as_value() << 12;
}

pub const fn addr(source_reg: RegisterIndex, dest_reg: RegisterIndex) -> Word
{
	return instruction(OpCode::Add) | (source_reg.as_value() << 10) | (dest_reg.as_value() << 8);
}

pub const fn addl(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	return instruction(OpCode::Add)
		| (dest_reg.as_value() << 10)
		| (dest_reg.as_value() << 8)
		| (literal as Word);
}

pub const fn subr(source_reg: RegisterIndex, dest_reg: RegisterIndex) -> Word
{
	return instruction(OpCode::Sub) | (source_reg.as_value() << 10) | (dest_reg.as_value() << 8);
}

pub const fn subl(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	return instruction(OpCode::Sub)
		| (dest_reg.as_value() << 10)
		| (dest_reg.as_value() << 8)
		| (literal as Word);
}
