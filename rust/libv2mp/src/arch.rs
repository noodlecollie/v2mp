#[derive(Clone, Copy)]
pub struct Word(pub u16);

impl Word
{
	#[inline]
	pub const fn word(&self) -> u16
	{
		return self.0;
	}

	#[inline]
	pub const fn byte0(&self) -> u8
	{
		return (self.0 & 0xFF) as u8;
	}

	#[inline]
	pub const fn byte1(&self) -> u8
	{
		return ((self.0 & 0xFF00) >> 4) as u8;
	}

	#[inline]
	pub const fn bits(&self, mask: u16, offset: u8) -> u16
	{
		return (self.0 & mask) >> offset;
	}
}

pub const NUM_OPCODES: usize = 16;
pub const OPCODE_MASK: u16 = 0xF000;
pub const INSTRUCTION_ARG_MASK: u16 = 0x0FFF;
pub const OPCODE_MASK_BITS: u8 = 4;
pub const INSTRUCTION_ARG_BITS: u8 = 16 - OPCODE_MASK_BITS;

#[repr(u16)]
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum OpCode
{
	/// No operation
	Nop = 0x0,

	/// Add
	Add = 0x1,

	/// Subtract
	Sub = 0x2,

	/// Multiply
	Mul = 0x3,

	/// Divide
	Div = 0x4,

	/// Assign to register
	Asgn = 0x5,

	/// Bit shift
	Shft = 0x6,

	/// Bitwise operation
	Bitw = 0x7,

	/// Conditional branch
	Cbx = 0x8,

	/// Load or store
	Ldst = 0x9,

	/// Stack push or pop
	Stk = 0xA,

	/// Raise signal
	Sig = 0xB,

	/// Unassigned opcode 0xC
	Unassigned0 = 0xC,

	/// Unassigned opcode 0xD
	Unassigned1 = 0xD,

	/// Unassigned opcode 0xE
	Unassigned2 = 0xE,

	/// Unassigned opcode 0xF
	Unassigned3 = 0xF,
}

impl OpCode
{
	pub const fn to_value(&self) -> u16
	{
		// SAFETY: Matches the repr type specified for the enum,
		// so we can always convert to this type.
		return unsafe { *(self as *const Self as *const u16) };
	}

	pub const fn from_value(value: u16) -> Option<Self>
	{
		if (value as usize) < NUM_OPCODES
		{
			// SAFETY: Op-codes are contiguous, so any value in this range will be valid.
			return unsafe { Some(*(&value as *const u16 as *const Self)) };
		}

		return None;
	}

	pub const fn from_word(word: Word) -> Self
	{
		// This should never fail, as the OpCode enum completely
		// fills the bit width provided in the word.
		return Self::from_value(word.bits(OPCODE_MASK, INSTRUCTION_ARG_BITS))
			.expect("Failed to interpret opcode from instruction word");
	}
}

pub const NUM_FAULTS: usize = 16;
pub const FAULT_MASK: u16 = 0xF000;
pub const FAULT_ARG_MASK: u16 = 0x0FFF;
pub const FAULT_MASK_BITS: u8 = 4;
pub const FAULT_ARG_BITS: u8 = 16 - FAULT_MASK_BITS;

#[repr(u16)]
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum FaultCode
{
	/// No fault
	None = 0x0,

	// Reserved bits set
	Res = 0x1,

	/// Alignment violation
	Algn = 0x2,

	/// Segment access violation
	Seg = 0x3,

	/// Invalid instruction
	Ini = 0x4,

	/// Stack overflow or underflow
	Sof = 0x5,

	/// Division by zero
	Div = 0x6,

	/// Invalid signal
	Ins = 0x7,

	/// Supervisor error
	Spv = 0x8,

	// Unassigned fault code 0x9
	Unassigned0 = 0x9,

	// Unassigned fault code 0xA
	Unassigned1 = 0xA,

	// Unassigned fault code 0xB
	Unassigned2 = 0xB,

	// Unassigned fault code 0xC
	Unassigned3 = 0xC,

	// Unassigned fault code 0xD
	Unassigned4 = 0xD,

	// Unassigned fault code 0xE
	Unassigned5 = 0xE,

	// Unassigned fault code 0xF
	Unassigned6 = 0xF,
}

impl FaultCode
{
	pub const fn to_value(&self) -> u16
	{
		// SAFETY: Matches the repr type specified for the enum,
		// so we can always convert to this type.
		return unsafe { *(self as *const Self as *const u16) };
	}

