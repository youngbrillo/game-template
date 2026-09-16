library = {
    name = "yaml-cpp",
    installLocation = vendorDirPath .. "/yaml-cpp",
}

function library:tryLoad()
    print "\t\ttrying to load the yaml-cpp lib"
end

function library:link()
    links (self.name)
end


function library:include()

    includedirs { 
        self.installLocation .."/include",
        self.installLocation .."/src",
    }
    self:platform_defines()

end

function library:platform_defines()
    defines {
        "YAML_CPP_STATIC_DEFINE" , 
        "DYAML_BUILD_SHARED_LIBS=Off",
        "YAML_CPP_DLL",
    } 
end
function library:build()
    project (self.name)
        kind "StaticLib"

        self:platform_defines()

        language "C"
        location (workspaceBuildPath)
        targetdir ( workspaceBuildPath.."/bin/%{cfg.buildcfg}")


        vpaths 
        {
            ["Header Files/*"] = { self.installLocation.. "./include/**.h", self.installLocation.. "./include/**.hpp"},
            ["Source Files/*"] = { self.installLocation.. "./src/**.cpp", self.installLocation.. "./src/**.c"},
        }
        files {
            self.installLocation.. "./include/**.hpp",
            self.installLocation.. "./include/**.h",
            self.installLocation.. "./src/**.hpp", 
            self.installLocation.. "./src/**.h",
            self.installLocation.. "./src/**.cpp",
            self.installLocation.. "./src/**.c"
        }

        includedirs { self.installLocation.. "/src" }
        includedirs { self.installLocation.. "/include"}

end


return library;