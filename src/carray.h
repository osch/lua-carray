#ifndef CARRAY_H
#define CARRAY_H

#include "util.h"

/* ============================================================================================ */

extern const char* const CARRAY_CLASS_NAME;

/* ============================================================================================ */

typedef struct CarrayUserData CarrayUserData;
struct carray;

struct CarrayUserData
{
    const char*    className;
    struct carray* impl;
};

/* ============================================================================================ */

int carray_push_meta(lua_State* L);

int carray_init_module(lua_State* L, int module);

/* ============================================================================================ */

#endif /* CARRAY_CAPI_IMPL_H */
