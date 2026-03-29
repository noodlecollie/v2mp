use crate::arch::*;

pub const fn addr(source_reg: RegisterIndex, dest_reg: RegisterIndex) -> Word
{
	return OpCode::Add.as_register_value()
		| (source_reg.as_value() << 10)
		| (dest_reg.as_value() << 8);
}

pub const fn addl(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	return OpCode::Add.as_register_value()
		| (dest_reg.as_value() << 10)
		| (dest_reg.as_value() << 8)
		| (literal as Word);
}

pub const fn subr(source_reg: RegisterIndex, dest_reg: RegisterIndex) -> Word
{
	return OpCode::Sub.as_register_value()
		| (source_reg.as_value() << 10)
		| (dest_reg.as_value() << 8);
}

pub const fn subl(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	return OpCode::Sub.as_register_value()
		| (dest_reg.as_value() << 10)
		| (dest_reg.as_value() << 8)
		| (literal as Word);
}

pub const fn mulru(dest_reg: RegisterIndex) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Mul.as_register_value() | (dest_is_r1 << 11);
}

pub const fn mulrs(dest_reg: RegisterIndex) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Mul.as_register_value() | (dest_is_r1 << 11) | (1 << 9);
}

pub const fn mullu(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Mul.as_register_value() | (dest_is_r1 << 11) | (1 << 10) | (literal as Word);
}

pub const fn mulls(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Mul.as_register_value()
		| (dest_is_r1 << 11)
		| (1 << 10)
		| (1 << 9)
		| (literal as Word);
}

pub const fn divru(dest_reg: RegisterIndex) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Div.as_register_value() | (dest_is_r1 << 11);
}

pub const fn divrs(dest_reg: RegisterIndex) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Div.as_register_value() | (dest_is_r1 << 11) | (1 << 9);
}

pub const fn divlu(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Div.as_register_value() | (dest_is_r1 << 11) | (1 << 10) | (literal as Word);
}

pub const fn divls(dest_reg: RegisterIndex, literal: Byte) -> Word
{
	let dest_is_r1: Word = match dest_reg
	{
		RegisterIndex::R0 => 0,
		RegisterIndex::R1 => 1,
		_ => panic!("Invalid register for operation"),
	};

	return OpCode::Div.as_register_value()
		| (dest_is_r1 << 11)
		| (1 << 10)
		| (1 << 9)
		| (literal as Word);
}
