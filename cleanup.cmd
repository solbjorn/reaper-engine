RD /s /q 3rd_party\_LIB
RD /s /q 3rd_party\_TEMP
del 3rd_party\Src\luajit2\luajit2\src\*.exe
del 3rd_party\Src\luajit2\luajit2\src\*.lib
del 3rd_party\Src\luajit2\luajit2\src\*.pdb
del 3rd_party\Src\luajit2\luajit2\src\lua51.*
RD /s /q 3rd_party\Src\mimalloc\_Build
RD /s /q 3rd_party\Src\oneTBB\_Build
RD /s /q 3rd_party\Src\openal-soft\_Build
RD /s /q ogsr_engine\_LIB
RD /s /q ogsr_engine\_TEMP
RD /s /q bin_x86\
RD /s /q bin_x64\
for /d %%B in (.vs\Engine\*) do (
	del %%B\*.VC.*
	del %%B\*.bin*
	RD /s /q %%B\ipch
)
pause
