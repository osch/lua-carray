#include <limits.h>

#define CARRAY_CAPI_IMPLEMENT_SET_CAPI 1

#include "util.h"
#include "carray.h"
#include "carray_capi_impl.h"

/* ============================================================================================ */

const char* const CARRAY_CLASS_NAME = "carray";

/* ============================================================================================ */


typedef enum ElementType ElementType;

enum ElementType
{
    TYPE_UNSIGNED_CHAR = 0, 
    TYPE_UCHAR,
    TYPE_CHAR,
    
    TYPE_SIGNED_CHAR, 
    TYPE_SCHAR,
    
    TYPE_SHORT,
    
    TYPE_UNSIGNED_SHORT,
    TYPE_USHORT,
    
    TYPE_INT,
    
    TYPE_UNSIGNED_INT,
    TYPE_UINT,
    
    TYPE_LONG,
    
    TYPE_UNSIGNED_LONG,
    TYPE_ULONG,
    
#if CARRAY_CAPI_HAVE_LONG_LONG
    TYPE_LONG_LONG,
    TYPE_LLONG,
    
    TYPE_UNSIGNED_LONG_LONG,
    TYPE_UNSIGNED_LLONG,
    TYPE_ULLONG,
#endif

    TYPE_FLOAT,
    TYPE_DOUBLE,
    
    TYPE_INTEGER,
    TYPE_NUMBER,
    
    TYPE_INT8,
    TYPE_UINT8,
    
    TYPE_INT16,
    TYPE_UINT16,
    
    TYPE_INT32,
    TYPE_UINT32,
    
    TYPE_INT64,
    TYPE_UINT64,
    
    TYPE_FLOAT32,
    TYPE_FLOAT64
};

static const char* const elementTypeNames[] =
{
    "unsigned char",
    "uchar",
    "char",
    
    "signed char",
    "schar",

    "short",
    
    "unsigned short",
    "ushort",
    
    "int",
    
    "unsigned int",
    "uint",
    
    "long",
    
    "unsigned long",
    "ulong",
    
#if CARRAY_CAPI_HAVE_LONG_LONG
    "long long",
    "llong",
    
    "unsigned long long",
    "unsigned llong",
    "ullong",
#endif

    "float",
    "double",
    
    "integer",
    "number",
    
    "int8",
    "uint8",
    
    "int16",
    "uint16",
    
    "int32",
    "uint32",
    
    "int64",
    "uint64",
    
    "float32",
    "float64",
    
    NULL
};

/* ============================================================================================ */

#define CHARSIZE(bits) (bits + CHAR_BIT - 1)/CHAR_BIT

