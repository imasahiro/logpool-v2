#include <stdlib.h>
#include "../mold.h"
#include "../string_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct filter {
    uintptr_t   priority;
    uintptr_t   emitLog;
    mold_context_t *internal_context;
    struct format_api *api;
} filter_t;

static void molder_filter_key(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    mold->fctx = filter->internal_context;
    filter->api->fn_key(mold, key, v, klen, vlen);
    mold->fctx = (struct mold_context *) filter;
}

static struct mold_context *molder_filter_init(molder_t *mold, void **p)
{
    molder_param_t *args = (molder_param_t *) p;
    filter_t *filter = (filter_t *) malloc(sizeof(*filter));
    filter->api = (struct format_api *) args->unused[0];
    filter->emitLog    = 1;
    filter->priority   = (uintptr_t) args->unused[1];
    filter->internal_context = filter->api->fn_init(mold, (void**)args->unused[2]);
    return (struct mold_context *) filter;
}

static void molder_filter_close(molder_t *mold)
{
    filter_t *filter = (filter_t *) mold->fctx;
    mold->fctx = filter->internal_context;
    filter->api->fn_close(mold);
    free(filter);
}

static void molder_filter_flush(molder_t *mold, void **args)
{
    filter_t *filter = (filter_t *) mold->fctx;
    if (filter->emitLog) {
        molder_format_flush(mold);
        mold->fctx = filter->internal_context;
        filter->api->fn_flush(mold, args);
        molder_context_switch(mold, (mold_context_t *) filter);
    }
}

static int molder_filter_priority(molder_t *mold, int priority)
{
    filter_t *filter = (filter_t *) mold->fctx;
    filter->emitLog  = (priority <= filter->priority);
    return filter->emitLog;
}

static void molder_filter_null(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_null(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_bool(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_bool(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_int(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_int(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_hex(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_hex(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_float(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_float(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_char(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_char(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_string(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_string(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_raw(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_raw(mold, key, v, klen, vlen);
    molder_context_switch(mold, (mold_context_t *) filter);
}

static void molder_filter_delim(molder_t *mold)
{
    filter_t *filter = (filter_t *) mold->fctx;
    molder_context_switch(mold, filter->internal_context);
    filter->api->fn_delim(mold);
    molder_context_switch(mold, (mold_context_t *) filter);
}

struct format_api FILTER_API = {
    molder_filter_key,
    molder_filter_null,
    molder_filter_bool,
    molder_filter_int,
    molder_filter_hex,
    molder_filter_float,
    molder_filter_char,
    molder_filter_string,
    molder_filter_raw,
    molder_filter_delim,
    molder_filter_flush,
    molder_filter_close,
    molder_filter_priority,
    molder_filter_init
};

#ifdef __cplusplus
}
#endif