	pub const fn from_value(value: u16) -> Option<Self>
	{
		if (value as usize) < NUM_FAULTS
		{
			// SAFETY: Fault codes are contiguous, so any value in this range will be valid.
			return unsafe { Some(*(&value as *const u16 as *const Self)) };
		}

		return None;
	}

	pub const fn from_word(word: Word) -> Self
	{
		// This should never fail, as the FaultCode enum completely
		// fills the bit width provided in the word.
		return Self::from_value(word.bits(FAULT_MASK, FAULT_ARG_BITS))
			.expect("Failed to interpret fault type from instruction word");
	}

	pub const fn to_word(&self, args: u16) -> Word
	{
		return Word((self.to_value() << FAULT_ARG_BITS) | (args & FAULT_ARG_MASK));
	}
}

#[cfg(test)]
mod tests
{
	use super::*;

	#[test]
	fn all_valid_opcodes_from_word()
	{
		assert_eq!(OpCode::from_word(Word(0x0000)), OpCode::Nop);
		assert_eq!(OpCode::from_word(Word(0x1000)), OpCode::Add);
		assert_eq!(OpCode::from_word(Word(0x2000)), OpCode::Sub);
		assert_eq!(OpCode::from_word(Word(0x3000)), OpCode::Mul);
		assert_eq!(OpCode::from_word(Word(0x4000)), OpCode::Div);
		assert_eq!(OpCode::from_word(Word(0x5000)), OpCode::Asgn);
		assert_eq!(OpCode::from_word(Word(0x6000)), OpCode::Shft);
		assert_eq!(OpCode::from_word(Word(0x7000)), OpCode::Bitw);
		assert_eq!(OpCode::from_word(Word(0x8000)), OpCode::Cbx);
		assert_eq!(OpCode::from_word(Word(0x9000)), OpCode::Ldst);
		assert_eq!(OpCode::from_word(Word(0xA000)), OpCode::Stk);
		assert_eq!(OpCode::from_word(Word(0xB000)), OpCode::Sig);
		assert_eq!(OpCode::from_word(Word(0xC000)), OpCode::Unassigned0);
		assert_eq!(OpCode::from_word(Word(0xD000)), OpCode::Unassigned1);
		assert_eq!(OpCode::from_word(Word(0xE000)), OpCode::Unassigned2);
		assert_eq!(OpCode::from_word(Word(0xF000)), OpCode::Unassigned3);

		// Other bits should not affect the opcode
		assert_eq!(OpCode::from_word(Word(0x0BAD)), OpCode::Nop);
	}

	#[test]
	fn all_valid_fault_codes_from_word()
	{
		assert_eq!(FaultCode::from_word(Word(0x0000)), FaultCode::None);
		assert_eq!(FaultCode::from_word(Word(0x1000)), FaultCode::Res);
		assert_eq!(FaultCode::from_word(Word(0x2000)), FaultCode::Algn);
		assert_eq!(FaultCode::from_word(Word(0x3000)), FaultCode::Seg);
		assert_eq!(FaultCode::from_word(Word(0x4000)), FaultCode::Ini);
		assert_eq!(FaultCode::from_word(Word(0x5000)), FaultCode::Sof);
		assert_eq!(FaultCode::from_word(Word(0x6000)), FaultCode::Div);
		assert_eq!(FaultCode::from_word(Word(0x7000)), FaultCode::Ins);
		assert_eq!(FaultCode::from_word(Word(0x8000)), FaultCode::Spv);
		assert_eq!(FaultCode::from_word(Word(0x9000)), FaultCode::Unassigned0);
		assert_eq!(FaultCode::from_word(Word(0xA000)), FaultCode::Unassigned1);
		assert_eq!(FaultCode::from_word(Word(0xB000)), FaultCode::Unassigned2);
		assert_eq!(FaultCode::from_word(Word(0xC000)), FaultCode::Unassigned3);
		assert_eq!(FaultCode::from_word(Word(0xD000)), FaultCode::Unassigned4);
		assert_eq!(FaultCode::from_word(Word(0xE000)), FaultCode::Unassigned5);
		assert_eq!(FaultCode::from_word(Word(0xF000)), FaultCode::Unassigned6);

		// Other bits should not affect the fault code
		assert_eq!(FaultCode::from_word(Word(0x0BAD)), FaultCode::None);
	}
}
