const std = @import("std");
const time = std.time;

const Allocator = std.mem.Allocator;

fn threadHello(num: usize) void {
    std.debug.print("Hello from thread {}!\n", .{num});
    time.sleep(2000 * time.ns_per_ms);
}

const threads_len: usize = 10;

pub fn main() !void{
    var Threads: [threads_len]std.Thread = undefined;
    for (0..Threads.len) |index| {
        Threads[index] = try std.Thread.spawn(.{}, threadHello, .{ index });
    }

    std.debug.print("Hello from main!\n", .{});

    for (0..Threads.len) |index| {
        std.Thread.join(Threads[index]);
        std.debug.print("Thread {} joined!\n", .{ index });
    }
}

