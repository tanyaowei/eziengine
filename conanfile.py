from conans import ConanFile, CMake

class EziEngineConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "gcc", "txt"
    options = { "shared": [True, False]} 
    default_options = "shared=False"

    def requirements(self):
        self.requires("glm/0.9.9.5@_/_")
        self.requires("arduinojson/5.13.0@conan/stable")
        self.requires("rttr/master@myremote/stable")

    def configure(self):
        self.options["rttr"].shared = self.options.shared 

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