static int Carray_new(lua_State* L)
{
    int arg = 1;

    int typeArg = arg++;
    ElementType elementType = luaL_checkoption(L, typeArg, NULL, elementTypeNames);

    size_t elementCount = 0;
    if (!lua_isnoneornil(L, arg)) {
        elementCount = luaL_checkinteger(L, arg++);
    }

    bool   isInteger   = false;
    bool   isUnsigned  = false;
    bool   isFloat     = false;
    size_t elementSize = 0;

    carray_type normalizedType = 0;
    
    switch (elementType)
    {
        case TYPE_UNSIGNED_CHAR: 
        case TYPE_UCHAR:
        case TYPE_CHAR:          normalizedType = CARRAY_UCHAR;  isInteger = true; elementSize = sizeof(unsigned char); isUnsigned = true; break;
        
        case TYPE_SIGNED_CHAR:
        case TYPE_SCHAR:         normalizedType = CARRAY_SCHAR;  isInteger = true; elementSize = sizeof(signed char); break;
        
        case TYPE_SHORT:         normalizedType = CARRAY_SHORT;  isInteger = true; elementSize = sizeof(short); break;
    
        case TYPE_UNSIGNED_SHORT:
        case TYPE_USHORT:        normalizedType = CARRAY_USHORT; isInteger = true; elementSize = sizeof(unsigned short); break;
    
        case TYPE_INT:           normalizedType = CARRAY_INT;    isInteger = true; elementSize = sizeof(int); break;
        
        case TYPE_UNSIGNED_INT:
        case TYPE_UINT:          normalizedType = CARRAY_UINT;   isInteger = true; elementSize = sizeof(unsigned int); isUnsigned = true; break;
        
        case TYPE_LONG:          normalizedType = CARRAY_LONG;   isInteger = true; elementSize = sizeof(long); break;

        case TYPE_UNSIGNED_LONG:
        case TYPE_ULONG:         normalizedType = CARRAY_ULONG;  isInteger = true; elementSize = sizeof(unsigned long long); isUnsigned = true; break;
        
#if CARRAY_CAPI_HAVE_LONG_LONG
        case TYPE_LONG_LONG:
        case TYPE_LLONG:         normalizedType = CARRAY_LLONG;  isInteger = true; elementSize = sizeof(long long); break;

        case TYPE_UNSIGNED_LONG_LONG:
        case TYPE_UNSIGNED_LLONG:
        case TYPE_ULLONG:        normalizedType = CARRAY_ULLONG; isInteger = true; elementSize = sizeof(unsigned long); isUnsigned = true; break;
#endif
        case TYPE_FLOAT:         normalizedType = CARRAY_FLOAT;  isFloat = true; elementSize = sizeof(float); break;
        case TYPE_DOUBLE:        normalizedType = CARRAY_DOUBLE; isFloat = true; elementSize = sizeof(double); break;
        
        case TYPE_INTEGER: isInteger = true; elementSize = sizeof(lua_Integer); break;
        case TYPE_NUMBER:  isFloat   = true; elementSize = sizeof(lua_Number); break;
        
        case TYPE_INT8:    elementSize = CHARSIZE(8); isInteger = true; break;
        case TYPE_UINT8:   elementSize = CHARSIZE(8); isInteger = true; isUnsigned = true; break;
        
        case TYPE_INT16:   elementSize = CHARSIZE(16); isInteger = true; break;
        case TYPE_UINT16:  elementSize = CHARSIZE(16); isInteger = true; isUnsigned = true; break;
        
        case TYPE_INT32:   elementSize = CHARSIZE(32); isInteger = true; break;
        case TYPE_UINT32:  elementSize = CHARSIZE(32); isInteger = true; isUnsigned = true; break;
        
        case TYPE_INT64:   elementSize = CHARSIZE(64); isInteger = true; break;
        case TYPE_UINT64:  elementSize = CHARSIZE(64); isInteger = true; isUnsigned = true; break;
        
        case TYPE_FLOAT32: elementSize = CHARSIZE(32); break;
        case TYPE_FLOAT64: elementSize = CHARSIZE(64); break;
    }
    if (!normalizedType) {
        if (isInteger) {
            if (isUnsigned) {
                if (elementSize <= sizeof(unsigned char)) {
                    normalizedType = CARRAY_UCHAR;
                } else if (elementSize <= sizeof(unsigned short)) {
                    normalizedType = CARRAY_USHORT;
                } else if (elementSize <= sizeof(unsigned int)) {
                    normalizedType = CARRAY_UINT;
                } else if (elementSize <= sizeof(unsigned long)) {
                    normalizedType = CARRAY_ULONG;
#if CARRAY_CAPI_HAVE_LONG_LONG
                } else if (elementSize <= sizeof(unsigned long long)) {
                    normalizedType = CARRAY_ULLONG;
#endif
                }
            } else {
                if (elementSize <= sizeof(signed char)) {
                    normalizedType = CARRAY_SCHAR;
                } else if (elementSize <= sizeof(signed short)) {
                    normalizedType = CARRAY_SHORT;
                } else if (elementSize <= sizeof(signed int)) {
                    normalizedType = CARRAY_INT;
                } else if (elementSize <= sizeof(signed long)) {
                    normalizedType = CARRAY_LONG;
#if CARRAY_CAPI_HAVE_LONG_LONG
                } else if (elementSize <= sizeof(long long)) {
                    normalizedType = CARRAY_LLONG;
#endif
                }
            }
        } else {
                if (elementSize <= sizeof(float)) {
                    normalizedType = CARRAY_FLOAT;
                } else if (elementSize <= sizeof(double)) {
                    normalizedType = CARRAY_DOUBLE;
                }
        }
    }
    carray* created = NULL;
    
    if (normalizedType && elementSize > 0) {
        created = carray_capi_impl.newCarray(L, normalizedType, CARRAY_DEFAULT, elementCount, NULL);
    }
    if (!created) {
        return luaL_error(L, "cannot create carray for type '%s'", lua_tostring(L, typeArg));
    }
    return 1;
}

