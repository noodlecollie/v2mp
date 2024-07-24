// Taken from https://github.com/capy-ui/capy/blob/master/examples/notepad.zig

const std = @import("std");
const capy = @import("capy");
const v2mp = @import("v2mp");

pub usingnamespace capy.cross_platform;

var cpu: v2mp.Cpu = .{ .fetch_callback = fetchInstruction };
var instruction_text = capy.Atom([]const u8).of("unset");

fn fetchInstruction(_: v2mp.Word) v2mp.InstructionFetchError!v2mp.Word {
    const Static = struct {
        var index: usize = 0;
        const instructions: [3]v2mp.Word =
            .{
            0x0000,
            0x1001,
            0x2001,
        };
    };

    const out = Static.instructions[Static.index];
    Static.index = (Static.index + 1) % Static.instructions.len;
    return out;
}

fn executeInstruction(_: *anyopaque) !void {
    cpu.fetchDecodeExecute();
}

pub fn main() !void {
    try capy.init();

    var window = try capy.Window.init();

    try window.set(capy.column(.{}, .{
        capy.row(
            .{ .expand = .Fill },
            .{
                capy.label(.{ .text = "R0" }),
                capy.textField(.{ .readOnly = true, .name = "r0_field" }),
            },
        ),
        capy.row(
            .{ .expand = .Fill },
            .{
                capy.label(.{ .text = "R1" }),
                capy.textField(.{ .readOnly = true, .name = "r1_field" }),
            },
        ),
        capy.row(
            .{ .expand = .Fill },
            .{
                capy.label(.{ .text = "LR" }),
                capy.textField(.{ .readOnly = true, .name = "lr_field" }),
            },
        ),
        capy.row(
            .{ .expand = .Fill },
            .{
                capy.label(.{ .text = "PC" }),
                capy.textField(.{ .readOnly = true, .name = "pc_field" }),
            },
        ),
        capy.row(
            .{ .expand = .Fill },
            .{
                capy.label(.{ .text = "SR" }),
                capy.textField(.{ .readOnly = true, .name = "sr_field" }),
            },
        ),
        capy.row(
            .{ .expand = .Fill },
            .{
                capy.label(.{ .text = "Instruction" }),
                capy.textField(.{ .name = "instruction_field" }).bind("text", &instruction_text),
            },
        ),
        capy.button(.{ .label = "Execute", .onclick = executeInstruction }),
    }));

    var instruction_text_buffer: [8]u8 = undefined;
    const printed = std.fmt.bufPrint(&instruction_text_buffer, "0x{X:0<4}", .{cpu._ir}) catch unreachable;
    instruction_text.set(printed);

    window.setTitle("V2MP");
    window.setPreferredSize(800, 600);
    window.show();

    capy.runEventLoop();
}
