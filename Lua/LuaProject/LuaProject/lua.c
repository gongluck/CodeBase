#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int main()
{
    int res = 0;
    char luaStr[] = "";
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    res = luaL_loadfile(L,"test.lua");
    res = lua_pcall(L,0,0,0);
    if(res != 0)
    {
        printf(lua_tostring(L,-1));
        lua_pop(L,1);
    }

    lua_close(L);

    system("pause");
    return 0;
}