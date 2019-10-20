const std = @import("std");
const time = std.time;
const Timer = time.Timer;

const str_size = 131072;
const tries = 8192;

pub fn main() !void {
    var stdout_file = try std.io.getStdOut();
    const stdout = &stdout_file.outStream().stream;

    var str1: [str_size]u8 = undefined;
    std.mem.set(u8, &str1, 'a');

    var str2: [std.base64.Base64Encoder.calcSize(str1.len)]u8 = undefined;
    std.base64.standard_encoder.encode(&str2, &str1);
    try stdout.print("encode {}... to {}...: ", str1[0..4], str2[0..4]);

    var timer = try Timer.start();
    var s: usize = 0;
    var i: usize = 0;
    var start = timer.lap();
    while (i < tries) : (i += 1) {
        std.base64.standard_encoder.encode(&str2, &str1);
        s += str2.len;
    }
    var end = timer.read();
    var elapsed_s = @intToFloat(f64, end - start) / time.ns_per_s;
    try stdout.print("{}, {d:.2}\n", s, elapsed_s);

    var str3: [str_size]u8 = undefined;
    try std.base64.standard_decoder.decode(&str3, &str2);
    try stdout.print("decode {}... to {}...: ", str2[0..4], str3[0..4]);

    i = 0;
    s = 0;
    start = timer.lap();
    while (i < tries) : (i += 1) {
        try std.base64.standard_decoder.decode(&str3, &str2);
        s += str3.len;
    }
    end = timer.read();
    elapsed_s = @intToFloat(f64, end - start) / time.ns_per_s;
    try stdout.print("{}, {d:.2}\n", s, elapsed_s);
}
