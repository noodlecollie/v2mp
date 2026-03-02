pub type Byte = u8;
pub type Word = u16;

pub const SIZE_OF_WORD: usize = size_of::<Word>();
pub const SIZE_OF_BYTE: usize = size_of::<Byte>();
pub const REGISTER_WIDTH: u8 = 16;

#[derive(Clone, Copy)]
pub struct InstructionWord(pub Word);

impl InstructionWord
{
	#[inline]
	pub const fn value(&self) -> Word
	{
		return self.0;
	}

	#[inline]
	pub const fn byte0(&self) -> Byte
	{
		return (self.0 & 0xFF) as Byte;
	}

	#[inline]
	pub const fn byte1(&self) -> Byte
	{
		return ((self.0 & 0xFF00) >> 4) as Byte;
	}

	#[inline]
	pub const fn bits(&self, mask: Word, mask_offset: u8, return_offset: u8) -> Word
	{
		return (self.0 & (mask << mask_offset)) >> return_offset;
	}

	#[inline]
	pub const fn any_bits_set(&self, mask: Word) -> bool
	{
		return (self.0 & mask) != 0;
	}
}

pub const NUM_OPCODES: usize = 16;
pub const OPCODE_MASK: Word = 0xF000;
pub const INSTRUCTION_ARG_MASK: Word = 0x0FFF;
pub const OPCODE_MASK_BITS: u8 = 4;
pub const INSTRUCTION_ARG_BITS: u8 = REGISTER_WIDTH - OPCODE_MASK_BITS;

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
	pub const fn from_value(value: Word) -> Option<Self>
	{
		if (value as usize) < NUM_OPCODES
		{
			// SAFETY: Op-codes are contiguous, so any value in this range will be valid.
			return unsafe { Some(*(&value as *const Word as *const Self)) };
		}

		return None;
	}

	pub const fn as_value(&self) -> Word
	{
		// SAFETY: Matches the repr type specified for the enum,
		// so we can always convert to this type.
		return unsafe { *(self as *const Self as *const Word) };
	}

	pub const fn from_instruction(word: InstructionWord) -> Self
	{
		// This should never fail, as the OpCode enum completely
		// fills the bit width of the mask.
		return Self::from_value(word.bits(OPCODE_MASK, 0, INSTRUCTION_ARG_BITS))
			.expect("Failed to interpret opcode from instruction word");
	}
}

pub const NUM_FAULTS: usize = 16;
pub const FAULT_MASK: Word = 0xF000;
pub const FAULT_ARG_MASK: Word = 0x0FFF;
pub const FAULT_MASK_BITS: u8 = 4;
pub const FAULT_ARG_BITS: u8 = REGISTER_WIDTH - FAULT_MASK_BITS;

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
	#[inline]
	pub const fn from_value(value: Word) -> Option<Self>
	{
		if (value as usize) < NUM_FAULTS
		{
			// SAFETY: Fault codes are contiguous, so any value in this range will be valid.
			return unsafe { Some(*(&value as *const Word as *const Self)) };
		}

		return None;
	}

	#[inline]
	pub const fn as_value(&self) -> Word
	{
		// SAFETY: Matches the repr type specified for the enum,
		// so we can always convert to this type.
		return unsafe { *(self as *const Self as *const Word) };
	}

	#[inline]
	pub const fn from_instruction(word: InstructionWord) -> Self
	{
		// This should never fail, as the FaultCode enum completely
		// fills the bit width of the mask.
		return Self::from_value(word.bits(FAULT_MASK, 0, FAULT_ARG_BITS))
			.expect("Failed to interpret fault type from instruction word");
	}

	#[inline]
	pub const fn as_register_value(&self, args: Word) -> Word
	{
		return (self.as_value() << FAULT_ARG_BITS) | (args & FAULT_ARG_MASK);
	}
}

pub const NUM_REGISTER_INDICES: usize = 4;
pub const REGISTER_INDEX_MASK: Word = 0x3;
pub const REGISTER_INDEX_MASK_BITS: u8 = 2;

#[repr(u16)]
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum RegisterIndex
{
	R0 = 0x0,
	R1 = 0x1,
	Lr = 0x2,
	Pc = 0x3,
}

impl RegisterIndex
{
	#[inline]
	pub const fn from_value(value: Word) -> Option<Self>
	{
		if (value as usize) < NUM_REGISTER_INDICES
		{
			// SAFETY: Indices are contiguous, so any value in this range will be valid.
			return unsafe { Some(*(&value as *const Word as *const Self)) };
		}

		return None;
	}

	#[inline]
	pub const fn as_value(&self) -> Word
	{
		// SAFETY: Matches the repr type specified for the enum,
		// so we can always convert to this type.
		return unsafe { *(self as *const Self as *const Word) };
	}

