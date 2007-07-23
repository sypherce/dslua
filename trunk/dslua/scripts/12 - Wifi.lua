-- Here is the standard 'my first script'

-- set up screens for text output
SCREEN_BOTTOM   = 0
BGBotText = Screen.LoadTextBG()
Screen.Initialize( SCREEN_BOTTOM, BGBotText )

PCIP = "192.168.1.2"

-- say hello
print( "Connecting to Wifi!" )
if Wifi.ConnectWFC() then
	print( "Connected!" )
	Buffer = "....."
	Sock = Wifi.InitUDP(PCIP,9501)
	print( "UDP Connected!" )
	q = 1;
	a = 0
	b = 0
	x = 0
	y = 0
	l = 0
	r = 0
	start = 0
	select = 0
	up = 0
	down = 0
	left = 0
	right = 0
			Sock:Send("/p0")
	while q > 0 do
		if Pads.A() and a == 0 then
			Sock:Send("/ba0")
			a = 1
		elseif not Pads.A() and a == 1 then
			Sock:Send("/ba1")
			a = 0
		end
		if Pads.B() and b == 0 then
			Sock:Send("/bb0")
			b = 1
		elseif not Pads.B() and b == 1 then
			Sock:Send("/bb1")
			b = 0
		end
		if Pads.X() and x == 0 then
			Sock:Send("/bx0")
			x = 1
		elseif not Pads.X() and x == 1 then
			Sock:Send("/bx1")
			x = 0
		end
		if Pads.Y() and y == 0 then
			Sock:Send("/by0")
			y = 1
		elseif not Pads.Y() and y == 1 then
			Sock:Send("/by1")
			y = 0
		end
		if Pads.L() and l == 0 then
			Sock:Send("/bl0")
			l = 1
		elseif not Pads.L() and l == 1 then
			Sock:Send("/bl1")
			l = 0
		end
		if Pads.R() and r == 0 then
			Sock:Send("/br0")
			r = 1
		elseif not Pads.R() and r == 1 then
			Sock:Send("/br1")
			r = 0
		end
		if Pads.Select() and select == 0 then
			Sock:Send("/be0")
			select = 1
		elseif not Pads.Select() and select == 1 then
			Sock:Send("/be1")
			select = 0
		end
		if Pads.Up() and up == 0 then
			Sock:Send("/du0")
			up = 1
		elseif not Pads.Up() and up == 1 then
			Sock:Send("/du1")
			up = 0
		end
		if Pads.Down() and down == 0 then
			Sock:Send("/dd0")
			down = 1
		elseif not Pads.Down() and down == 1 then
			Sock:Send("/dd1")
			down = 0
		end
		if Pads.Left() and left == 0 then
			Sock:Send("/dl0")
			left = 1
		elseif not Pads.Left() and left == 1 then
			Sock:Send("/dl1")
			left = 0
		end
		if Pads.Right() and right == 0 then
			Sock:Send("/dr0")
			right = 1
		elseif not Pads.Right() and right == 1 then
			Sock:Send("/dr1")
			right = 0
		end
		if Pads.Start() then
			q = 0
		end
	end
	-- received = Sock:RecvFrom("192.168.1.2",Buffer, 6, 0)
	-- if received then
	-- print(Buffer)
	-- else
	-- print("Failed To Receieve")
	-- end
	-- Sock:Close()
	Wifi.Disconnect()
	print( "Disconnected!" )
end

-- wait until a key is pressed
DSLua.WaitForAnyKey()