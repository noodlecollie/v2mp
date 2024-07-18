// Taken from https://github.com/capy-ui/capy/blob/master/examples/notepad.zig

const std = @import("std");
const capy = @import("capy");

pub usingnamespace capy.cross_platform;

fn executeInstruction(_: *anyopaque) !void {
    std.debug.print("Execute button pressed\n", .{});
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
                capy.textField(.{ .name = "instruction_field" }),
            },
        ),
        capy.button(.{ .label = "Execute", .onclick = executeInstruction }),
    }));

    window.setTitle("V2MP");
    window.setPreferredSize(800, 600);
    window.show();

    capy.runEventLoop();
}
