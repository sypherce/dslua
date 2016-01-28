# How to Setup and Use DSLua #

  * Welcome to DSLua
  * Installation
  * Running Programs
  * Auto-start
  * gfx2gba
  * More Information

## Welcome to DSLua ##

DSLua is a port of the [Lua scripting language](http://www.lua.org/) to the [Nintendo DS](http://www.nintendo.com/systemsds). Originally made by waruwaru, now maintained by sypherce, daltonlaffs, and me, Jeremysr. You can get it or the source code from [DSLua.com](http://www.dslua.com/component/option,com_docman/task,cat_view/gid,9/Itemid,21/), on the download page. This document will explain how to use DSLua and write programs for it.

## Installation ##

  1. Download DSLua 0.7 from the [DSLua.com download page](http://www.dslua.com/component/option,com_docman/task,cat_view/gid,9/Itemid,21/).
  1. Unzip it with some program like WinZip.
  1. Patch the DSLua ROM (either DSLua.nds or DSLua.sc.nds or DSLua.ds.gba) with [DLDI](http://chishm.drunkencoders.com/DLDI/).
  1. Copy the _scripts_ directory, which was in the download, to the root directory of your card. You only need scripts/main.lua which is a file browser that lets you execute lua scripts (DSLua won't work without it.) All the other files are just example programs.
  1. Copy the DSLua ROM to your card.
  1. That's all you have to do, it's now installed!

## Running Programs ##

First find some games/apps for DSLua at these or other web pages:

  * [DSLua.com projects forum](http://www.dslua.com/component/option,com_smf/Itemid,19/board,8.0)
  * [Dev-Scene.com: DSLua Releases](http://www.dev-scene.com/NDS/DSLua_Releases)

I recommend making a directory on the root of your card called "dslua", and putting all the DSLua homebrew you download in there, each in their own directory inside the dslua directory. This is because many DSLua projects come with around 10 to 50 different files for all the code, graphics, save files, and sound/music. Also a lot of games/apps using the Auto-start feature want you to have the game directory inside the /dslua directory.

After you've put the game files onto your card, run DSLua.nds (or DSLua.sc.nds or DSLua.ds.gba...). The file browser should run. If it doesn't you either have to patch it with DLDI or you didn't copy the "scripts" directory to the root of your card. (Read the installation section to fix these problems.) Navigate to the directory where your DSLua program is stored using the up, down, and a buttons. Find the .lua file that you're supposed to run. Most programs will come with more than one .lua file. The one you should run is usually called either main.lua or (The title of the game).lua. Select this file and press A to run it.

## Auto-start ##

The Auto-start feature lets you make a DSLua file that will automatically run a certain file on the storage device (or GBFS). See auto-start.bat for instructions and an example.

## gfx2gba ##

Currently your game sprites must be in .raw/.pal format, and your backgrounds must be in .raw/.pal/.map format. To convert them, use gfx2gba.exe which is found in the bin directory. From the command line, run this to make a sprite:

gfx2gba -poutput.pal input.bmp

The output will be a palette file (output.pal) and a raw image file with the same name as your input (in this example it would be input.raw). The input file must be a 256-color bitmap (.bmp). Its width and height must be 16, 32, or 64 pixels. (Examples: 16x16, 32x64, 64x16...)

To make a background (tile map), you need to add -m to the command:

gfx2gba -m -poutput.pal input.bmp


With this example it will output three files: input.raw, input.map, and output.pal. Like sprites, the input bitmap must be 256 colors and the width and height must be in multiples of 8.

## More Information ##
  * [Programming in Lua](http://www.lua.org/pil)
  * [Lua Manual (DSLua currently uses Lua 5.0)](http://www.lua.org/manual/)
  * [DSLua.com (good place to ask for help)](http://www.dslua.com/)
  * [Jeremysr's DSLua Tutorials](http://www.bio-gaming.com/jeremy/dslua/index.php?act=tutorials)

[Back to index](DSLuaDocumentation.md)