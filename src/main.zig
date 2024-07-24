// Taken from https://github.com/capy-ui/capy/blob/master/examples/notepad.zig

const std = @import("std");
const capy = @import("capy");
const v2mp = @import("v2mp");

pub usingnamespace capy.cross_platform;
const TextAtom = capy.Atom([]const u8);

const Model = struct {
    r0: TextAtom = TextAtom.of(""),
    r1: TextAtom = TextAtom.of(""),
    lr: TextAtom = TextAtom.of(""),
    pc: TextAtom = TextAtom.of(""),
    sp: TextAtom = TextAtom.of(""),
    sr: TextAtom = TextAtom.of(""),
    ir: TextAtom = TextAtom.of(""),
    fault: TextAtom = TextAtom.of(""),
    fault_label: TextAtom = TextAtom.of(""),

    pub fn updateFromWord(field: *TextAtom, value: v2mp.Word) void {
        var text_buffer: [8]u8 = undefined;
        const printed = std.fmt.bufPrint(&text_buffer, "0x{X:0<4}", .{value}) catch unreachable;
        field.set(printed);
    }
};

var cpu: v2mp.Cpu = .{ .fetch_callback = fetchInstruction };
var model: Model = .{};

fn fetchInstruction(address: v2mp.Word) v2mp.InstructionFetchError!v2mp.Word {
    const instructions: [3]v2mp.Word =
        .{
        0x0000,
        0x1001,
        0x2001,
    };

    if (address % v2mp.size_of_word != 0) {
        return v2mp.InstructionFetchError.UnalignedMemoryAccess;
    }

    const index = address / v2mp.size_of_word;

    if (index >= instructions.len) {
        return v2mp.InstructionFetchError.SegmentationFault;
    }

    return instructions[index];
}

fn executeInstruction(_: *anyopaque) !void {
    cpu.fetchDecodeExecute();
    updateModel();
}

fn reset(_: *anyopaque) !void {
    cpu.reset();
    updateModel();
}

fn updateModel() void {
    Model.updateFromWord(&model.r0, cpu.getR0());
    Model.updateFromWord(&model.r1, cpu.getR1());
    Model.updateFromWord(&model.lr, cpu.getLr());
    Model.updateFromWord(&model.pc, cpu.getPc());
    Model.updateFromWord(&model.sp, cpu.getSp());
    Model.updateFromWord(&model.sr, cpu.getSr());
    Model.updateFromWord(&model.ir, cpu.getIr());
    Model.updateFromWord(&model.fault, cpu.getFault());

    model.fault_label.set(v2mp.faultNameFromWord(cpu.getFault()));
}

pub fn main() !void {
    try capy.init();

    var window = try capy.Window.init();

    try window.set(capy.column(.{}, .{
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "IR" }),
                capy.textField(.{ .readOnly = true, .name = "ir_field" }).bind("text", &model.ir),
            },
        ),
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "R0" }),
                capy.textField(.{ .readOnly = true, .name = "r0_field" }).bind("text", &model.r0),
            },
        ),
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "R1" }),
                capy.textField(.{ .readOnly = true, .name = "r1_field" }).bind("text", &model.r1),
            },
        ),
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "LR" }),
                capy.textField(.{ .readOnly = true, .name = "lr_field" }).bind("text", &model.lr),
            },
        ),
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "PC" }),
                capy.textField(.{ .readOnly = true, .name = "pc_field" }).bind("text", &model.pc),
            },
        ),
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "SP" }),
                capy.textField(.{ .readOnly = true, .name = "sp_field" }).bind("text", &model.sp),
            },
        ),
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "SR" }),
                capy.textField(.{ .readOnly = true, .name = "sr_field" }).bind("text", &model.sr),
            },
        ),
        capy.row(
            .{},
            .{
                capy.label(.{ .text = "Fault" }),
                capy.textField(.{ .readOnly = true, .name = "fault_field" }).bind("text", &model.fault),
                capy.label(.{ .name = "fault_label" }).bind("text", &model.fault_label),
            },
        ),
        capy.row(.{}, .{
            capy.button(.{ .label = "Execute", .onclick = executeInstruction }),
            capy.button(.{ .label = "Reset", .onclick = reset }),
        }),
    }));

    updateModel();

    window.setTitle("V2MP");
    window.setPreferredSize(500, 600);
    window.show();

    capy.runEventLoop();
}
