rmdir /s /q release-cf-blue
rmdir /s /q release-cf-green
rmdir /s /q release-cf-gray
rmdir /s /q release-gbfs

set CFSUPPORT=1
make clean
make
set CFSUPPORT=0

rmdir /s /q gba_nds_fat
mkdir gba_nds_fat
xcopy libfatdragon_blue\*.* gba_nds_fat
mkdir release-cf-blue
mkdir release-cf-blue\bin
mkdir release-cf-blue\scripts
copy PACK.BAT release-cf-blue\PACK.BAT
copy ProcessGFX.BAT release-cf-blue\ProcessGFX.BAT
copy ProcessMap.BAT release-cf-blue\ProcessMap.BAT
copy DSLua.nds release-cf-blue\DSLua.nds
copy DSLua.sc.nds release-cf-blue\DSLua.sc.nds
copy DSLua.ds.gba release-cf-blue\DSLua.ds.gba
xcopy text\*.* release-cf-blue
xcopy bin\*.* release-cf-blue\bin
xcopy scripts\*.* release-cf-blue\scripts

make clean
make


set CFSUPPORT=1
make clean
make
set CFSUPPORT=0

rmdir /s /q gba_nds_fat
mkdir gba_nds_fat
xcopy libfatdragon_green\*.* gba_nds_fat
mkdir release-cf-green
mkdir release-cf-green\bin
mkdir release-cf-green\scripts
copy PACK.BAT release-cf-green\PACK.BAT
copy ProcessGFX.BAT release-cf-green\ProcessGFX.BAT
copy ProcessMap.BAT release-cf-green\ProcessMap.BAT
copy DSLua.nds release-cf-green\DSLua.nds
copy DSLua.sc.nds release-cf-green\DSLua.sc.nds
copy DSLua.ds.gba release-cf-green\DSLua.ds.gba
xcopy text\*.* release-cf-green
xcopy bin\*.* release-cf-green\bin
xcopy scripts\*.* release-cf-green\scripts

make clean
make


set CFSUPPORT=1
make clean
make
set CFSUPPORT=0

rmdir /s /q gba_nds_fat
mkdir gba_nds_fat
xcopy libfatdragon_gray\*.* gba_nds_fat
mkdir release-cf-gray
mkdir release-cf-gray\bin
mkdir release-cf-gray\scripts
copy PACK.BAT release-cf-gray\PACK.BAT
copy ProcessGFX.BAT release-cf-gray\ProcessGFX.BAT
copy ProcessMap.BAT release-cf-gray\ProcessMap.BAT
copy DSLua.nds release-cf-gray\DSLua.nds
copy DSLua.sc.nds release-cf-gray\DSLua.sc.nds
copy DSLua.ds.gba release-cf-gray\DSLua.ds.gba
xcopy text\*.* release-cf-gray
xcopy bin\*.* release-cf-gray\bin
xcopy scripts\*.* release-cf-gray\scripts

make clean
make

mkdir release-gbfs
mkdir release-gbfs\bin
mkdir release-gbfs\scripts
mkdir release-gbfs\bundle
copy PACK.BAT release-gbfs\PACK.BAT
copy ProcessGFX.BAT release-gbfs\ProcessGFX.BAT
copy ProcessMap.BAT release-gbfs\ProcessMap.BAT
copy DSLua.nds release-gbfs\DSLua.nds
copy DSLua.sc.nds release-gbfs\DSLua.sc.nds
copy DSLua.ds.gba release-gbfs\DSLua.ds.gba
copy DSLua-PACK.nds release-gbfs\bundle\DSLua-PACK.nds
copy DSLua-PACK.sc.nds release-gbfs\bundle\DSLua-PACK.sc.nds
copy DSLua-PACK.ds.gba release-gbfs\bundle\DSLua-PACK.ds.gba
xcopy text\*.* release-gbfs
xcopy bin\*.* release-gbfs\bin
xcopy scripts\*.* release-gbfs\scripts
