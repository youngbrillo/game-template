basename = path.getbasename(os.getcwd())

project(basename)
    cppdialect "C++20"
    language "C++"
    kind "ConsoleApp"
    location    (workspaceBuildPath)
    targetdir   ( workspaceBuildPath.. "/bin/%{cfg.buildcfg}")

    filter "action:vs*"
        debugdir (os.getcwd())
        debugargs {"app.config.yml"}

    filter {"action:vs*", "configurations:Release"}
        kind "WindowedApp"
        entrypoint "mainCRTStartup"

    filter{}

    vpaths
    {
        ["Application Resource Files/*"] = {"src/**.rc", "src/**.ico"},
    }
    files {
        "./src/*.c", "./src/**.cpp", "./src/**.h", "./src/**.hpp",
       "./include/**.h", "./include/**.hpp"
    }

    filter "system:windows"
        files {"res/**.rc", "res/**.ico"}
        resincludedirs { "src/**" }
    filter{}

    filter "files:**.ico"
        buildaction "Embed"

    filter{}


    includedirs { "/src", "/include", vendorIncludePath }

    LinkToLibrary("lib")

    IncludeVendor("yaml-cpp")
    IncludeVendor("imgui")
    IncludeVendor("box3D")

    LinkVendor("raylib")
    LinkVendor("yaml-cpp")
    LinkVendor("imgui")
    LinkVendor("box3D")

