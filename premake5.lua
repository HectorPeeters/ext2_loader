workspace "Ext2"
	language "C"
	architecture "x86_64"
	configurations { "Debug", "Release" }
	
	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"
	
	filter { "configurations:Release" }
		optimize "On"
	
	targetdir ("build/bin/%{prj.name}/%{cfg.longname}")
    objdir ("build/obj/%{prj.name}/%{cfg.longname}")
    
project "Ext2"
    kind "ConsoleApp"

    files { "src/*.c" }

    includedirs { "include/" }
