#include <limits.h>
#include <stdio.h>
#include <stddef.h>

#define CARRAY_CAPI_IMPLEMENT_SET_CAPI 1
#define CARRAY_CAPI_IMPLEMENT_GET_CAPI 1

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
        case TYPE_USHORT:        normalizedType = CARRAY_USHORT; isInteger = true; elementSize = sizeof(unsigned short); isUnsigned = true; break;
    
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
        case TYPE_FLOAT:         normalizedType = CARRAY_FLOAT;                    elementSize = sizeof(float); break;
        case TYPE_DOUBLE:        normalizedType = CARRAY_DOUBLE;                   elementSize = sizeof(double); break;
        
        case TYPE_INTEGER: isInteger = true; elementSize = sizeof(lua_Integer); break;
        case TYPE_NUMBER:                    elementSize = sizeof(lua_Number); break;
        
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
        return NULL;
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
        return NULL;
    }
}

/* ============================================================================================ */

static const char* typeToString(carray* impl)
{
    if (impl) {
        switch (impl->elementType) {
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

static const char* baseTypeToString(carray* impl)
{
    if (impl) {
        switch (impl->elementType) {
            case CARRAY_UCHAR:   
            case CARRAY_USHORT:  
            case CARRAY_UINT:    
            case CARRAY_ULONG:   
#if CARRAY_CAPI_HAVE_LONG_LONG
            case CARRAY_ULLONG:  
#endif                           
                                 return "uint";

            case CARRAY_SCHAR:   
            case CARRAY_SHORT:   
            case CARRAY_INT:     
            case CARRAY_LONG:    
#if CARRAY_CAPI_HAVE_LONG_LONG
            case CARRAY_LLONG:   
#endif                           
                                 return "int";
                                 
            case CARRAY_FLOAT:   
            case CARRAY_DOUBLE:  return "float";
            
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
    int arg = 1;
    CarrayUserData* udata = checkReadableUdata(L, arg++);

    if (udata->impl->elementSize != 1) {
        return luaL_argerror(L, 1, "carray type is not char");
    }

    lua_Integer index1;
    lua_Integer index2;
    
    const size_t totalCount = udata->impl->elementCount;
    
    if (lua_gettop(L) == 1) {
        index1 = 1;
        index2 = totalCount;
    } else {
        index1 = luaL_checkinteger(L, arg++);
        index2 = luaL_checkinteger(L, arg++);
    }
    if (index1 < 0) {
        index1 = totalCount + index1 + 1;
    }
    if (index1 <= 1) {
        index1 = 1;
    }
    if (index2 < 0) {
        index2 = totalCount + index2 +  1;
    }
    if (index2 > totalCount) {
        index2 = totalCount;
    }
    
    const int count = index2 - index1  + 1;
    if (count > 0) {
        char* ptr = udata->impl->buffer + index1 - 1;
        lua_pushlstring(L, ptr, count);
    } else {
        lua_pushstring(L, "");
    }
    return 1;
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
    size_t totalCount = udata->impl->elementCount;
    if (index1 >= 0) {
        index1 -= 1;
    } else {
        index1 = totalCount + index1;
    }
    if (index2 >= 0) {
        index2 -= 1;
    } else {
        index2 = totalCount + index2;
    }
    if (index2 < -1) {
        index2 = -1;
    }
    int nilOffset = 0;
    if (index1 < 0) {
        luaL_checkstack(L, -index1, NULL);
        while (index1 < 0) {
            lua_pushnil(L);
            index1 += 1;
            nilOffset += 1;
        }
    }
    carray_type elementType = udata->impl->elementType;
    size_t      elementSize = udata->impl->elementSize;
    char* ptr = udata->impl->buffer + index1 * elementSize;
    int count = index2 - index1  + 1;
    if (index1 + count > totalCount) {
        count = totalCount - index1;
    }
    if (count > 0) {
        luaL_checkstack(L, count, NULL);
    }
    int c = count;
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
    return count > 0 ? (nilOffset + count) : nilOffset;
}


/* ============================================================================================ */

static int Carray_set(lua_State* L)
{
    CarrayUserData* udata = checkWritableUdata(L, 1);
    carray*         impl  = udata->impl;
    
    lua_Integer index = luaL_checkinteger(L, 2);
    
    size_t count = impl->elementCount;
    if (index >= 0) {
        index -= 1;
    } else {
        index = count + index;
    }
    if (index < 0 || index >= count) {
        return luaL_argerror(L, 2, "index out of bounds");
    }
    char*   ptr         = impl->buffer;
    int     topArg      = lua_gettop(L);

    int i = index;
    int a = 3;

again:
    switch (impl->elementType) {
        case CARRAY_UCHAR: {
            unsigned char* p = (unsigned char*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_SCHAR: {
            signed char* p = (signed char*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_SHORT: {
            short* p = (short*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_USHORT: {
            unsigned short* p = (unsigned short*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_INT: {
            int* p = (int*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_UINT: {
            unsigned int* p = (unsigned int*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_LONG: {
            long* p = (long*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_ULONG: {
            unsigned long* p = (unsigned long*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
#if CARRAY_CAPI_HAVE_LONG_LONG
        case CARRAY_LLONG: {
            long long* p = (long long*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
        case CARRAY_ULLONG: {
            unsigned long long* p = (unsigned long long*)ptr;
            while (i < count && a <= topArg) { if (!lua_isinteger(L, a)) goto special;
                                               p[i++] = lua_tointeger(L, a++); }
            break;
        }
#endif                
        case CARRAY_FLOAT: {
            float* p = (float*)ptr;
            while (i < count && a <= topArg) { if (lua_type(L, a) != LUA_TNUMBER) goto special;
                                               p[i++] = lua_tonumber(L, a++); }
            break;
        }
        case CARRAY_DOUBLE: {
            double* p = (double*)ptr;
            while (i < count && a <= topArg) { if (lua_type(L, a) != LUA_TNUMBER) goto special;
                                               p[i++] = lua_tonumber(L, a++); }
            break;
        }
        default: return luaL_argerror(L, 1, "internal type error");
    }
    if (a <= topArg) {
        return luaL_argerror(L, a, "index out of bounds");
    }

    lua_settop(L, 1);
    return 1;

special:;
    bool added = false;
    if ((impl->elementType == CARRAY_UCHAR || impl->elementType == CARRAY_SCHAR) && lua_type(L, a) == LUA_TSTRING)
    {
        size_t len = 0;
        const char* str = lua_tolstring(L, a, &len);
        if (len > 0) {
            if (i + len > count) {
                return luaL_argerror(L, a, "index out of bounds");
            }
            memcpy(ptr + i, str, len);
            i += len;
        }
        added = true;
    }
    else {
        int errorReason = 0;
        const carray_capi* otherCapi = carray_get_capi(L, a, &errorReason);
        const carray* otherCarray = NULL;
        carray_info otherInfo;
        if (otherCapi) {
            otherCarray = otherCapi->toReadableCarray(L, a, &otherInfo);
        }
        if (otherCarray) {
            if (otherInfo.elementType == impl->elementType) {
                size_t otherCount = otherInfo.elementCount;
                if (otherCount > 0) {
                    if (i + otherCount > count) {
                        return luaL_argerror(L, a, "index out of bounds");
                    }
                    memcpy(ptr + i * impl->elementSize, otherCapi->getReadableElementPtr(otherCarray, 0, otherCount), otherCount * otherInfo.elementSize);
                    i += otherCount;
                }
                added = true;
            } 
            else {
                return luaL_argerror(L, a, lua_pushfstring(L, "carray type mismatch, expected: %s<%s>", CARRAY_CLASS_NAME, typeToString(impl)));
            }
        }
        else if (errorReason == 1) {
            return luaL_argerror(L, a, "carray version mismatch");
        }
    }
    if (added) {
        ++a;
        goto again;
    }
    if (impl->elementType == CARRAY_FLOAT || impl->elementType == CARRAY_DOUBLE) {
        return luaL_argerror(L, a, "number or carray expected");
    } else if (impl->elementType == CARRAY_UCHAR || impl->elementType == CARRAY_SCHAR) {
        return luaL_argerror(L, a, "integer, carray or string expected");
    } else {
        return luaL_argerror(L, a, "integer or carray expected");
    }
}

/* ============================================================================================ */

static int calcGrowReservePercent(carray* impl)
{
    if (impl->elementCount < 10 * 1000 * 1000) {
        return 100;
    }
    else if (impl->elementCount < 100 * 1000 * 1000) {
        return 50;
    }
    else {
        return 10;
    }
}


/* ============================================================================================ */

static int internalInsert(lua_State* L, carray* impl, int insertPos, int firstArg)
{
    int     arg         = firstArg;
    int     nargs       = lua_gettop(L) - arg + 1;
    size_t  pos0        = insertPos - 1;
    void*   ptr0        = carray_capi_impl.insertElements(impl, pos0, nargs, calcGrowReservePercent(impl));
    if (!ptr0) {
        return luaL_error(L, "adding elements failed");
    }
    int remaining = nargs;

again:
    switch (impl->elementType) {
        case CARRAY_UCHAR: {
            unsigned char* p = (unsigned char*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_SCHAR: {
            signed char* p = (signed char*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_SHORT: {
            short* p = (short*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_USHORT: {
            unsigned short* p = (unsigned short*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_INT: {
            int* p = (int*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_UINT: {
            unsigned int* p = (unsigned int*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_LONG: {
            long* p = (long*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_ULONG: {
            unsigned long* p = (unsigned long*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
#if CARRAY_CAPI_HAVE_LONG_LONG
        case CARRAY_LLONG: {
            long long* p = (long long*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
        case CARRAY_ULLONG: {
            unsigned long long* p = (unsigned long long*)ptr0;
            while (--remaining >= 0) { if (!lua_isinteger(L, arg)) goto special; 
                                       *(p++) = lua_tointeger(L, arg++); }
            break;
        }
#endif                
        case CARRAY_FLOAT: {
            float* p = (float*)ptr0;
            while (--remaining >= 0) { if (lua_type(L, arg) != LUA_TNUMBER) goto special; 
                                       *(p++) = lua_tonumber(L, arg++); }
            break;
        }
        case CARRAY_DOUBLE: {
            double* p = (double*)ptr0;
            while (--remaining >= 0) { if (lua_type(L, arg) != LUA_TNUMBER) goto special; 
                                       *(p++) = lua_tonumber(L, arg++); }
            break;
        }
        default: return luaL_argerror(L, 1, "internal type error");
    }
    lua_settop(L, 1);
    return 1;
    
special:
    remaining += 1;
    int currPos = pos0 + nargs - remaining;
    if (remaining >= 0) {
        carray_capi_impl.removeElements(impl, currPos, remaining, 0);
    }
    bool added = false;
    size_t addedCount = 0;
    {
        int errorReason = 0;
        const carray_capi* otherCapi = carray_get_capi(L, arg, &errorReason);
        const carray* otherCarray = NULL;
        carray_info otherInfo;
        if (otherCapi) {
            otherCarray = otherCapi->toReadableCarray(L, arg, &otherInfo);
        }
        if (otherCarray) {
            if (otherInfo.elementType == impl->elementType) {
                size_t otherCount = otherInfo.elementCount;
                if (otherCount > 0) {
                    ptr0 = carray_capi_impl.insertElements(impl, currPos, otherCount, calcGrowReservePercent(impl));
                    if (!ptr0) {
                        return luaL_error(L, "adding elements failed");
                    }
                    memcpy(ptr0, otherCapi->getReadableElementPtr(otherCarray, 0, otherCount), otherCount * otherInfo.elementSize);
                }
                added = true;
                addedCount = otherCount;
            } 
            else {
                return luaL_argerror(L, arg, lua_pushfstring(L, "carray type mismatch, expected: %s<%s>", CARRAY_CLASS_NAME, typeToString(impl)));
            }
        }
        else if (errorReason == 1) {
            return luaL_argerror(L, arg, "carray version mismatch");
        }
    }
    if (lua_type(L, arg) == LUA_TSTRING && (impl->elementType == CARRAY_UCHAR || impl->elementType == CARRAY_SCHAR))
    {
        size_t len = 0;
        const char* str = lua_tolstring(L, arg, &len);
        if (len > 0) {
            ptr0 = carray_capi_impl.insertElements(impl, currPos, len, calcGrowReservePercent(impl));
            if (!ptr0) {
                return luaL_error(L, "adding elements failed");
            }
            memcpy(ptr0, str, len);
        }
        added = true;
        addedCount = len;
    }
    if (added) 
    {
        arg += 1;
        remaining -= 1;
        pos0 = currPos + addedCount;
        nargs = remaining;
        if (nargs > 0) {
            ptr0 = carray_capi_impl.insertElements(impl, pos0, nargs, calcGrowReservePercent(impl));
            if (!ptr0) {
                return luaL_error(L, "adding elements failed");
            }
            goto again;
        } else {
            lua_settop(L, 1);
            return 1;
        }
    }
    if (impl->elementType == CARRAY_FLOAT || impl->elementType == CARRAY_DOUBLE) {
        return luaL_argerror(L, arg, "number or carray expected");
    } else if (impl->elementType == CARRAY_UCHAR || impl->elementType == CARRAY_SCHAR) {
        return luaL_argerror(L, arg, "integer, carray or string expected");
    } else {
        return luaL_argerror(L, arg, "integer or carray expected");
    }
}

/* ============================================================================================ */

static int Carray_append(lua_State* L)
{
    int arg = 1;
    CarrayUserData* udata = checkWritableUdata(L, arg++);
    carray*         impl  = udata->impl;

    return internalInsert(L, impl, impl->elementCount + 1, arg);
}

/* ============================================================================================ */

static int Carray_insert(lua_State* L)
{
    int arg = 1;
    CarrayUserData* udata = checkWritableUdata(L, arg++);
    carray*         impl  = udata->impl;

    int insertPos = luaL_checkinteger(L, arg);
    if (insertPos <= 0 || insertPos > impl->elementCount + 1) {
        return luaL_argerror(L, arg, "index out of bounds");
    }
    arg += 1;
    
    return internalInsert(L, impl, insertPos, arg);
}

/* ============================================================================================ */

static int Carray_appendsub(lua_State* L)
{
    int arg = 1;
    carray* impl1 = checkWritableUdata(L, arg++)->impl;

    size_t otherCount = 0;
    const char* str   = NULL;
    carray*     impl2 = NULL;
    
    if ((impl1->elementType == CARRAY_UCHAR || impl1->elementType == CARRAY_SCHAR) && lua_type(L, arg) == LUA_TSTRING)
    {
        str = lua_tolstring(L, arg, &otherCount);
    }
    else {
        impl2 = checkReadableUdata(L, arg)->impl;
        if (impl2->elementType != impl1->elementType) {
            return luaL_argerror(L, arg, lua_pushfstring(L, "carray type mismatch, expected: %s<%s>", CARRAY_CLASS_NAME, typeToString(impl1)));
        }
        otherCount = impl2->elementCount;
    }
    ++arg;
    
    lua_Integer index1 = luaL_checkinteger(L, arg++);
    if (index1 < 0) {
        index1 = otherCount + index1 + 1;
    }
    lua_Integer index2 = luaL_checkinteger(L, arg++);
    if (index2 < 0) {
        index2 = otherCount + index2 + 1;
    }
    if (index1 < 1) {
        index1 = 1;
    }
    if (index2 > otherCount) {
        index2 = otherCount;
    }
    lua_Integer count = index2 - index1 + 1;
    if (count > 0) {
        void* dest = carray_capi_impl.insertElements(impl1, impl1->elementCount, count, calcGrowReservePercent(impl1));
        if (!dest) {
            return luaL_error(L, "insert into carray failed");
        }
        const void* src;
        if (str) {
            src = str + index1 - 1;
        } else {
            src = carray_capi_impl.getReadableElementPtr(impl2, index1 - 1, count);
        }
        memcpy(dest, src, count * impl1->elementSize);
    }
    lua_settop(L, 1);
    return 1;
}

/* ============================================================================================ */

static int Carray_insertsub(lua_State* L)
{
    int arg = 1;
    carray* impl1 = checkWritableUdata(L, arg++)->impl;
    
    lua_Integer insertPos = luaL_checkinteger(L, arg);
    if (insertPos <= 0 || insertPos > impl1->elementCount + 1) {
        return luaL_argerror(L, arg, "index out of bounds");
    }
    ++arg;
    
    size_t otherCount = 0;
    const char* str   = NULL;
    carray*     impl2 = NULL;

    if ((impl1->elementType == CARRAY_UCHAR || impl1->elementType == CARRAY_SCHAR) && lua_type(L, arg) == LUA_TSTRING)
    {
        str = lua_tolstring(L, arg, &otherCount);
    }
    else {
        impl2 = checkReadableUdata(L, arg)->impl;
        if (impl2->elementType != impl1->elementType) {
            return luaL_argerror(L, arg, lua_pushfstring(L, "carray type mismatch, expected: %s<%s>", CARRAY_CLASS_NAME, typeToString(impl1)));
        }
        otherCount = impl2->elementCount;
    }
    ++arg;
    
    lua_Integer index1 = luaL_checkinteger(L, arg++);
    if (index1 < 0) {
        index1 = otherCount + index1 + 1;
    }
    lua_Integer index2 = luaL_checkinteger(L, arg++);
    if (index2 < 0) {
        index2 = otherCount + index2 + 1;
    }
    if (index1 < 1) {
        index1 = 1;
    }
    if (index2 > otherCount) {
        index2 = otherCount;
    }
    lua_Integer fromPos = index1;
    lua_Integer count   = index2 - index1 + 1;
    if (count > 0) {
        char* dest = carray_capi_impl.insertElements(impl1, insertPos - 1, count, calcGrowReservePercent(impl1));
        if (!dest) {
            return luaL_error(L, "insert into carray failed");
        }
        if (str) {
            memcpy(dest, str + fromPos - 1, count);
        } 
        else {
            if (impl1 != impl2) {
                const void* src = carray_capi_impl.getReadableElementPtr(impl2, fromPos - 1, count);
                memcpy(dest, src, count * impl1->elementSize);
            } else {
                if (fromPos < insertPos) {
                    lua_Integer c = insertPos - fromPos;
                    const void* src = carray_capi_impl.getReadableElementPtr(impl2, fromPos - 1, c);
                    memcpy(dest, src, c * impl1->elementSize);
                    lua_Integer f2 = insertPos + count;
                    dest += c * impl1->elementSize;
                    count -= c;
                    fromPos = f2;
                }
                else {
                    fromPos += count;
                }
                if (count > 0) {
                    const void* src = carray_capi_impl.getReadableElementPtr(impl2, fromPos - 1, count);
                    memcpy(dest, src, count * impl1->elementSize);
                }
            }
        }
    }
    lua_settop(L, 1);
    return 1;
}

/* ============================================================================================ */

static int Carray_setsub(lua_State* L)
{
    int arg = 1;
    carray* impl1 = checkWritableUdata(L, arg++)->impl;
    
    lua_Integer insertPos = luaL_checkinteger(L, arg);
    if (insertPos <= 0 || insertPos > impl1->elementCount + 1) {
        return luaL_argerror(L, arg, "index out of bounds");
    }
    ++arg;
    
    size_t otherCount = 0;
    const char* str   = NULL;
    carray*     impl2 = NULL;

    if ((impl1->elementType == CARRAY_UCHAR || impl1->elementType == CARRAY_SCHAR) && lua_type(L, arg) == LUA_TSTRING)
    {
        str = lua_tolstring(L, arg, &otherCount);
    }
    else {
        impl2 = checkReadableUdata(L, arg)->impl;
        if (impl2->elementType != impl1->elementType) {
            return luaL_argerror(L, arg, lua_pushfstring(L, "carray type mismatch, expected: %s<%s>", CARRAY_CLASS_NAME, typeToString(impl1)));
        }
        otherCount = impl2->elementCount;
    }
    ++arg;
    
    lua_Integer index1 = luaL_checkinteger(L, arg++);
    if (index1 < 0) {
        index1 = otherCount + index1 + 1;
    }
    lua_Integer index2 = luaL_checkinteger(L, arg++);
    if (index2 < 0) {
        index2 = otherCount + index2 + 1;
    }
    if (index1 < 1) {
        index1 = 1;
    }
    if (index2 > otherCount) {
        index2 = otherCount;
    }
    lua_Integer fromPos = index1;
    lua_Integer count   = index2 - index1 + 1;
    if (insertPos + count - 1 > impl1->elementCount) {
        return luaL_argerror(L, 1, "insufficient array length");
    }
    if (count > 0) {
        void* dest = carray_capi_impl.getWritableElementPtr(impl1, insertPos - 1, count);
        if (!dest) {
            return luaL_error(L, "set into carray failed");
        }
        if (str) {
            memcpy(dest, str + fromPos - 1, count);
        } 
        else {
            const void* src = carray_capi_impl.getReadableElementPtr(impl2, fromPos - 1, count);
            if (impl1 != impl2) {
                memcpy(dest, src, count * impl1->elementSize);
            } else {
                memmove(dest, src, count * impl1->elementSize);
            }
        }
    }
    lua_settop(L, 1);
    return 1;
}

/* ============================================================================================ */

static int Carray_remove(lua_State* L)
{
    int arg = 1;
    CarrayUserData* udata = checkWritableUdata(L, arg++);
    carray*         impl  = udata->impl;

    lua_Integer index1 = luaL_checkinteger(L, arg++);
    lua_Integer index2;
    
    bool shrink = false;
    if (!lua_isnoneornil(L, arg)) {
        index2 = luaL_checkinteger(L, arg++);
        if (!lua_isnoneornil(L, arg)) {
            luaL_checktype(L, arg, LUA_TBOOLEAN);
            shrink = lua_toboolean(L, arg++);
        }
    } else {
        index2 = index1;
    }
    lua_Integer totalCount = impl->elementCount;
    if (index1 >= 0) {
        index1 -= 1;
    } else {
        index1 = totalCount + index1;
    }
    if (index2 >= 0) {
        index2 -= 1;
    } else {
        index2 = totalCount + index2;
    }
    if (index1 < 0) {
        index1 = 0;
    }
    if (index1 < totalCount) 
    {
        if (index2 >= totalCount) {
            index2 = totalCount - 1;
        }
        int count = index2 - index1  + 1;
        if (count > 0) {
            size_t elementSize = udata->impl->elementSize;
            char* ptr = udata->impl->buffer + index1 * elementSize;
            carray_capi_impl.removeElements(impl, index1, count, shrink ? -1 : 0);
        }
    }
    lua_settop(L, 1);
    return 1;
}

/* ============================================================================================ */

static int Carray_setlen(lua_State* L)
{
    int arg = 1;
    
    CarrayUserData* udata = checkWritableUdata(L, arg++);
    carray*         impl  = udata->impl;
    size_t       oldCount = impl->elementCount;

    lua_Number newCount = luaL_checkinteger(L, arg++);
    if (newCount < 0)  newCount = 0;
    
    bool shrink = false;
    if (!lua_isnoneornil(L, arg)) {
        luaL_checktype(L, arg, LUA_TBOOLEAN);
        shrink = lua_toboolean(L, arg++);
    }

    if (newCount <= oldCount && !shrink) {
        impl->elementCount = newCount;
    } else {
        if (!carray_capi_impl.resizeCarray(impl, newCount, shrink ? -1 : 0)) {
            return luaL_error(L, "resizing carray failed");
        }
    }
    if (newCount > oldCount) {
        size_t n = newCount - oldCount;
        memset(impl->buffer + impl->elementSize * oldCount, 0, impl->elementSize * n);
    }
    lua_settop(L, 1);
    return 1;
}

/* ============================================================================================ */

static int Carray_reserve(lua_State* L)
{
    int arg = 1;
    
    CarrayUserData* udata = checkWritableUdata(L, arg++);
    carray*         impl  = udata->impl;
    
    if (!lua_isnoneornil(L, arg)) {
        lua_Integer newRes    = luaL_checkinteger(L, arg);
        size_t      currCount = impl->elementCount;
        size_t      currRes   = impl->elementCapacity - currCount;
        if (newRes > 0 && newRes > currRes) {
            carray_capi_impl.resizeCarray(impl, currCount + newRes, 0);
            impl->elementCount = currCount;
        }
        else if (newRes <= 0) {
            carray_capi_impl.resizeCarray(impl, currCount, -1);
        }
        lua_settop(L, 1);
        return 1;
    }
    lua_pushinteger(L, impl->elementCapacity - impl->elementCount);
    return 1;
}

/* ============================================================================================ */

static int Carray_reset(lua_State* L)
{
    CarrayUserData* udata = checkWritableUdata(L, 1);
    bool shrink = false;
    if (!lua_isnoneornil(L, 2)) {
        luaL_checktype(L, 2, LUA_TBOOLEAN);
        shrink = lua_toboolean(L, 2);
    }
    if (!shrink) {
        udata->impl->elementCount = 0;
    } else {
        carray_capi_impl.resizeCarray(udata->impl, 0, -1);
    }
    lua_settop(L, 1);
    return 1;
}

/* ============================================================================================ */

static int Carray_type(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);
    lua_pushstring(L, typeToString(udata->impl));
    return 1;
}

/* ============================================================================================ */

static int Carray_basetype(lua_State* L)
{
    CarrayUserData* udata = luaL_checkudata(L, 1, CARRAY_CLASS_NAME);
    lua_pushstring(L, baseTypeToString(udata->impl));
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
    lua_pushboolean(L, udata->impl && !(udata->impl->attr & CARRAY_READONLY) && !udata->impl->isRef);
    return 1;
}

/* ============================================================================================ */

static int Carray_appendfile(lua_State* L)
{
    int arg = 1;
    CarrayUserData* udata = checkWritableUdata(L, arg);
    carray*         impl  = udata->impl;
    
    luaL_Stream* stream = (luaL_Stream*)luaL_testudata(L, ++arg, LUA_FILEHANDLE);
    FILE* file;
    if (stream) {
        file = stream->f;
        if (!file) {
            return luaL_argerror(L, arg, "invalid file");
        }
    } else {
        const char* name = lua_tostring(L, arg);
        if (!name) {
            luaL_argerror(L, arg, "file handle or name expected");
        }
        file = fopen(name, "rb");
        if (!file) {
            luaL_argerror(L, arg, "cannot open file");
        }
    }
    lua_Integer maxCount = -1;
    if (!lua_isnoneornil(L, ++arg)) {
        maxCount = luaL_checkinteger(L, arg);
        if (maxCount <= 0) {
            if (!stream) fclose(file);
            lua_settop(L, 1);
            return 1;
        }
    }
    size_t elementSize = impl->elementSize;
again:;
    size_t elementCount = impl->elementCount;
    size_t nitems;
    if (maxCount > 0) {
        nitems = maxCount;
    } else {
        nitems = impl->elementCapacity - elementCount;
        if (nitems < 8*1024) {
            nitems = 8*1024;
        }
    }
    char* data = carray_capi_impl.resizeCarray(impl, elementCount + nitems, 0);
    if (!data) {
        if (!stream) fclose(file);
        return luaL_error(L, "resizing carray failed");
    }
    size_t rslt = fread(data + (elementSize * elementCount), elementSize, nitems, file);
    if (rslt > 0) {
        impl->elementCount = elementCount + rslt;
        if (rslt == nitems && maxCount < 0) {
            goto again;
        }
    } else {
        impl->elementCount = elementCount;
    }
    if (rslt < nitems && ferror(file)) {
        int en = errno;
        if (!stream) fclose(file);
        return luaL_error(L, "error reading from file: %s (errno=%d)", strerror(en), en);
    }
    if (!stream) fclose(file);
    lua_settop(L, 1);
    return 1;
}

/* ============================================================================================ */

static int Carray_equals(lua_State* L)
{
    carray* udata1 = checkReadableUdata(L, 1)->impl;
    carray* udata2 = checkReadableUdata(L, 2)->impl;

    size_t dataLength = udata1->elementSize * udata1->elementCount;

    lua_pushboolean(L, udata1->elementType  == udata2->elementType
                   &&  udata1->elementSize  == udata2->elementSize
                   &&  udata1->elementCount == udata2->elementCount
                   && (dataLength == 0 || memcmp(udata1->buffer, udata2->buffer, dataLength) == 0));
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
    { "append",     Carray_append    },
    { "insert",     Carray_insert    },
    { "appendsub",  Carray_appendsub },
    { "insertsub",  Carray_insertsub },
    { "setsub",     Carray_setsub    },
    { "remove",     Carray_remove    },
    { "setlen",     Carray_setlen    },
    { "reserve",    Carray_reserve   },
    { "reset",      Carray_reset     },
    { "tostring",   Carray_getstring },
    { "type",       Carray_type      },
    { "basetype",   Carray_basetype  },
    { "bitwidth",   Carray_bitwidth  },
    { "writable",   Carray_writable  },
    { "resizable",  Carray_resizable },
    { "equals",     Carray_equals    },
    { "appendfile", Carray_appendfile },
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
