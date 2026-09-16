vendor_libs = {}

function string.starts(text, start)
    return string.sub(text, 1, string.len(text)) == start
end

function AddGroup(name, directory)
    group(name)

    for _, folderName in ipairs(directory) do
        if( string.starts(folderName, "build") == false
            and string.starts(folderName, ".") == false
        )then
            if(os.isfile(folderName .. "/premake5.lua")) then
                include (folderName)
            end
        end
    end
end

function LinkToLibrary(lib, p_path)
    links (lib)

    local path = p_path or workspacePath
    includedirs (path.."/"..lib.."/include")
    includedirs (path.."/"..lib.."/")
end

function LinkVendor(lib)
    vendor_libs[lib]:link()
end


function IncludeVendor(lib)
    vendor_libs[lib]:include()
end

function SearchForVendors()
    local files = os.matchfiles("./vendors/libraries/*.lua")
    if #files == 0 then 
        print "\t\t no vendors found"
    end
    for _, filename in ipairs(files) do 
        local lib = include(filename)
        vendor_libs[lib.name] = lib;

        print ("\t\t- found library '" .. lib.name .. "'")
    end
end

function IncludeVendorsInProject()
    SearchForVendors()
    local workingDir = os.getcwd()

    group "Vendors"
        for name, lib in pairs(vendor_libs) do
            lib:tryLoad();
            os.chdir(lib.installLocation)

            lib:build();
            os.chdir(workingDir);
            

        end
end