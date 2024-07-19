const std = @import("std");
const v2mp = @import("v2mp");

pub const Cpu = struct {
    const _InstructionResult = struct {
        pc: ?v2mp.Word = null,
        sr: ?v2mp.Word = null,
        lr: ?v2mp.Word = null,
        r0: ?v2mp.Word = null,
        r1: ?v2mp.Word = null,
        sp: ?v2mp.Word = null,
        fault: ?v2mp.Word = null,
    };

    // We probably want this to be able to return errors too,
    // in case there's an issue obtaining the instruction.
    const FetchInstructionFn = *const fn (address: v2mp.Word) v2mp.Word;

    const _ExecInstructionFn = *const fn (this: *Cpu) _InstructionResult;

    fetch_callback: FetchInstructionFn,

    _pc: v2mp.Word = 0,
    _sr: v2mp.Word = 0,
    _lr: v2mp.Word = 0,
    _r0: v2mp.Word = 0,
    _r1: v2mp.Word = 0,
    _ir: v2mp.Word = 0,
    _sp: v2mp.Word = 0,
    _fault: v2mp.Word = v2mp.Fault.none,

    _exec_callbacks: [16]_ExecInstructionFn = .{
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
        &_executeUnassigned,
    },

    fn fetchDecodeExecute(this: *Cpu) void {
        this._ir = this.fetch_callback(this._pc);

        const instruction_op = v2mp.instructionOpCodeFromWord(this._ir);
        std.debug.assert(instruction_op <= this._exec_callbacks.len);

        const result = this._exec_callbacks[instruction_op](this);
        this._acceptResult(result);
    }

    fn _acceptResult(this: *Cpu, result: _InstructionResult) void {
        this._pc = result.pc orelse this._pc + v2mp.size_of_word;
        this._sr = result.sr orelse this._sr;
        this._lr = result.lr orelse this._lr;
        this._r0 = result.r0 orelse this._r0;
        this._r1 = result.r1 orelse this._r1;
        this._sp = result.sp orelse this._sp;
        this._fault = result.fault orelse v2mp.Fault.none;
    }

    fn _executeUnassigned(_: *Cpu) _InstructionResult {
        return .{ .fault = v2mp.Fault.ini };
    }
};
