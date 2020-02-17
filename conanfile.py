from conans import ConanFile, CMake

class EziEngineConan(ConanFile):
    name            = "eziengine"
    version         = "1.0.0"     
    description     = "Conan package for rttr."    
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "gcc", "txt"
    options = { "shared": [True, False]} 
    default_options = "shared=False"
    exports_sources = "src/*"

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
        cmake.configure(source_folder="src")
        cmake.build()

    def package(self):      
        self.copy("*.h"  , dst="include", src="src")
        self.copy("*.a"  , dst="lib", keep_path=False)
        self.copy("*.so" , dst="lib", keep_path=False)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["eziengine"]