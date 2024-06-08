-- WinIoTarg.lua

project "winiotarg"
   location "."
   kind "SharedLib"
   language "C++"
   objdir "%{cfg.buildcfg}"
      
   defines { "WINIOTARG_EXPORTS", "_AFXDLL", "_WINDOWS" }
   removedefines "TMWCNFG_INCLUDE_ASSERTS"
   files { "*.h", "*.cpp" }
   includedirs { "..", "../../../", "../../../thirdPartyCode/openssl", "../../../thirdPartyCode/openssl/inc32" }

   filter "configurations:Debug"
      targetdir "../../../bind"
	  libdirs { "../../../thirdPartyCode/openssl/out32dll.dbg" }

   filter "configurations:Release"
      targetdir "../../../bin"
	  libdirs { "../../../thirdPartyCode/openssl/out32dll" }
