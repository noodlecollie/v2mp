use std::alloc::{Layout, alloc, alloc_zeroed, dealloc};
use std::ptr::null_mut;

use crate::arch::*;
use crate::signals::SignalRegisters;

pub enum StoreType
{
	Code,
	Data,
	Stack,
}

pub enum AccessError
{
	Misaligned,
	OutOfBounds,
}

pub struct ExecutionContext
{
	pub registers: Registers,
	pub memory_store: MemoryStore,
}

impl ExecutionContext
{
	pub fn new(cs_words: usize, ds_words: usize, ss_words: usize) -> Self
	{
		return Self {
			registers: Registers::default(),
			memory_store: MemoryStore::new(cs_words, ds_words, ss_words),
		};
	}
}

#[derive(Clone, Debug, PartialEq)]
pub struct Registers
{
	pub pc: Word,
	pub sr: Word,
	pub lr: Word,
	pub r0: Word,
	pub r1: Word,
	pub ir: Word,
	pub sp: Word,
	pub fr: Word,
	pub s0: Word,
	pub s1: Word,
	pub s2: Word,
}

impl Registers
{
	pub fn get_register_value(&self, index: RegisterIndex) -> Word
	{
		return match index
		{
			RegisterIndex::R0 => self.r0,
			RegisterIndex::R1 => self.r1,
			RegisterIndex::Lr => self.lr,
			RegisterIndex::Pc => self.pc,
		};
	}
}

impl Default for Registers
{
	fn default() -> Self
	{
		return Self {
			pc: 0,
			sr: 0,
			lr: 0,
			r0: 0,
			r1: 0,
			ir: 0,
			sp: 0,
			fr: 0,
			s0: 0,
			s1: 0,
			s2: 0,
		};
	}
}

#[derive(Debug)]
pub struct RegisterTransform
{
	pub pc: Option<Word>,
	pub sr: Option<Word>,
	pub lr: Option<Word>,
	pub r0: Option<Word>,
	pub r1: Option<Word>,
	pub ir: Option<Word>,
	pub sp: Option<Word>,
	pub fr: Option<Word>,
	pub s0: Option<Word>,
	pub s1: Option<Word>,
	pub s2: Option<Word>,
}

impl RegisterTransform
{
	pub fn set_register(mut self, index: RegisterIndex, value: Word) -> Self
	{
		match index
		{
			RegisterIndex::R0 => self.r0 = Some(value),
			RegisterIndex::R1 => self.r1 = Some(value),
			RegisterIndex::Lr => self.lr = Some(value),
			RegisterIndex::Pc => self.pc = Some(value),
		};

		return self;
	}

	pub fn set_signal_registers(mut self, registers: SignalRegisters) -> Self
	{
		self.s0 = Some(registers.s0);
		self.s1 = Some(registers.s1);
		self.s2 = Some(registers.s2);
		self.fr = Some(registers.fr);

		return self;
	}

	pub fn apply(self, mut registers: Registers) -> Registers
	{
		const REG_VAL_FAULT_NONE_WITH_INVALID_ARGS: Word =
			FaultCode::Div.as_register_value(FAULT_ARGS_RESERVED_BITS_SET);

		registers = Registers {
			pc: self.pc.unwrap_or(registers.pc),
			sr: self.sr.unwrap_or(registers.sr),
			lr: self.lr.unwrap_or(registers.lr),
			r0: self.r0.unwrap_or(registers.r0),
			r1: self.r1.unwrap_or(registers.r1),
			ir: self.ir.unwrap_or(registers.ir),
			sp: self.sp.unwrap_or(registers.sp),
			fr: self.fr.unwrap_or(registers.fr),
			s0: self.s0.unwrap_or(SignalCode::None.as_value()),
			s1: self.s1.unwrap_or(0),
			s2: self.s2.unwrap_or(0),
		};

		// If there is no fault code set, but there are fault bits set,
		// map this to a Res fault.
		let fault_bits: WordBits = WordBits(registers.fr);

		if FaultCode::from_word_bits(fault_bits) == FaultCode::None
			&& fault_bits.any_bits_set(FAULT_ARG_MASK)
		{
			registers.fr = REG_VAL_FAULT_NONE_WITH_INVALID_ARGS;
		}

		return registers;
	}
}

