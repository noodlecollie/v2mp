const std = @import("std");
const build_capy = @import("capy");

const Builder = struct {
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,

    pub fn init(b: *std.Build) Builder {
        return .{
            .b = b,
            .target = b.standardTargetOptions(.{}),
            .optimize = b.standardOptimizeOption(.{}),
        };
    }

    pub fn construct(this: Builder) void {
        this.addLibV2mp();
        this.addV2mpDebugger();
        this.addUnitTestStep();
    }

    fn addLibV2mp(this: Builder) void {
        const v2mplib = this.b.addSharedLibrary(.{
            .name = "v2mp",
            .root_source_file = this.b.path("src/v2mp/v2mp.zig"),
            .target = this.target,
            .optimize = this.optimize,
        });

        this.b.installArtifact(v2mplib);
    }

    fn addV2mpDebugger(this: Builder) void {
        // Capy stuff here is based on https://github.com/capy-ui/zig-template/blob/main/build.zig
        const capy_dep = this.b.dependency("capy", .{
            .target = this.target,
            .optimize = this.optimize,
            .app_name = @as([]const u8, "capy-template"),
        });

        const exe = this.b.addExecutable(.{
            .name = "v2mp_debugger",
            .root_source_file = this.b.path("src/main.zig"),
            .target = this.target,
            .optimize = this.optimize,
        });

        exe.root_module.addImport("capy", capy_dep.module("capy"));

        // This declares intent for the executable to be installed into the
        // standard location when the user invokes the "install" step (the default
        // step when running `zig build`).
        this.b.installArtifact(exe);

        this.addExecutableRunStep(exe);
    }

    fn addExecutableRunStep(this: Builder, exe: *std.Build.Step.Compile) void {
        // This *creates* a Run step in the build graph, to be executed when another
        // step is evaluated that depends on it. The next line below will establish
        // such a dependency.
        const run_cmd = this.b.addRunArtifact(exe);

        // By making the run step depend on the install step, it will be run from the
        // installation directory rather than directly from within the cache directory.
        // This is not necessary, however, if the application depends on other installed
        // files, this ensures they will be present and in the expected location.
        run_cmd.step.dependOn(this.b.getInstallStep());

        // This allows the user to pass arguments to the application in the build
        // command itself, like this: `zig build run -- arg1 arg2 etc`
        if (this.b.args) |args| {
            run_cmd.addArgs(args);
        }

        // This creates a build step. It will be visible in the `zig build --help` menu,
        // and can be selected like this: `zig build run`
        // This will evaluate the `run` step rather than the default, which is "install".
        const run_step = this.b.step("run", "Run the app");
        run_step.dependOn(&run_cmd.step);
    }

    fn addUnitTestStep(this: Builder) void {
        const exe_unit_tests = this.b.addTest(.{
            .root_source_file = this.b.path("src/main.zig"),
            .target = this.target,
            .optimize = this.optimize,
        });

        const run_exe_unit_tests = this.b.addRunArtifact(exe_unit_tests);

        // Similar to creating the run step earlier, this exposes a `test` step to
        // the `zig build --help` menu, providing a way for the user to request
        // running the unit tests.
        const test_step = this.b.step("test", "Run unit tests");
        test_step.dependOn(&run_exe_unit_tests.step);
    }
};

// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    const builder = Builder.init(b);
    builder.construct();
}
