library = {
    name = "raylib",
    downloadURL = "https://github.com/raysan5/raylib/archive/refs/tags/6.0.zip",
    installLocation = vendorDirPath .. "/raylib",
}

function library:tryLoad()

end

function library:link()
    links ("raylib")

    includedirs { 
          self.installLocation .."/src" 
        , self.installLocation .."/src/external" 
        , self.installLocation .."/src/external/glfw/include" 
    }

    self:platform_defines();
    
    filter "files:**.dll"
        buildaction "Copy"

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        dependson {"raylib"}
        links {"raylib.lib"}
        characterset ("MBCS")
        buildoptions { "/Zc:__cplusplus" }

    filter "system:windows"
        defines{"_WIN32"}
        links {"winmm", "gdi32", "opengl32"}
        libdirs {workspaceBuildPath .. "/bin/%{cfg.buildcfg}"}

    filter "system:linux"
        links {"pthread", "m", "dl", "rt", "X11"}

    filter "system:macosx"
        links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

    filter {"options:backend=SDL2"}
        includedirs {"../SDL2/include" }

    filter { "system:windows", "options:backend=SDL2", "platforms:x64"}
        libdirs {"../SDL2/lib/x64"}
        links {"SDL2"}
        files "../SDL2/lib/x64/SDL2.dll"

    filter { "system:windows", "options:backend=SDL2", "platforms:x32"}
        libdirs {"../SDL2/lib/x32"}
        links {"SDL2"}
        files "../SDL2/lib/x32/SDL2.dll"

    filter { "system:windows", "options:backend=SDL3", "platforms:x64", "action:vs*"}
        includedirs {"../SDL3/include/SDL3" }
        includedirs {"../SDL3/include" }
        libdirs {"../SDL3/lib/x64"}
        links {"SDL3"}
        files "../SDL3/lib/x64/SDL3.dll"
        
    filter { "system:windows", "options:backend=SDL3", "platforms:x32", "action:vs*"}
        includedirs {"../SDL3/include/SDL3" }
        includedirs {"../SDL3/include" }
        libdirs {"../SDL3/lib/x32"}
        links {"SDL3"}
        files "../SDL3/lib/x32/SDL3.dll"
 
    filter { "system:windows", "options:backend=SDL3", "platforms:x64", "action:gmake*"}
        includedirs {"../SDL3/x86_64-w64-mingw32/include/SDL3" }
        includedirs {"../SDL3/x86_64-w64-mingw32/include" }
        libdirs {"../SDL3/x86_64-w64-mingw32/lib/"}
        libdirs {"../SDL3/x86_64-w64-mingw32/bin/"}
        links {"SDL3"}
        files "../SDL3/x86_64-w64-mingw32/bin/SDL3.dll"

    filter { "system:*nix OR system:macosx", "options:backend=SDL2",  "configurations:Debug OR configurations:Release"}
        links {"SDL2"}

    filter { "system:*nix OR system:macosx", "options:backend=SDL3",  "configurations:Debug OR configurations:Release"}
        links {"SDL3"}

    filter{}

end


function library:include()

    includedirs { 
          self.installLocation .."/src" 
        , self.installLocation .."/src/external" 
        , self.installLocation .."/src/external/glfw/include" 
    }

    self:platform_defines()
    
    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}

    filter{}
end

function library:platform_defines()
     filter {"options:backend=GLFW"}
        defines{"PLATFORM_DESKTOP"}

    filter {"options:backend=RLFW"}
        defines{"PLATFORM_DESKTOP_RGFW"}

    filter {"options:backend=SDL2"}
        defines{"PLATFORM_DESKTOP_SDL"}

    filter {"options:backend=SDL3"}
        defines{"PLATFORM_DESKTOP_SDL"}

    filter {"options:graphics=opengl43"}
        defines{"GRAPHICS_API_OPENGL_43"}

    filter {"options:graphics=opengl33"}
        defines{"GRAPHICS_API_OPENGL_33"}

    filter {"options:graphics=opengl21"}
        defines{"GRAPHICS_API_OPENGL_21"}

    filter {"options:graphics=opengl11"}
        defines{"GRAPHICS_API_OPENGL_11"}

    filter {"options:graphics=openges3"}
        defines{"GRAPHICS_API_OPENGL_ES3"}

    filter {"options:graphics=openges2"}
        defines{"GRAPHICS_API_OPENGL_ES2"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux"}
        defines {"_GLFW_X11"}
        defines {"_GNU_SOURCE"}
    -- This is necessary, otherwise compilation will fail since
    -- there is no CLOCK_MONOTOMIC. raylib claims to have a workaround
    -- to compile under c99 without -D_GNU_SOURCE, but it didn't seem
    -- to work. raylib's Makefile also adds this flag, probably why it went
    -- unnoticed for so long.
    -- It compiles under c11 without -D_GNU_SOURCE, because c11 requires
    -- to have CLOCK_MONOTOMIC
    -- See: https://github.com/raysan5/raylib/issues/2729
    filter{}
        -- defines {"SUPPORT_GPU_SKINNING=1"}
        defines {"SUPPORT_FILEFORMAT_JPG=1"}
end
function library:build()
    project "raylib"
        kind "StaticLib"

        self:platform_defines()

        language "C"
        location (workspaceBuildPath)
        targetdir ( workspaceBuildPath.."/bin/%{cfg.buildcfg}")

        filter {"options:backend=SDL2"}
            includedirs {"SDL2/include" }

        filter {"options:backend=SDL3", "action:vs*"}
            includedirs {"SDL3/include/SDL3" }
            includedirs {"SDL3/include" }
            
        filter { "system:windows", "options:backend=SDL3", "platforms:x64", "action:gmake*"}
            includedirs {"SDL3/x86_64-w64-mingw32/include/SDL3" }
            includedirs {"SDL3/x86_64-w64-mingw32/include" }

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("MBCS")
            buildoptions { "/Zc:__cplusplus" }
        filter{}

        includedirs {self.installLocation .. "/src", self.installLocation .. "/src/external/glfw/include" }
        vpaths
        {
            ["Header Files"] = { self.installLocation .. "/src/**.h"},
            ["Source Files/*"] = { self.installLocation .. "/src/**.c"},
        }
        files {self.installLocation .. "/src/*.h", self.installLocation .. "/src/*.c"}

        removefiles {self.installLocation .. "/src/rcore_*.c"}

        -- sorry to my MACOS people...
        -- filter { "system:macosx", "files:" .. currentLibdir .. "/src/rglfw.c" }
        --     compileas "Objective-C"

        filter{}
end


return library;