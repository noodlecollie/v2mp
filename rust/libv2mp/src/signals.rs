use std::collections::BTreeMap;

use lazy_static::lazy_static;

use crate::arch::*;
use crate::execution_context::Registers;

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
	pub const fn reserved(signal: SignalCode) -> Self
	{
		return Self {
			s0: signal.as_value(),
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
}

// Parses CPU registers and returns the relevant signal registers.
pub fn raise(registers: &Registers) -> SignalRegisters
{
	return if let Some(entry) = SIGNAL_MAP.get(&registers.r0)
	{
		(entry.raise)(registers)
	}
	else
	{
		SignalRegisters::invalid_signal(registers.r0)
	};
}

pub fn handle(registers: Registers) -> Registers
{
	// By this point, we expect that if s0 is set then it maps to a known signal.
	// If it doesn't it means there's a programmer error in raise().
	return (SIGNAL_MAP
		.get(&registers.s0)
		.expect("Expected valid signal code in signal register")
		.handle)(registers);
}

struct SignalEntry
{
	pub raise: fn(registers: &Registers) -> SignalRegisters,
	pub handle: fn(registers: Registers) -> Registers,
}

// TODO: This probably needs to be set up at the mainboard level in the module
// hierarchy, not at the CPU level.
lazy_static! {
	static ref SIGNAL_MAP: BTreeMap<Word, SignalEntry> = {
		let mut map: BTreeMap<Word, SignalEntry> = BTreeMap::new();

		map.insert(
			SignalCode::InternalLoadStore.as_value(),
			SignalEntry {
				raise: |_| SignalRegisters::reserved(SignalCode::InternalLoadStore),
				handle: |registers: Registers| {
					// TODO: Load or store based on the register values set
					todo!();
				},
			},
		);

		map.insert(
			SignalCode::InternalPushPopStack.as_value(),
			SignalEntry {
				raise: |_| SignalRegisters::reserved(SignalCode::InternalPushPopStack),
				handle: |registers: Registers| {
					// TODO: Stack operation
					todo!();
				},
			},
		);

		map
	};
}
