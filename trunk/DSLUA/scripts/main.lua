-- Here is the standard 'my first script'

-- set up screens for text output
SCREEN_BOTTOM   = 0
BGBotText = Screen.LoadTextBG()
Screen.Initialize( SCREEN_BOTTOM, BGBotText )

-- say hello
print( "Hello world!" )
print( "Welcome to " .. "DSLua" )
print( "Visit us at www.DSLua.com" )
print( "Press any button to exit" )

dir = Directory.Directory()
dir:Open("/")
q = dir:Reset()
dir:Close()

print(q);

-- wait until a key is pressed
DSLua.WaitForAnyKey()