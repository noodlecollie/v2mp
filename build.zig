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
            .root_source_file = this.b.path("src/v2mp/defs.zig"),
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
    // // Standard target options allows the person running `zig build` to choose
    // // what target to build for. Here we do not override the defaults, which
    // // means any target is allowed, and the default is native. Other options
    // // for restricting supported target set are available.
    // const target = b.standardTargetOptions(.{});

    // // Standard optimization options allow the person running `zig build` to select
    // // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall. Here we do not
    // // set a preferred release mode, allowing the user to decide how to optimize.
    // const optimize = b.standardOptimizeOption(.{});

    // // Capy stuff here is based on https://github.com/capy-ui/zig-template/blob/main/build.zig
    // const capy_dep = b.dependency("capy", .{
    //     .target = target,
    //     .optimize = optimize,
    //     .app_name = @as([]const u8, "capy-template"),
    // });

    // const capy = capy_dep.module("capy");

    // // Commenting these out until we need them later for reference.

    // // const lib = b.addStaticLibrary(.{
    // //     .name = "v2mp",
    // //     // In this case the main source file is merely a path, however, in more
    // //     // complicated build scripts, this could be a generated file.
    // //     .root_source_file = b.path("src/root.zig"),
    // //     .target = target,
    // //     .optimize = optimize,
    // // });

    // // // This declares intent for the library to be installed into the standard
    // // // location when the user invokes the "install" step (the default step when
    // // // running `zig build`).
    // // b.installArtifact(lib);

    // const exe = b.addExecutable(.{
    //     .name = "v2mp_debugger",
    //     .root_source_file = b.path("src/main.zig"),
    //     .target = target,
    //     .optimize = optimize,
    // });

    // exe.root_module.addImport("capy", capy);

    // // This declares intent for the executable to be installed into the
    // // standard location when the user invokes the "install" step (the default
    // // step when running `zig build`).
    // b.installArtifact(exe);

    // // This *creates* a Run step in the build graph, to be executed when another
    // // step is evaluated that depends on it. The next line below will establish
    // // such a dependency.
    // const run_cmd = b.addRunArtifact(exe);

    // // By making the run step depend on the install step, it will be run from the
    // // installation directory rather than directly from within the cache directory.
    // // This is not necessary, however, if the application depends on other installed
    // // files, this ensures they will be present and in the expected location.
    // run_cmd.step.dependOn(b.getInstallStep());

    // // This allows the user to pass arguments to the application in the build
    // // command itself, like this: `zig build run -- arg1 arg2 etc`
    // if (b.args) |args| {
    //     run_cmd.addArgs(args);
    // }

    // // This creates a build step. It will be visible in the `zig build --help` menu,
    // // and can be selected like this: `zig build run`
    // // This will evaluate the `run` step rather than the default, which is "install".
    // const run_step = b.step("run", "Run the app");
    // run_step.dependOn(&run_cmd.step);

    // // // Creates a step for unit testing. This only builds the test executable
    // // // but does not run it.
    // // const lib_unit_tests = b.addTest(.{
    // //     .root_source_file = b.path("src/root.zig"),
    // //     .target = target,
    // //     .optimize = optimize,
    // // });

    // // const run_lib_unit_tests = b.addRunArtifact(lib_unit_tests);

    // const exe_unit_tests = b.addTest(.{
    //     .root_source_file = b.path("src/main.zig"),
    //     .target = target,
    //     .optimize = optimize,
    // });

    // const run_exe_unit_tests = b.addRunArtifact(exe_unit_tests);

    // // Similar to creating the run step earlier, this exposes a `test` step to
    // // the `zig build --help` menu, providing a way for the user to request
    // // running the unit tests.
    // const test_step = b.step("test", "Run unit tests");
    // // test_step.dependOn(&run_lib_unit_tests.step);
    // test_step.dependOn(&run_exe_unit_tests.step);

    const builder = Builder.init(b);
    builder.construct();
}
