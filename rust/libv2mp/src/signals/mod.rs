mod internal;

use std::collections::BTreeMap;

use lazy_static::lazy_static;

use crate::arch::*;
use crate::execution_context::{ExecutionContext, Registers};

#[derive(Debug)]
pub struct SignalRegisters
{
	pub s0: Word,
	pub s1: Word,
	pub s2: Word,
	pub fr: Word,
}

impl SignalRegisters
{
	pub const fn reserved(signal_word: Word) -> Self
	{
		return Self {
			s0: signal_word,
			s1: 0,
			s2: 0,
			fr: FaultCode::Res.as_register_value(0),
		};
	}

	pub const fn invalid_signal(signal_word: Word) -> Self
	{
		return Self {
			s0: signal_word,
			s1: 0,
			s2: 0,
			fr: FaultCode::Ins.as_value(),
		};
	}

	pub const fn new(s0: Word, s1: Word, s2: Word) -> Self
	{
		return Self {
			s0,
			s1,
			s2,
			fr: FaultCode::None.as_value(),
		};
	}
}

// Parses CPU registers and returns the relevant signal registers.
pub fn raise(registers: &Registers) -> SignalRegisters
{
	return raise_impl(registers, false);
}

pub fn raise_internal(registers: &Registers) -> SignalRegisters
{
	return raise_impl(registers, true);
}

pub fn handle(context: ExecutionContext) -> ExecutionContext
{
	// By this point, we expect that if s0 is set then it maps to a known signal.
	// If it doesn't it means there's a programmer error in raise().
	return (SIGNAL_MAP
		.get(&context.registers.s0)
		.expect("Expected valid signal code in signal register")
		.handle)(context);
}

fn raise_impl(registers: &Registers, internal_allowed: bool) -> SignalRegisters
{
	let signal_word: Word = registers.r0;

	return if let Some(entry) = SIGNAL_MAP.get(&signal_word)
	{
		if (signal_word & SIGNAL_INTERNAL_MASK) != 0 && !internal_allowed
		{
			SignalRegisters::reserved(signal_word)
		}
		else
		{
			(entry.raise)(registers)
		}
	}
	else
	{
		SignalRegisters::invalid_signal(signal_word)
	};
}

struct SignalEntry
{
	pub raise: fn(registers: &Registers) -> SignalRegisters,
	pub handle: fn(context: ExecutionContext) -> ExecutionContext,
}

lazy_static! {
	static ref SIGNAL_MAP: BTreeMap<Word, SignalEntry> = {
		let mut map: BTreeMap<Word, SignalEntry> = BTreeMap::new();

		map.insert(
			SignalCode::InternalLoadStore.as_value(),
			SignalEntry {
				raise: internal::load_store::raise,
				handle: internal::load_store::handle,
			},
		);

		map.insert(
			SignalCode::InternalPushPopStack.as_value(),
			SignalEntry {
				raise: internal::push_pop_stack::raise,
				handle: internal::push_pop_stack::handle,
			},
		);

		map
	};
}
