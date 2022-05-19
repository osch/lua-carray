#include <jack/jack.h>

#define CARRAY_CAPI_IMPLEMENT_SET_CAPI 1

#include "util.h"
#include "main.h"
#include "carray_capi_impl.h"
#include "carray.h"

#ifndef CARRAY_VERSION
    #error CARRAY_VERSION is not defined
#endif 

#define CARRAY_STRINGIFY(x) #x
#define CARRAY_TOSTRING(x) CARRAY_STRINGIFY(x)
#define CARRAY_VERSION_STRING CARRAY_TOSTRING(CARRAY_VERSION)

/* ============================================================================================ */

static const char* const CARRAY_MODULE_NAME = "carray";

/* ============================================================================================ */


DLL_PUBLIC int luaopen_carray(lua_State* L)
{
    luaL_checkversion(L); /* does nothing if compiled for Lua 5.1 */

    /* ---------------------------------------- */

    int n = lua_gettop(L);
    
    int module      = ++n; lua_newtable(L);

    lua_pushliteral(L, CARRAY_VERSION_STRING);
    lua_setfield(L, module, "_VERSION");
    
    lua_checkstack(L, LUA_MINSTACK);
    
    carray_init_module(L, module);

    lua_newtable(L);                                   /* -> module meta */
    lua_pushstring(L, CARRAY_MODULE_NAME);             /* -> module meta, "carray" */
    lua_setfield(L, -2, "__metatable");                /* -> module meta */
    carray_set_capi(L, -1, &carray_capi_impl);         /* -> module meta */    
    lua_setmetatable(L, module);                       /* -> */
    
    lua_settop(L, module);

    return 1;
}

/* ============================================================================================ */
