-- optional options -----------------------------------------------------------------
newoption {
    trigger = "vendor_directory",
    value = "path",
    description = "Relative or Absolute filepath that should contain vendor library source code",
    default = "/../vendors",
}
newoption {
    trigger = "vendor_include",
    value = "path",
    description = "Relative or Absolute filepath that should contain vendor library header code",
    default = "/../vendors/include",
}
newoption {
    trigger = "backend",
    value = "API",
    description = "Windowing backend",
    default = "GLFW",
}
newoption {
    trigger = "graphics",
    description = "Opengl graphics specification",
    default = "opengl43",
}
-- globals  -------------------------------------------------------------------------
workspacePath = os.getcwd();
workspaceName = path.getbasename(workspacePath)
workspaceBuildPath = workspacePath .. "/.build"
vendorDirPath = workspacePath .. _OPTIONS["vendor_directory"]
vendorIncludePath = workspacePath .. _OPTIONS["vendor_include"]


include "/vendors/buildUtils"

workspace(workspaceName)
    configurations { "Debug", "Release"}
    platforms { "x64", "x86", "ARM64"}

    defaultplatform ("x64")

    filter "configurations:Debug"
        defines { "DEBUG", "IMGUI_EDITOR_ENABLED=1"}
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "IMGUI_EDITOR_ENABLED=0" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

    cdialect "C17"
    cppdialect "C++20"

    IncludeVendorsInProject()

    AddGroup("Libraries", {table.unpack(os.matchdirs("*"))})
    AddGroup("Applications", {table.unpack(os.matchdirs("Apps/*"))})
