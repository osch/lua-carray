#ifndef CARRAY_CAPI_IMPL_H
#define CARRAY_CAPI_IMPL_H

#include "util.h"
#include "carray_capi.h"
#include "async_util.h"

/* ============================================================================================ */

struct carray
{
    AtomicCounter usageCounter;
    carray_type   type;
    carray_attr   attr;
    size_t        elementSize;
    bool          isInteger;
    bool          isUnsigned;
    bool          isRef;

    void (*releaseCallback)(void* dataRef, size_t elementCount);

    char*         buffer;
    size_t        elementCount;
    size_t        elementCapacity;
};

/* ============================================================================================ */

extern const carray_capi carray_capi_impl;

#endif /* CARRAY_CAPI_IMPL_H */
