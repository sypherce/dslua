SCREEN_TOP = 1
SCREEN_BOTTOM = 0
BGTopText = Screen.LoadTextBG()
BGBot16Bit = Screen.Load16BitBG()
Screen.Initialize( SCREEN_TOP, BGTopText )
Screen.Initialize( SCREEN_BOTTOM, BGBot16Bit )

-- Draw a (colorful) border
BGBot16Bit:Line(0, 0, 255, 0, 0, 31, 0)
BGBot16Bit:Line(255, 0, 255, 191, 31, 0, 0)
BGBot16Bit:Line(255, 191, 0, 191, 31, 15, 0)
BGBot16Bit:Line(0, 191, 0, 0, 31, 31, 0)

Color = { Red = 15, Green = 15, Blue = 15 }

BGTopText:PrintXY( 1, 1, "\[Up/Down\]    Red   : " .. tostring(Color.Red) )
BGTopText:PrintXY( 1, 2, "\[Left/Right\] Green : " .. tostring(Color.Green) )
BGTopText:PrintXY( 1, 3, "\[L/R\]        Blue  : " .. tostring(Color.Blue) )

while not Pads.Start() do
	if Stylus.Down() then
		BGBot16Bit:Plot( Stylus.X(), Stylus.Y(), Color.Red, Color.Green, Color.Blue )
	end

	if Pads.Up() then Color.Red = Color.Red + 1 end
	if Pads.Down() then Color.Red = Color.Red - 1 end
	if Pads.Left() then Color.Green = Color.Green - 1 end
	if Pads.Right() then Color.Green = Color.Green + 1 end
	if Pads.L() then Color.Blue = Color.Blue - 1 end
	if Pads.R() then Color.Blue = Color.Blue + 1 end
	if Pads.Select() then BGBot16Bit:Clear() end

	if Color.Red == 32 then Color.Red = 31 end
	if Color.Red == -1 then Color.Red = 0 end
	if Color.Green == 32 then Color.Green = 31 end
	if Color.Green == -1 then Color.Green = 0 end
	if Color.Blue == 32 then Color.Blue = 31 end
	if Color.Blue == -1 then Color.Blue = 0 end

	if Pads.AnyKey() then
		BGTopText:PrintXY( 22, 1, tostring( Color.Red ) .. "  " )
		BGTopText:PrintXY( 22, 2, tostring( Color.Green ) .. "  " )
		BGTopText:PrintXY( 22, 3, tostring( Color.Blue ) .. "  " )
	end

	Screen.WaitForVBL()
end