/* ============================================================================================ */

static CarrayUserData* checkReadableUdata(lua_State* L, int index)
{
    CarrayUserData* udata = luaL_checkudata(L, index, CARRAY_CLASS_NAME);
    if (udata->impl) {
        return udata;   
    } else {
        luaL_argerror(L, index, "invalid carray");
    }
}

/* ============================================================================================ */

static CarrayUserData* checkWritableUdata(lua_State* L, int index)
{
    CarrayUserData* udata = checkReadableUdata(L, index);
    if (!(udata->impl->attr & CARRAY_READONLY)) {
        return udata;
    } else {
        luaL_argerror(L, index, "carray is not writable");
    }
}

/* ============================================================================================ */

static const char* typeToString(carray* impl)
{
    if (impl) {
        switch (impl->type) {
            case CARRAY_UCHAR:   return "unsigned char";
            case CARRAY_SCHAR:   return "signed char";
            
            case CARRAY_SHORT:   return "short";
            case CARRAY_USHORT:  return "unsigned short";
            
            case CARRAY_INT:     return "int";
            case CARRAY_UINT:    return "unsigned int";
            
            case CARRAY_LONG:    return "long";
            case CARRAY_ULONG:   return "unsigned long";
            
#if CARRAY_CAPI_HAVE_LONG_LONG
            case CARRAY_LLONG:   return "long long";
            case CARRAY_ULLONG:  return "unsigned long long";
#endif
            case CARRAY_FLOAT:   return "float";
            case CARRAY_DOUBLE:  return "double";
            
        }
    }
    return "?";
}

/* ============================================================================================ */

static int Carray_len(lua_State* L)
{
    CarrayUserData* udata = checkReadableUdata(L, 1);
    lua_pushinteger(L, udata->impl->elementCount);
    return 1;
}

/* ============================================================================================ */

static int Carray_getstring(lua_State* L)
{
    CarrayUserData* udata = checkReadableUdata(L, 1);
    lua_Integer index1;
    lua_Integer index2;
    
    const size_t count = udata->impl->elementCount;
    
    if (lua_gettop(L) == 1) {
        index1 = 1;
        index2 = count;
    } else {
        if (!lua_isnoneornil(L, 2)) {
            index1 = luaL_checkinteger(L, 2);
        } else {
            index1 = 1;
        }
        if (!lua_isnoneornil(L, 3)) {
            index2 = luaL_checkinteger(L, 3);
        } else {
            index2 = index1;
        }
    }
    if (udata->impl) {
        if (udata->impl->elementSize != 1) {
            return luaL_argerror(L, 1, "carray type is not char");
        }
        if (index1 >= 0) {
            index1 -= 1;
        } else {
            index1 = count + index1;
        }
        if (index2 >= 0) {
            index2 -= 1;
        } else {
            index2 = count + index2;
        }
        if (   0 <= index1 && index1 < count
            && 0 <= index2 && index2 < count)
        {
            char* ptr = udata->impl->buffer + index1;
            const int count = index2 - index1  + 1;
            if (count <= 0) {
                return 0;
            }
            lua_pushlstring(L, ptr, count);
            return 1;
        }
    }
    return luaL_argerror(L, 2, "index out of bounds");
}

/* ============================================================================================ */

