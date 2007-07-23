-- Here is the standard 'my first script'
SCREEN_BOTTOM   = 0
BGBotText = Screen.LoadTextBG()
Screen.Initialize( SCREEN_BOTTOM, BGBotText )
if(not Rumble.IsInserted())then
BGBotText:PrintXY(0,0, "Please insert rumble pack!" )
end
while(not Rumble.IsInserted())do
end
BGBotText:PrintXY(0,0, "Thank you for inserting a rumble pack!" )
BGBotText:PrintXY(0,2, "Touch the screen to activate the rumble pack." )
BGBotText:PrintXY(0,3, "Right side is faster, left side is slower." )
BGBotText:PrintXY(0,4, "Press the start button to exit." )

rumbleValue = 0
rumbleBool = 0
while 1 do
	rumbleValue = rumbleValue + 1;
	rumbleActivate = Stylus.X()
	if(rumbleActivate <= rumbleValue and Pads.A())then
		if(rumbleBool == 1)then
			rumbleBool = 0
		else
			rumbleBool = 1
		end
		rumbleValue = 0
		Rumble.SetRumble(rumbleBool)
	end
	if(Pads.Start()) then
	    break
	end
end
