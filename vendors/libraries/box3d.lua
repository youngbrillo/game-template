library = {
    name = "box3D",
    installLocation = vendorDirPath .. "/box3D",
}

function library:tryLoad()
    print "\t\ttrying to load box3D lib"
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