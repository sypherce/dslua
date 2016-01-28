# How to Setup and Use DSLua #

  * Welcome to DSLua
  * CF Flavor
  * GBFS Flavor
  * Auto-start
  * gfx2gba
  * More Information

## Welcome to DSLua ##

DSLua is a port of the [Lua scripting language](http://www.lua.org/) to the [Nintendo DS](http://www.nintendo.com/systemsds). Originally made by waruwaru, now maintained by sypherce, daltonlaffs, and me, Jeremysr. You can get it or the source code from [DSLua.com](http://www.dslua.com/component/option,com_docman/task,cat_view/gid,9/Itemid,21/), on the download page. This document will explain how to use DSLua and write programs for it.

## CF Flavor ##

DSLua comes in two flavors: CF flavor, and GBFS flavor. This section will explain the CF flavor.

## GBFS Flavor ##

The GBFS flavor packs your lua files (and graphics, music, sound effects, whatever you need to run your program) into a single DS executable, so you don't have to have all the lua files on your storage device, like in the CF flavor.

To pack the files, you must first put your files in the scripts directory (found in the main directory of DSLua GBFS flavor). Then, run PACK.BAT. It should put all the files from the scripts directory and pack them into bundle/DSLUA-PACK.nds (or .ds.gba or .sc.nds). Then you run whichever one is compatible with your homebrew device, and when run it will let you choose a packed file to run (lua files will be highlighted in green).

## Auto-start ##

The Auto-start feature lets you make a DSLua file that will automatically run a certain file on the storage device (or GBFS). See auto-start.bat for instructions and an example.

## gfx2gba ##

Currently your game sprites must be in .raw/.pal format, and your backgrounds must be in .raw/.pal/.map format. To convert them, use gfx2gba.exe which is found in the bin directory. From the command line, run this to make a sprite:

gfx2gba -poutput.pal input.bmp

The output will be a palette file (output.pal) and a raw image file with the same name as your input (in this example it would be input.raw). The input file must be a 256-colour bitmap (.bmp). Its width and height must be 16, 32, or 64 pixels. (Examples: 16x16, 32x64, 64x16...)

To make a background (tile map), you need to add -m to the command:

gfx2gba -m -poutput.pal input.bmp

With this example it will output three files: input.raw, input.map, and output.pal. Like sprites, the input bitmap must be 256 colours and the width and height must be in multiples of 8.

## More Information ##
  * [Programming in Lua](http://www.lua.org/pil)
  * [Lua Manual (DSLua currently uses Lua 5.0)](http://www.lua.org/manual/)
  * [DSLua.com (good place to ask for help)](http://www.dslua.com/)
  * [Jeremysr's DSLua Tutorials](http://www.bio-gaming.com/jeremy/dslua/index.php?act=tutorials)

[Back to index](DSLuaDocumentation.md)