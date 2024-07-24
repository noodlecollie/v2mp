const std = @import("std");
const build_capy = @import("capy");

const Dependency = std.Build.Dependency;
const CompileStep = std.Build.Step.Compile;

const BuilderData = struct {
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,

    pub fn init(b: *std.Build) BuilderData {
        return .{
            .b = b,
            .target = b.standardTargetOptions(.{}),
            .optimize = b.standardOptimizeOption(.{}),
        };
    }
};

// Capy stuff here is based on https://github.com/capy-ui/zig-template/blob/main/build.zig
fn getCapyDependency(data: BuilderData) *Dependency {
    return data.b.dependency("capy", .{
        .target = data.target,
        .optimize = data.optimize,
        .app_name = @as([]const u8, "capy-template"),
    });
}

fn addLibV2mp(data: BuilderData) *CompileStep {
    const v2mplib = data.b.addSharedLibrary(.{
        .name = "v2mp",
        .root_source_file = data.b.path("src/v2mp/v2mp.zig"),
        .target = data.target,
        .optimize = data.optimize,
    });

    data.b.installArtifact(v2mplib);

    return v2mplib;
}

fn addV2mpDebugger(data: BuilderData, v2mp_lib: *CompileStep) *CompileStep {
    const capy_dep = getCapyDependency(data);

    const exe = data.b.addExecutable(.{
        .name = "v2mp_debugger",
        .root_source_file = data.b.path("src/main.zig"),
        .target = data.target,
        .optimize = data.optimize,
    });

    exe.root_module.addImport("capy", capy_dep.module("capy"));
    exe.root_module.addImport("v2mp", &v2mp_lib.root_module);

    // This declares intent for the executable to be installed into the
    // standard location when the user invokes the "install" step (the default
    // step when running `zig build`).
    data.b.installArtifact(exe);

    return exe;
}

fn addExecutableRunStep(data: BuilderData, exe: *CompileStep) void {
    // This *creates* a Run step in the build graph, to be executed when another
    // step is evaluated that depends on it. The next line below will establish
    // such a dependency.
    const run_cmd = data.b.addRunArtifact(exe);

    // By making the run step depend on the install step, it will be run from the
    // installation directory rather than directly from within the cache directory.
    // This is not necessary, however, if the application depends on other installed
    // files, this ensures they will be present and in the expected location.
    run_cmd.step.dependOn(data.b.getInstallStep());

    // This allows the user to pass arguments to the application in the build
    // command itself, like this: `zig build run -- arg1 arg2 etc`
    if (data.b.args) |args| {
        run_cmd.addArgs(args);
    }

    // This creates a build step. It will be visible in the `zig build --help` menu,
    // and can be selected like this: `zig build run`
    // This will evaluate the `run` step rather than the default, which is "install".
    const run_step = data.b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}

fn addUnitTestStep(data: BuilderData) void {
    const exe_unit_tests = data.b.addTest(.{
        .root_source_file = data.b.path("src/v2mp/tests.zig"),
        .target = data.target,
        .optimize = data.optimize,
    });

    const run_exe_unit_tests = data.b.addRunArtifact(exe_unit_tests);

    // Similar to creating the run step earlier, this exposes a `test` step to
    // the `zig build --help` menu, providing a way for the user to request
    // running the unit tests.
    const test_step = data.b.step("test", "Run unit tests");
    test_step.dependOn(&run_exe_unit_tests.step);
}

// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    const builder_data = BuilderData.init(b);
    const v2mp_lib = addLibV2mp(builder_data);
    const debugger_exe = addV2mpDebugger(builder_data, v2mp_lib);
    addExecutableRunStep(builder_data, debugger_exe);
    addUnitTestStep(builder_data);
}
