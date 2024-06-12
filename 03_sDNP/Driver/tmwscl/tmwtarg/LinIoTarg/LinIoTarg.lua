-- LinIoTarg.lua

project "LinIoTarg"
   location "."
   kind "StaticLib"
   language "C"

   files { "*.h", "*.c" }
   includedirs { "..", "../../../" }

