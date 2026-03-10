use crate::arch::*;
use crate::execution_context::{
	AccessError, ExecutionContext, RegisterTransform, Registers, StoreType,
};
use crate::signals::SignalRegisters;

pub mod load_store
{

	pub use super::*;

	const MASK_RESBITS: Word = 0x1FFF;
	const MASK_OPERATION_IS_STORE: Word = 0x8000;
	const REGINDEX_SHIFT: u8 = 13;

	pub fn raise(registers: &Registers) -> SignalRegisters
	{
		let r1_bits: WordBits = WordBits(registers.r1);

		if r1_bits.any_bits_set(MASK_RESBITS)
		{
			return SignalRegisters::reserved(registers.r0);
		}

		return SignalRegisters::new(registers.r0, registers.r1, registers.lr);
	}

	pub fn handle(mut context: ExecutionContext) -> ExecutionContext
	{
		let s1_bits: WordBits = WordBits(context.registers.s1);
		let is_store: bool = s1_bits.any_bits_set(MASK_OPERATION_IS_STORE);
		let register_index: RegisterIndex = RegisterIndex::from_word_bits(s1_bits, REGINDEX_SHIFT);

		if is_store
		{
			let value_to_store: Word = context.registers.get_register_value(register_index);
			let result: Result<(), AccessError> = context.memory_store.write_word(
				StoreType::Data,
				context.registers.lr,
				value_to_store,
			);

			if let Err(err) = result
			{
				return error_to_fault(context, err);
			}

			return ExecutionContext {
				registers: RegisterTransform {
					sr: Some(StatusRegisterFlag::Z.set_if(0, value_to_store == 0)),
					..Default::default()
				}
				.apply(context.registers),
				memory_store: context.memory_store,
			};
		}

		let result: Result<Word, AccessError> = context
			.memory_store
			.read_word(StoreType::Data, context.registers.lr);

		return match result
		{
			Ok(value) => ExecutionContext {
				registers: RegisterTransform {
					sr: Some(StatusRegisterFlag::Z.set_if(0, value == 0)),
					..Default::default()
				}
				.set_register(register_index, value)
				.apply(context.registers),
				memory_store: context.memory_store,
			},
			Err(err) => error_to_fault(context, err),
		};
	}

	fn error_to_fault(context: ExecutionContext, err: AccessError) -> ExecutionContext
	{
		let fault: Word = match err
		{
			AccessError::Misaligned => FaultCode::Algn.as_register_value(0),
			AccessError::OutOfBounds => FaultCode::Seg.as_register_value(0),
		};

		return ExecutionContext {
			registers: RegisterTransform {
				fr: Some(fault),
				..Default::default()
			}
			.apply(context.registers),
			memory_store: context.memory_store,
		};
	}
}

pub mod push_pop_stack
{
	pub use super::*;

	pub fn raise(registers: &Registers) -> SignalRegisters
	{
		todo!();
	}

	pub fn handle(context: ExecutionContext) -> ExecutionContext
	{
		todo!();
	}
}
