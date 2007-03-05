#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "DSLGL.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_DSGLInit3D(lua_State * lState)
{
    PA_Init3D();
    lua_pushboolean(lState, 1);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_DSGLLoadIdentity(lua_State * lState)
{
    glLoadIdentity();
    lua_pushboolean(lState, 1);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_DSGLMaterialShinyness(lua_State * lState)
{
    glMaterialShinyness();
    lua_pushboolean(lState, 1);
    return 1;
}
//------------------------------------------------------------
//------------------------------------------------------------
static int l_DSGLInit3DAndBg(lua_State * lState)
{
    //PA_Init3DAndBg();
    lua_pushboolean(lState, 1);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_DSGLInit3DDrawing(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);
    float eyeX = luaL_checknumber(lState, 4);
    float eyeY = luaL_checknumber(lState, 5);
    float eyeZ = luaL_checknumber(lState, 6);

    //PA_Init3DDrawing(x,y,z,eyeX,eyeY,eyeZ);
    lua_pushboolean(lState, 1);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_DSGL3DBox(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);
    float w = luaL_checknumber(lState, 4);
    float h = luaL_checknumber(lState, 5);
    float d = luaL_checknumber(lState, 6);
    float rX = luaL_checknumber(lState, 7);
    float rY = luaL_checknumber(lState, 8);
    float rZ = luaL_checknumber(lState, 9);
    u8 r = luaL_checkint(lState, 10);
    u8 g = luaL_checkint(lState, 11);
    u8 b = luaL_checkint(lState, 12);

    //PA_3DBox(x,y,z,w,h,d,rX,rY,rZ,r,g,b);
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLPushMatrix(lua_State * lState)
{
    glPushMatrix();
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLFlush(lua_State * lState)
{
    glFlush();
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLPopMatrix(lua_State * lState)
{
    u32 n = luaL_checkint(lState, 1);

    glPopMatrix(n);
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLVueKeys(lua_State * lState)
{
    //PA_VueKeys();
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLResetTextures(lua_State * lState)
{
    glResetTextures();
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLResetMatrixStack(lua_State * lState)
{
    glResetMatrixStack();
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLReset(lua_State * lState)
{
    glReset();
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLIdentity(lua_State * lState)
{
    glIdentity();
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLBegin(lua_State * lState)
{
    u32 val = luaL_checkint(lState, 1);

    glBegin(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int l_DSGLEnd(lua_State * lState)
{
    glEnd();
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGL3Dto2D(lua_State * lState)
{
    //PA_3Dto2D();
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLTexture(lua_State * lState)
{
    int val = luaL_checkint(lState, 1);

    //PA_Texture(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLEnable(lua_State * lState)
{
    int val = luaL_checkint(lState, 1);

    glEnable(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLDisable(lua_State * lState)
{
    int val = luaL_checkint(lState, 1);

    glDisable(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLRestoreMatrix(lua_State * lState)
{
    int32 val = luaL_checkint(lState, 1);

    glRestoreMatrix(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLStoreMatrix(lua_State * lState)
{
    int32 val = luaL_checkint(lState, 1);

    glStoreMatrix(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLNormal(lua_State * lState)
{
    uint32 val = luaL_checkint(lState, 1);

    glNormal(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLMatrixMode(lua_State * lState)
{
    int val = luaL_checkint(lState, 1);

    glMatrixMode(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLPolyFmt(lua_State * lState)
{
    int val = luaL_checkint(lState, 1);

    glPolyFmt(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLAlphaFunc(lua_State * lState)
{
    int val = luaL_checkint(lState, 1);

    glAlphaFunc(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLTexCoord1i(lua_State * lState)
{
    uint32 val = luaL_checkint(lState, 1);

    glTexCoord1i(val);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLRotate3D(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    //PA_Rotate3D(x,y,z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLTranslate3D(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    //PA_Translate3D(x,y,z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLScale3D(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    //PA_Scale3D(x,y,z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLVertex3D(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    //PA_Vertex3D(x,y,z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLVertex2D(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);

    //PA_Vertex2D(x,y);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLScalef(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    glScalef(x, y, z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLTranslatef(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    glTranslatef(x, y, z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLNormal3f(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    glNormal3f(x, y, z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLClearColor(lua_State * lState)
{
    uint8 x = luaL_checkint(lState, 1);
    uint8 y = luaL_checkint(lState, 2);
    uint8 z = luaL_checkint(lState, 3);

    glClearColor(x, y, z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLColor3b(lua_State * lState)
{
    uint8 x = luaL_checkint(lState, 1);
    uint8 y = luaL_checkint(lState, 2);
    uint8 z = luaL_checkint(lState, 3);

    glColor3b(x, y, z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLColor3f(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);
    float z = luaL_checknumber(lState, 3);

    glColor3f(x, y, z);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLViewPort(lua_State * lState)
{
    uint8 x1 = luaL_checkint(lState, 1);
    uint8 y1 = luaL_checkint(lState, 2);
    uint8 x2 = luaL_checkint(lState, 3);
    uint8 y2 = luaL_checkint(lState, 4);

    glViewPort(x1, y1, x2, y2);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLTexCoord(lua_State * lState)
{
    float x = luaL_checknumber(lState, 1);
    float y = luaL_checknumber(lState, 2);

    //PA_TexCoord(x,y);
    lua_pushboolean(lState, 1);
    return 1;
}

static int lDSGLClearDepth(lua_State * lState)
{
    uint16 n = luaL_checkint(lState, 3);

    glClearDepth(n);
    lua_pushboolean(lState, 1);
    return 1;
}

//static int l_DSGLMaterialShinnyness( lua_State* lState )
//{
//	glMaterialShinnyness();
//	lua_pushboolean( lState, 1 );
//	return 1;
//}

static const struct luaL_reg DSLGLLib [] =
{
    {"Init3D", l_DSGLInit3D},
    {"LoadIdentity", l_DSGLLoadIdentity},
    //{ "Init3DAndBg", l_DSGLInit3DAndBg },
    //{ "Init3DDrawing", l_DSGLInit3DDrawing },
    //{ "GL3DBox", l_DSGL3DBox },
    {"PushMatrix", l_DSGLPushMatrix},
    {"PopMatrix", l_DSGLPopMatrix},
    {"Flush", l_DSGLFlush},
    //{ "VueKeys", l_DSGLVueKeys },
    {"ResetTextures", l_DSGLResetTextures},
    {"ResetMatrixStack", l_DSGLResetMatrixStack},
    {"Reset", l_DSGLReset},
    {"Identity", l_DSGLIdentity},
    {"Begin", l_DSGLBegin},
    {"End", l_DSGLEnd},
    //{ "GL3Dto2D", lDSGL3Dto2D },
    //{ "Texture", lDSGLTexture },
    {"Enable", lDSGLEnable},
    {"Disable", lDSGLDisable},
    {"RestoreMatrix", lDSGLRestoreMatrix},
    {"StoreMatrix", lDSGLStoreMatrix},
    {"Normal", lDSGLNormal},
    {"MatrixMode", lDSGLMatrixMode},
    {"PolyFmt", lDSGLPolyFmt},
    {"AlphaFunc", lDSGLAlphaFunc},
    {"TexCoord1i", lDSGLTexCoord1i},
    //{ "Texture", lDSGLTexture },
    //{ "Rotate3D", lDSGLRotate3D },
    //{ "Translate3D", lDSGLTranslate3D },
    //{ "Scale3D", lDSGLScale3D },
    //{ "Vertex2D", lDSGLVertex3D },
    //{ "Vertex3D", lDSGLVertex3D },
    {"Scalef", lDSGLScalef},
    {"Translatef", lDSGLTranslatef},
    {"Normal3f", lDSGLNormal3f},
    {"ClearColor", lDSGLClearColor},
    {"Color3b", lDSGLColor3b},
    {"Color3f", lDSGLColor3f},
    {"ViewPort", lDSGLViewPort},
    //{ "TexCoord", lDSGLTexCoord },
    {"ClearDepth", lDSGLClearDepth},
    {"MaterialShinyness", l_DSGLMaterialShinyness},
    {NULL, NULL}
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLGLLib(lua_State * lState)
{
    luaL_openlib(lState, "dsgl", DSLGLLib, 0);
    return 1;
}

