library = {
    name = "lua",
    installLocation = vendorDirPath .. "/lua",
}

function library:tryLoad()
    print ("\t\ttrying to load ".. self.name .. " lib")
    print ("\t\t\t install path:  ".. self.installLocation)
end

function library:link()
    links (self.name)
end


function library:include()

    includedirs { 
        self.installLocation .."/",
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

        includedirs { self.installLocation.. "/" }

        vpaths 
        {
            ["Header Files/*"] = { self.installLocation.. "./**.h", self.installLocation.. "./**.hpp"},
            ["Source Files/*"] = { self.installLocation.. "./**.cpp", self.installLocation.. "./**.c"},
        }
        files {
            self.installLocation.. "./**.h",
            self.installLocation.. "./**.c"
        }

        removefiles {
            self.installLocation.. "/testes/**", 
            self.installLocation.. "/manual/**", 
            self.installLocation.. "./lua.c", 
            self.installLocation.. "./onelua.c",
        }


end


return library;