RD /s /q 3rd_party\_LIB
RD /s /q 3rd_party\_TEMP
RD /s /q 3rd_party\Src\DirectX\DirectXTex\_Build
RD /s /q 3rd_party\Src\FreeType\SheenBidi\_Build
RD /s /q 3rd_party\Src\FreeType\freetype\_Build
RD /s /q 3rd_party\Src\FreeType\freetype\_Build2
RD /s /q 3rd_party\Src\FreeType\harfbuzz\_Build
RD /s /q 3rd_party\Src\FreeType\harfbuzz\_Build2
RD /s /q 3rd_party\Src\FreeType\plutosvg\_Build
RD /s /q 3rd_party\Src\FreeType\plutovg\_Build
RD /s /q 3rd_party\Src\OpCoDe\_Build
RD /s /q 3rd_party\Src\SFML\_Build
RD /s /q 3rd_party\Src\Xiph\flac\_Build
RD /s /q 3rd_party\Src\Xiph\ogg\_Build
RD /s /q 3rd_party\Src\Xiph\theora\_Build
RD /s /q 3rd_party\Src\Xiph\vorbis\_Build
RD /s /q 3rd_party\Src\abseil\_Build
RD /s /q 3rd_party\Src\compression\lz4\_Build
RD /s /q 3rd_party\Src\compression\squashfs\_Build
RD /s /q 3rd_party\Src\compression\xxHash\_Build
RD /s /q 3rd_party\Src\compression\zstd\_Build
RD /s /q 3rd_party\Src\llvm\_Build
RD /s /q 3rd_party\Src\luajit2\_Build
RD /s /q 3rd_party\Src\mimalloc\_Build
RD /s /q 3rd_party\Src\openal-soft\_Build
RD /s /q 3rd_party\Src\tinyxml2\_Build
RD /s /q 3rd_party\Src\tmc\hwloc\_Build
RD /s /q ogsr_engine\_LIB
RD /s /q ogsr_engine\_TEMP
RD /s /q bin_x64\
for /d %%B in (.vs\Engine\*) do (
	del %%B\*.VC.*
	del %%B\*.bin*
	RD /s /q %%B\ipch
)
pause
