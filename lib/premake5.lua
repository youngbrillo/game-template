basename = path.getbasename(os.getcwd())

project(basename)
    cppdialect "C++20"
    language "C++"
    kind "StaticLib"
    location    (workspaceBuildPath)
    targetdir   ( workspaceBuildPath.. "/bin/%{cfg.buildcfg}")

    files {
        "*.c", "**.cpp", "**.h", "**.hpp",
    }
		
    filter{}

    includedirs { "./", vendorIncludePath }

    IncludeVendor("raylib")
    IncludeVendor("yaml-cpp")
    IncludeVendor("imgui")
    IncludeVendor("box3D")
    IncludeVendor("lua")