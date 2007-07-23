-- Here is the standard 'my first script'

-- set up screens for text output
SCREEN_BOTTOM   = 0
GL_PROJECTION = 0
GL_QUADS = 1
GL_MODELVIEW = 2
BGBotText = Screen.LoadTextBG()
Screen.Initialize( SCREEN_BOTTOM, BGBotText )

-- say hello
print( "Hello world!" )
print( "Welcome to " .. "DSLua" )
print( "Visit us at www.DSLua.com" )
print( "Press any button to exit" )
DSGL.Init3D()
while 1 do
DSGL.ClearColor(0, 255, 0)
DSGL.ClearDepth(32767)
DSGL.Reset()
DSGL.MatrixMode(GL_PROJECTION)
DSGL.LoadIdentity()
DSGL.Ortho(0, 1, 1, 0, 1, 0)
DSGL.PushMatrix()
DSGL.Translatef(0, 0, -1)
DSGL.MatrixMode(GL_MODELVIEW)
DSGL.PolyFmt(2031808)
DSGL.Rotatef(43, 0, 0, 1)
DSGL.Begin(GL_QUADS)
DSGL.Color3b(255, 255, 255)
DSGL.Vertex2f(0, 0)
DSGL.Vertex2f(1, 0)
DSGL.Vertex2f(1, 1)
DSGL.Vertex2f(0, 1)
DSGL.End()

DSGL.PopMatrix(1)
DSGL.Flush()
end

-- wait until a key is pressed
DSLua.WaitForAnyKey()