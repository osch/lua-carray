#include <limits.h>

#include "util.h"
#include "async_util.h"

#define CARRAY_CAPI_IMPLEMENT_GET_CAPI 1

#include "carray.h"
#include "carray_capi_impl.h"

/* ============================================================================================ */

static carray* internalNewCarray(lua_State* L, carray_type type, carray_attr attr, size_t elementCount, void** data,
                                 void* dataRef, void (*releaseCallback)(void* dataRef, size_t elementCount))
{
    size_t elementSize = 0;
    bool   isUnsigned  = false;
    bool   isInteger   = false;
    bool   isFloat     = false;
    
    switch (type) {
        case CARRAY_SCHAR:   isInteger = true; elementSize = sizeof(signed char); break;
        case CARRAY_UCHAR:   isInteger = true; elementSize = sizeof(unsigned char); isUnsigned = true; break;
        
        case CARRAY_SHORT:   isInteger = true; elementSize = sizeof(short); break;
        case CARRAY_USHORT:  isInteger = true; elementSize = sizeof(ushort); isUnsigned = true; break;
        
        case CARRAY_INT:     isInteger = true; elementSize = sizeof(int); break;
        case CARRAY_UINT:    isInteger = true; elementSize = sizeof(unsigned int); isUnsigned = true; break;
        
        case CARRAY_LONG:    isInteger = true; elementSize = sizeof(long); break;
        case CARRAY_ULONG:   isInteger = true; elementSize = sizeof(unsigned long); isUnsigned = true; break;
        
        case CARRAY_FLOAT:   isFloat = true; elementSize = sizeof(float); break;
        case CARRAY_DOUBLE:  isFloat = true; elementSize = sizeof(double); break;

#if CARRAY_CAPI_HAVE_LONG_LONG
        case CARRAY_LLONG:   isInteger = true; elementSize = sizeof(long long); break;
        case CARRAY_ULLONG:  isInteger = true; elementSize = sizeof(unsigned long long); isUnsigned = true; break;
#endif        
    }
    if (elementSize == 0) {
        return NULL;
    }
    CarrayUserData* udata = lua_newuserdata(L, sizeof(CarrayUserData));
    memset(udata, 0, sizeof(CarrayUserData));               /* -> udata */
    carray_push_meta(L);                                    /* -> udata, meta */
    lua_setmetatable(L, -2);                                /* -> udata */
    
    udata->className = CARRAY_CLASS_NAME;
    udata->impl = malloc(sizeof(carray));
    if (!udata->impl) {
        luaL_error(L, "cannot allocate carray");
        return NULL;
    }
    memset(udata->impl, 0, sizeof(carray));
    udata->impl->usageCounter = 1;
    udata->impl->type         = type;
    udata->impl->attr         = attr;
    udata->impl->elementSize  = elementSize;
    udata->impl->isInteger    = isInteger;
    udata->impl->isUnsigned   = isUnsigned;
    udata->impl->isRef        = dataRef != NULL;
    
    if (dataRef == NULL) 
    {
        if (elementCount > 0) {
            udata->impl->buffer = malloc(elementSize * elementCount);
            if (!udata->impl->buffer) {
                luaL_error(L, "cannot allocate carray");
                return NULL;
            }
            if (data) {
                *data = udata->impl->buffer;
            } else {
                memset(udata->impl->buffer, 0, elementSize * elementCount);
            }
        }
    } else {
        udata->impl->buffer          = dataRef;
        udata->impl->releaseCallback = releaseCallback;
    }
    udata->impl->elementCount    = elementCount;
    udata->impl->elementCapacity = elementCount;
}

/* ============================================================================================ */

static carray* newCarray(lua_State* L, carray_type type, carray_attr attr, size_t elementCount, void** data)
{
    return internalNewCarray(L, type, attr, elementCount, data, NULL, NULL);
}

/* ============================================================================================ */

static carray* newCarrayRef(lua_State* L, carray_type type, carray_attr attr, void* dataRef, size_t elementCount,
                            void (*releaseCallback)(void* dataRef, size_t elementCount))
{
    return internalNewCarray(L, type, attr, elementCount, NULL, (void*)dataRef, releaseCallback);
}

/* ============================================================================================ */

static const carray* toReadableCarray(lua_State* L, int index, carray_info* info)
{
    CarrayUserData* udata = lua_touserdata(L, index);
    if (udata) {
        size_t len = lua_rawlen(L, index);
        if (   len == sizeof(CarrayUserData) 
            && udata->className == CARRAY_CLASS_NAME
            && udata->impl)
        {
            if (info) {
                memset(info, 0, sizeof(carray_info));
                if (udata->impl) {
                    info->type         = udata->impl->type;
                    info->attr         = udata->impl->attr;
                    info->elementSize  = udata->impl->elementSize;
                    info->elementCount = udata->impl->elementCount;
                }
            }
            return udata->impl;
        }
    }
    return NULL;
}

/* ============================================================================================ */

static carray* toWritableCarray(lua_State* L, int index, carray_info* info)
{
    CarrayUserData* udata = lua_touserdata(L, index);
    if (udata) {
        size_t len = lua_rawlen(L, index);
        if (   len == sizeof(CarrayUserData) 
            && udata->className == CARRAY_CLASS_NAME
            && udata->impl
            && (!(udata->impl->attr & CARRAY_READONLY)))
        {
            if (info) {
                memset(info, 0, sizeof(carray_info));
                if (udata->impl) {
                    info->type         = udata->impl->type;
                    info->attr         = udata->impl->attr;
                    info->elementSize  = udata->impl->elementSize;
                    info->elementCount = udata->impl->elementCount;
                }
            }
            return udata->impl;
        }
    }
    return NULL;
}

/* ============================================================================================ */

static void retainCarray(const carray* array)
{
    carray* impl = (carray*)array;
    if (impl) {
        atomic_inc(&impl->usageCounter);
    }
}

/* ============================================================================================ */

static void releaseCarray(const carray* array)
{
    carray* impl = (carray*)array;

    if (impl && atomic_dec(&impl->usageCounter) == 0) {
        if (impl->buffer) {
            if (impl->isRef) {
                if (impl->releaseCallback) {
                    impl->releaseCallback(impl->buffer, impl->elementCount);
                }
            } else {
                free(impl->buffer);
            }
            impl->buffer = NULL;
            impl->elementCount    = 0;
            impl->elementCapacity = 0;
        }
        free(impl);
    }
}

/* ============================================================================================ */

static void* getWritableElementPtr(carray* impl, size_t offset, size_t count)
{
    if (impl && !(impl->attr & CARRAY_READONLY)
             && 0 <= offset && offset < impl->elementCount
             && 0 <  count  && offset + count <= impl->elementCount)
    {
        return impl->buffer;
    }
    return NULL;
}

/* ============================================================================================ */

const void* getReadableElementPtr(const carray* array, size_t offset, size_t count)
{
    carray* impl = (carray*)array;

    if (impl && 0 <= offset && offset < impl->elementCount
             && 0 <  count  && offset + count <= impl->elementCount)
    {
        return impl->buffer;
    }
    return NULL;
}

/* ============================================================================================ */

const carray_capi carray_capi_impl =
{
    CARRAY_CAPI_VERSION_MAJOR,
    CARRAY_CAPI_VERSION_MINOR,
    CARRAY_CAPI_VERSION_PATCH,
    
    NULL, /* next_capi */

    newCarray,
    newCarrayRef,
    toReadableCarray,
    toWritableCarray,
    retainCarray,
    releaseCarray,
    getWritableElementPtr,
    getReadableElementPtr
};

/* ============================================================================================ */
