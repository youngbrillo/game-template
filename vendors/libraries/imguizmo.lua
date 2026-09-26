library = {
    name = "ImGuizmo",
    installLocation = vendorDirPath .. "/ImGuizmo",
}

function library:tryLoad()
    print "\t\ttrying to load ImGuizmo lib"
end

function library:link()
    links (self.name)
end


function library:include()

    includedirs { 
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
            ["Header Files/*"] = { self.installLocation.. "./src/**.h", self.installLocation.. "./src/**.hpp"},
            ["Source Files/*"] = { self.installLocation.. "./src/**.cpp", self.installLocation.. "./src/**.c"},
        }
        files {
            self.installLocation.. "./src/**.hpp", 
            self.installLocation.. "./src/**.h",
            self.installLocation.. "./src/**.cpp",
            self.installLocation.. "./src/**.c"
        }

        includedirs { self.installLocation.. "/src" }
        IncludeVendor("imgui")

end


return library;