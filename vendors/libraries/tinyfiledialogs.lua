library = {
    name = "tinyfiledialogs",
    installLocation = vendorDirPath .. "/tinyfiledialogs",
}

function library:tryLoad()
    print "\t\ttrying to load tinyfiledialogs lib"
end

function library:link()
    links (self.name)
end


function library:include()

    includedirs { 
        self.installLocation .."./",
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
            ["Header Files/*"] = { self.installLocation.. "./**.h", self.installLocation.. "./**.hpp"},
            ["Source Files/*"] = { self.installLocation.. "./**.cpp", self.installLocation.. "./**.c"},
        }
        files {
            self.installLocation.. "./**.hpp", 
            self.installLocation.. "./**.h",
            self.installLocation.. "./**.cpp",
            self.installLocation.. "./**.c"
        }

        includedirs { self.installLocation.. "./" }
end


return library;