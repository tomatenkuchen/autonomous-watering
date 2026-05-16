from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Git
from pathlib import Path


class autonomous_wateringRecipe(ConanFile):
    name = "autonomouswatering"
    package_type = "application"

    # Optional metadata
    license = "GNU GPLv2"
    author = "tomatenkuchen"
    url = "https://github.com/tomatenkuchen/autonomous-watering"
    description = "firmware for autonomous watering system"
    topics = ("embedded", "watering", "esp32c6")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "main/*", "cmake/*", "sdkconfig"

    def set_version(self):
        git = Git(self)
        version = git.run("describe").split("-")
        if len(version) == 1:
            self.version = f"{version[0]}.0"
        else:
            self.version = f"{version[0]}.{version[1]}"

    def requirements(self):
        self.tool_requires("cmake/4.2.3")
        self.tool_requires("ninja/1.13.2")
        self.tool_requires("cppcheck/2.19.1")

        self.requires("etl/20.47.1")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)

        # feed version into to cmake
        version_file = Path(self.build_folder) / "version.cmake"
        version_str = str(self.version).split(".")
        version_file.write_text(
            f"set(CONAN_PROJECT_VERSION_MAJOR {version_str[0]})\n"
            f"set(CONAN_PROJECT_VERSION_MINOR {version_str[1]})\n"
            f"set(CONAN_PROJECT_VERSION_PATCH {version_str[2]})\n"
            f"set(CONAN_PROJECT_VERSION_TWEAK {version_str[3]})\n"
            f"set(CONAN_PROJECT_NAME {self.name})\n"
            f"set(CONAN_PROJECT_DESCRIPTION {self.description})\n"
        )

        tc.generate()

    def build(self):
        build_dir = Path(self.build_folder)
        source_dir = Path(self.source_folder)
        self.run(f"idf.py -C {source_dir} -B {build_dir} build")

        # copy compile commands from build type dir to build dir
        cc_path = build_dir / "compile_commands.json"
        cc_path.copy(build_dir / ".." / "compile_commands.json")
        

    def package(self):
        """instead of packaging anything, we can use this to flash"""
        build_dir = Path(self.build_folder)
        self.run(
            "python3 -m esptool --chip esp32c6 -b 460800 --before default-reset --after hard-reset write-flash @flash_args",
            cwd=build_dir,
        )