static int Carray_get(lua_State* L)
{
    CarrayUserData* udata = checkReadableUdata(L, 1);
    
    lua_Integer index1 = luaL_checkinteger(L, 2);
    lua_Integer index2;
    
    if (!lua_isnoneornil(L, 3)) {
        index2 = luaL_checkinteger(L, 3);
    } else {
        index2 = index1;
    }
    size_t count = udata->impl->elementCount;
    if (index1 >= 0) {
        index1 -= 1;
    } else {
        index1 = count + index1;
    }
    if (index2 >= 0) {
        index2 -= 1;
    } else {
        index2 = count + index2;
    }
    if (   0 <= index1 && index1 < count
        && 0 <= index2 && index2 < count)
    {
        carray_type elementType = udata->impl->type;
        size_t      elementSize = udata->impl->elementSize;
        char* ptr = udata->impl->buffer + index1 * elementSize;
        const int count = index2 - index1  + 1;
        int c = count;
        if (count > 0) {
            luaL_checkstack(L, count, NULL);
        }
        switch (elementType) {
            case CARRAY_UCHAR: {
                unsigned char* p = (unsigned char*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_SCHAR: {
                signed char* p = (signed char*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_SHORT: {
                short* p = (short*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_USHORT: {
                unsigned short* p = (unsigned short*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_INT: {
                int* p = (int*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_UINT: {
                unsigned int* p = (unsigned int*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_LONG: {
                long* p = (long*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_ULONG: {
                unsigned long* p = (unsigned long*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
#if CARRAY_CAPI_HAVE_LONG_LONG
            case CARRAY_LLONG: {
                long long* p = (long long*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
            case CARRAY_ULLONG: {
                unsigned long long* p = (unsigned long long*)ptr;
                while (--c >= 0) { lua_pushinteger(L, *(p++)); }
                break;
            }
#endif                
            case CARRAY_FLOAT: {
                float* p = (float*)ptr;
                while (--c >= 0) { lua_pushnumber(L, *(p++)); }
                break;
            }
            case CARRAY_DOUBLE: {
                double* p = (double*)ptr;
                while (--c >= 0) { lua_pushnumber(L, *(p++)); }
                break;
            }
            default: return luaL_argerror(L, 1, "internal type error");
        }
        return count > 0 ? count : 0;
    }
    return luaL_argerror(L, 2, "index out of bounds");
}


/* ============================================================================================ */

static int Carray_setstring(lua_State* L)
{
    CarrayUserData* udata = checkWritableUdata(L, 1);
    
    lua_Integer index = luaL_checkinteger(L, 2);

    if (udata->impl->elementSize != 1) {
        return luaL_argerror(L, 1, "carray type is not char");
    }
    size_t count = udata->impl->elementCount;
    if (index >= 0) {
        index -= 1;
    } else {
        index = count + index;
    }
    if (0 <= index && index < count)
    {
        char* ptr    = udata->impl->buffer + index;
        int   args   = lua_gettop(L) - 2;
        char* ptrEnd = udata->impl->buffer + count;
        
        for (int i = 0; i < args; ++i) {
            if (lua_type(L, 3 + i) != LUA_TSTRING) {
                return luaL_argerror(L, 3 + i, "string expected");
            }
            size_t len;
            const char* s = lua_tolstring(L, 3 + i, &len);
            if (ptr + len > ptrEnd) {
                return luaL_argerror(L, 3 + i, "index out of bounds");
            }
            memcpy(ptr, s, len);
            ptr += len;
        }
        return 0;
    }
    return luaL_argerror(L, 2, "index out of bounds");
}

/* ============================================================================================ */

static int Carray_set(lua_State* L)
{
    CarrayUserData* udata = checkWritableUdata(L, 1);

    lua_Integer index = luaL_checkinteger(L, 2);
    
    size_t count = udata->impl->elementCount;
    if (index >= 0) {
        index -= 1;
    } else {
        index = count + index;
    }
    if (0 <= index && index < count)
    {
        size_t  elementSize = udata->impl->elementSize;
        char*   ptr         = udata->impl->buffer + index * elementSize;
        int     args        = lua_gettop(L) - 2;

        if (index + args > count) {
            return luaL_argerror(L, lua_gettop(L) + 1 - (index + args - count), "index out of bounds");
        }
        int a = 2;
        switch (udata->impl->type) {
            case CARRAY_UCHAR: {
                unsigned char* p = (unsigned char*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_SCHAR: {
                signed char* p = (signed char*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_SHORT: {
                short* p = (short*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_USHORT: {
                unsigned short* p = (unsigned short*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_INT: {
                int* p = (int*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_UINT: {
                unsigned int* p = (unsigned int*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_LONG: {
                long* p = (long*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_ULONG: {
                unsigned long* p = (unsigned long*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
#if CARRAY_CAPI_HAVE_LONG_LONG
            case CARRAY_LLONG: {
                long long* p = (long long*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
            case CARRAY_ULLONG: {
                unsigned long long* p = (unsigned long long*)ptr;
                while (--args >= 0) { *(p++) = luaL_checkinteger(L, ++a); }
                break;
            }
#endif                
            case CARRAY_FLOAT: {
                float* p = (float*)ptr;
                while (--args >= 0) { *(p++) = luaL_checknumber(L, ++a); }
                break;
            }
            case CARRAY_DOUBLE: {
                double* p = (double*)ptr;
                while (--args >= 0) { *(p++) = luaL_checknumber(L, ++a); }
                break;
            }
            default: return luaL_argerror(L, 1, "internal type error");
        }
        
        return 0;
    }
    return luaL_argerror(L, 2, "index out of bounds");
}

/* ============================================================================================ */

static int Carray_type(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);
    lua_pushstring(L, typeToString(udata->impl));
    return 1;
}

/* ============================================================================================ */

static int Carray_bitwidth(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);
    lua_pushinteger(L, udata->impl ? (udata->impl->elementSize * CHAR_BIT) : 0);
    return 1;
}

/* ============================================================================================ */

static int Carray_writable(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);
    lua_pushboolean(L, udata->impl && !(udata->impl->attr & CARRAY_READONLY));
    return 1;
}

/* ============================================================================================ */

static int Carray_resizable(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);
    // TODO future implementation
    lua_pushboolean(L, false);
    return 1;
}

/* ============================================================================================ */

static int Carray_toString(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);
    lua_pushfstring(L, "%s<%s>: %p", CARRAY_CLASS_NAME, typeToString(udata->impl), udata);     /* -> rslt */
    return 1;
}

/* ============================================================================================ */

static int Carray_release(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);

    if (udata->impl) {
        carray_capi_impl.releaseCarray(udata->impl);
        udata->impl = NULL;
    }
    return 1;
}

/* ============================================================================================ */

static const luaL_Reg ModuleFunctions[] = 
{
    { "new",            Carray_new   },
    { NULL,             NULL } /* sentinel */
};

static const luaL_Reg CarrayMethods[] = 
{   
    { "len",        Carray_len       },
    { "get",        Carray_get       },
    { "set",        Carray_set       },
    { "tostring",   Carray_getstring },
    { "setstring",  Carray_setstring },
    { "type",       Carray_type      },
    { "bitwidth",   Carray_bitwidth  },
    { "writable",   Carray_writable  },
    { "resizable",  Carray_resizable },
    { NULL,         NULL } /* sentinel */
};

static const luaL_Reg CarrayMetaMethods[] = 
{
    { "__tostring", Carray_toString },
    { "__gc",       Carray_release  },

    { NULL,       NULL } /* sentinel */
};

/* ============================================================================================ */

static void setupCarrayMeta(lua_State* L)
{                                                      /* -> meta */
    lua_pushstring(L, CARRAY_CLASS_NAME);              /* -> meta, className */
    lua_setfield(L, -2, "__metatable");                /* -> meta */

    luaL_setfuncs(L, CarrayMetaMethods, 0);            /* -> meta */
    
    lua_newtable(L);                                   /* -> meta, CarrayClass */
    luaL_setfuncs(L, CarrayMethods, 0);                /* -> meta, CarrayClass */
    lua_setfield (L, -2, "__index");                   /* -> meta */
    carray_set_capi(L, -1, &carray_capi_impl);         /* -> meta */
}

/* ============================================================================================ */

int carray_push_meta(lua_State* L)
{
    if (luaL_newmetatable(L, CARRAY_CLASS_NAME)) {
        setupCarrayMeta(L);
    }
    return 1;
}

/* ============================================================================================ */

int carray_init_module(lua_State* L, int module)
{
    if (luaL_newmetatable(L, CARRAY_CLASS_NAME)) {
        setupCarrayMeta(L);
    }
    lua_pop(L, 1);
    
    lua_pushvalue(L, module);
        luaL_setfuncs(L, ModuleFunctions, 0);
    lua_pop(L, 1);

    return 0;
}

/* ============================================================================================ */