pub struct MemoryStore
{
	cs: MemoryBuffer,
	ds: MemoryBuffer,
	ss: MemoryBuffer,
}

impl MemoryStore
{
	pub fn new(cs_words: usize, ds_words: usize, ss_words: usize) -> Self
	{
		return Self {
			cs: MemoryBuffer::new_zeroed(cs_words),
			ds: MemoryBuffer::new_zeroed(ds_words),
			ss: MemoryBuffer::new_zeroed(ss_words),
		};
	}

	pub fn read_word(&self, store: StoreType, address: Word) -> Result<Word, AccessError>
	{
		return self.get_store(store).read_word(address as usize);
	}

	pub fn write_word(
		&mut self,
		store: StoreType,
		address: Word,
		value: Word,
	) -> Result<(), AccessError>
	{
		let store: &mut MemoryBuffer = match store
		{
			StoreType::Code => &mut self.cs,
			StoreType::Data => &mut self.ds,
			StoreType::Stack => &mut self.ss,
		};

		return store.write_word(address as usize, value);
	}

	pub fn store_size_in_words(&self, store: StoreType) -> Word
	{
		return self.get_store(store).size_in_words() as Word;
	}

	fn get_store(&self, store: StoreType) -> &MemoryBuffer
	{
		return match store
		{
			StoreType::Code => &self.cs,
			StoreType::Data => &self.ds,
			StoreType::Stack => &self.ss,
		};
	}
}

pub struct MemoryBuffer
{
	layout: Layout,
	mem: *mut u8,
}

impl MemoryBuffer
{
	pub fn new(size_in_words: usize) -> Self
	{
		return MemoryBuffer::create(size_in_words, false);
	}

	pub fn new_zeroed(size_in_words: usize) -> Self
	{
		return MemoryBuffer::create(size_in_words, true);
	}

	pub fn read_word(&self, address: usize) -> Result<Word, AccessError>
	{
		if address % 2 != 0
		{
			return Err(AccessError::Misaligned);
		}

		if address >= self.layout.size()
		{
			return Err(AccessError::OutOfBounds);
		}

		// SAFETY: The address used here is validated above not to be out of bounds of
		// the allocation.
		let accessor: *const Word = unsafe { self.mem.byte_add(address) } as *const Word;

		// SAFETY: The pointer is valid (as above), and the borrowing checks on self
		// ensure that no-one else is mutably accessing this data.
		return Ok(unsafe { *accessor });
	}

	pub fn write_word(&mut self, address: usize, value: Word) -> Result<(), AccessError>
	{
		if address % 2 != 0
		{
			return Err(AccessError::Misaligned);
		}

		if address >= self.layout.size()
		{
			return Err(AccessError::OutOfBounds);
		}

		// SAFETY: The address used here is validated above not to be out of bounds of
		// the allocation.
		let accessor: *mut Word = unsafe { self.mem.byte_add(address) } as *mut Word;

		// SAFETY: The pointer is valid (as above), and the borrowing checks on self
		// ensure that no-one else is mutably accessing this data.
		unsafe { *accessor = value };

		return Ok(());
	}

	pub fn size_in_bytes(&self) -> usize
	{
		return self.layout.size();
	}

	pub fn size_in_words(&self) -> usize
	{
		return self.layout.size() / SIZE_OF_WORD;
	}

	fn create(size_in_words: usize, zero: bool) -> Self
	{
		let layout: Layout = Layout::from_size_align(size_in_words * SIZE_OF_WORD, SIZE_OF_WORD)
			.expect("Failed to create allocation layout");

		assert!(layout.size() == size_in_words * SIZE_OF_WORD);

		if layout.size() < 1
		{
			return Self {
				layout,
				mem: null_mut::<u8>(),
			};
		}

		// SAFETY: Layout size is non-zero.
		let mem = unsafe {
			if zero
			{
				alloc_zeroed(layout)
			}
			else
			{
				alloc(layout)
			}
		};

		if mem.is_null()
		{
			// TODO: Better error handling
			panic!("Could not allocate {size_in_words} words for memory buffer");
		}

		return Self { layout, mem };
	}
}

impl Drop for MemoryBuffer
{
	fn drop(&mut self)
	{
		if !self.mem.is_null()
		{
			// SAFETY: Pointer is not null, and layout is the same as was used to allocate.
			unsafe { dealloc(self.mem, self.layout) };
		}
	}
}
