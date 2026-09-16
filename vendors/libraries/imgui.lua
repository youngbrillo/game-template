library = {
    name = "imgui",
    installLocation = vendorDirPath .. "/imgui",
}

function library:tryLoad()
    print "\t\ttrying to load dearImgui"
end

function library:link()
    links (self.name)
end


function library:include()

    includedirs { 
        self.installLocation .."/",
        self.installLocation .."/imgui",
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
            ["Header Files/*"] = { self.installLocation.. "/**.h", self.installLocation.. "/**.hpp"},
            ["Source Files/*"] = { self.installLocation.. "/**.cpp", self.installLocation.. "/**.c"},
        }
        files {
            self.installLocation.."/imgui_demo.cpp", 
            self.installLocation.."/imgui_draw.cpp", 
            self.installLocation.."/imgui_internal.h", 
            self.installLocation.."/imgui_stdlib.cpp", 
            self.installLocation.."/imgui_stdlib.h", 
            self.installLocation.."/imgui_tables.cpp", 
            self.installLocation.."/imgui_widgets.cpp", 
            self.installLocation.."/imgui.cpp", 
            self.installLocation.."/imgui.h", 
            self.installLocation.."/imstb_rectpack.h", 
            self.installLocation.."/imstb_textedit.h", 
            self.installLocation.."/imstb_truetype.h",
        }

        
        removefiles {
           self.installLocation.. "/backends/**", 
           self.installLocation.. "/examples/**", 
           self.installLocation.. "/misc/**", 
           self.installLocation.. "/imgui_impl_glfw.**"
        }
end


return library;