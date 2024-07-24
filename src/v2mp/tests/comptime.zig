const std = @import("std");
const v2mp = @import("../v2mp.zig");

test "Expect type widths are correct" {
    comptime std.testing.expect(@sizeOf(v2mp.Word) == 2);
    comptime std.testing.expect(@sizeOf(v2mp.Byte) == 1);
}
