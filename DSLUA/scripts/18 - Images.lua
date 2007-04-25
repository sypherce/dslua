SCREEN_TOP = 1
SCREEN_BOTTOM = 0

BGTop16Bit = Screen.Load16BitBG()
BGBotText = Screen.LoadTextBG()
Screen.Initialize(SCREEN_TOP, BGTop16Bit)
Screen.Initialize(SCREEN_BOTTOM, BGBotText)

BGBotText:PrintXY(0, 0, "Loading mario.bmp...")

file = assert(io.open("mario.bmp", "rb"))
image = file:read("*all")

BGBotText:PrintXY(0, 2, "BMP Height: " .. tostring(Image.BMPHeight(image)))
BGBotText:PrintXY(0, 3, "BMP Width: " .. tostring(Image.BMPWidth(image)))

Image.LoadBMP(SCREEN_TOP, 0, 0, image)

BGBotText:PrintXY(0, 5, "Press A to continue.")

while not Pads.A() do end
while Pads.A() do end

BGBotText:Clear()
BGTop16Bit:Clear()
image = nil

BGBotText:PrintXY(0, 0, "Loading dslua.jpg...")

file = assert(io.open("dslua.jpg", "rb"))
image = file:read("*all")

Image.LoadJPG(SCREEN_TOP, image)

BGBotText:PrintXY(0, 2, "Press A to continue.")

while not Pads.A() do end
while Pads.A() do end

BGBotText:Clear()
BGTop16Bit:Clear()
image = nil

BGBotText:PrintXY(0, 0, "Loading banana.gif...")

file = assert(io.open("banana.gif", "rb"))
image = file:read("*all")

Image.LoadGIF(SCREEN_TOP, image)

BGBotText:PrintXY(0, 2, "GIF Width: " .. Image.GIFWidth(image))
BGBotText:PrintXY(0, 3, "GIF Height: " .. Image.GIFHeight(image))

BGBotText:PrintXY(0, 5, "Press A to reload animation.")
BGBotText:PrintXY(0, 6, "Press START to quit.")

while not Pads.Start() do
	if Pads.A() then
		while Pads.A() do end
		BGTop16Bit:Clear()
		Image.LoadGIF(SCREEN_TOP, image)
	end
end