	#[inline]
	pub const fn from_instruction(word: InstructionWord, mask_offset: u8) -> Self
	{
		debug_assert!(
			mask_offset <= REGISTER_WIDTH - REGISTER_INDEX_MASK_BITS,
			"Mask offset exceeds width of register"
		);

		// This should never fail, as the RegisterIndex enum completely
		// fills the bit width of the mask.
		return Self::from_value(word.bits(REGISTER_INDEX_MASK, mask_offset, mask_offset))
			.expect("Failed to interpret register index from instruction word");
	}
}

#[repr(u16)]
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum StatusRegisterFlag
{
	Z = 1 << 0,
	C = 1 << 1,
}

impl StatusRegisterFlag
{
	#[inline]
	pub const fn as_value(&self) -> Word
	{
		// SAFETY: Matches the repr type specified for the enum,
		// so we can always convert to this type.
		return unsafe { *(self as *const Self as *const Word) };
	}

	#[inline]
	pub const fn is_set(&self, word: Word) -> bool
	{
		return (self.as_value() & word) != 0;
	}

	#[inline]
	pub const fn set(&self, word: Word) -> Word
	{
		return word | self.as_value();
	}

	#[inline]
	pub const fn clear(&self, word: Word) -> Word
	{
		return word & !self.as_value();
	}

	#[inline]
	pub const fn toggle(&self, word: Word) -> Word
	{
		return word ^ self.as_value();
	}

	#[inline]
	pub const fn set_if(&self, word: Word, should_set: bool) -> Word
	{
		return if should_set { self.set(word) } else { word };
	}

	#[inline]
	pub const fn clear_if(&self, word: Word, should_clear: bool) -> Word
	{
		return if should_clear { self.clear(word) } else { word };
	}

	#[inline]
	pub const fn toggle_if(&self, word: Word, should_toggle: bool) -> Word
	{
		return if should_toggle
		{
			self.toggle(word)
		}
		else
		{
			word
		};
	}
}

#[cfg(test)]
mod tests
{
	use super::*;

	#[test]
	fn all_valid_opcodes_from_instruction()
	{
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x0000)),
			OpCode::Nop
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x1000)),
			OpCode::Add
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x2000)),
			OpCode::Sub
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x3000)),
			OpCode::Mul
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x4000)),
			OpCode::Div
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x5000)),
			OpCode::Asgn
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x6000)),
			OpCode::Shft
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x7000)),
			OpCode::Bitw
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x8000)),
			OpCode::Cbx
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x9000)),
			OpCode::Ldst
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0xA000)),
			OpCode::Stk
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0xB000)),
			OpCode::Sig
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0xC000)),
			OpCode::Unassigned0
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0xD000)),
			OpCode::Unassigned1
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0xE000)),
			OpCode::Unassigned2
		);
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0xF000)),
			OpCode::Unassigned3
		);

		// Other bits should not affect the opcode
		assert_eq!(
			OpCode::from_instruction(InstructionWord(0x0BAD)),
			OpCode::Nop
		);
	}

	#[test]
	fn all_valid_fault_codes_from_instruction()
	{
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x0000)),
			FaultCode::None
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x1000)),
			FaultCode::Res
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x2000)),
			FaultCode::Algn
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x3000)),
			FaultCode::Seg
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x4000)),
			FaultCode::Ini
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x5000)),
			FaultCode::Sof
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x6000)),
			FaultCode::Div
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x7000)),
			FaultCode::Ins
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x8000)),
			FaultCode::Spv
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x9000)),
			FaultCode::Unassigned0
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0xA000)),
			FaultCode::Unassigned1
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0xB000)),
			FaultCode::Unassigned2
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0xC000)),
			FaultCode::Unassigned3
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0xD000)),
			FaultCode::Unassigned4
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0xE000)),
			FaultCode::Unassigned5
		);
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0xF000)),
			FaultCode::Unassigned6
		);

		// Other bits should not affect the fault code
		assert_eq!(
			FaultCode::from_instruction(InstructionWord(0x0BAD)),
			FaultCode::None
		);
	}

	#[test]
	fn all_valid_register_indices()
	{
		for shift in 0..REGISTER_WIDTH - REGISTER_INDEX_MASK_BITS
		{
			let words = (
				0x0000 << shift,
				0x0001 << shift,
				0x0002 << shift,
				0x0003 << shift,
			);

			assert_eq!(
				RegisterIndex::from_instruction(InstructionWord(words.0), shift),
				RegisterIndex::R0,
				"Checking literal value {:#06X}",
				words.0
			);
			assert_eq!(
				RegisterIndex::from_instruction(InstructionWord(words.1), shift),
				RegisterIndex::R1,
				"Checking literal value {:#06X}",
				words.1
			);
			assert_eq!(
				RegisterIndex::from_instruction(InstructionWord(words.2), shift),
				RegisterIndex::Lr,
				"Checking literal value {:#06X}",
				words.2
			);
			assert_eq!(
				RegisterIndex::from_instruction(InstructionWord(words.3), shift),
				RegisterIndex::Pc,
				"Checking literal value {:#06X}",
				words.3
			);
		}
	}
}
