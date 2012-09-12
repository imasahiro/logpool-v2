#include <stdlib.h>
#include "../mold.h"
#include "../string_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct plugin_entry {
    struct format_api *api;
    mold_context_t *lctx;
} entry_t;

typedef struct multiplexer {
    struct plugin_entry *entries;
    uintptr_t entry_size;
} multiplexer_t;

static void molder_multiplexer_key(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    multiplexer_t *multiplexer = (multiplexer_t *) mold->fctx;
    int i;
    for (i = 0; i < multiplexer->entry_size; i++) {
        entry_t *e = multiplexer->entries + i;
        molder_context_switch(mold, e->lctx);
        e->api->fn_key(mold, key, v, klen, vlen);
    }
    molder_context_switch(mold, (mold_context_t *) multiplexer);
}

static struct mold_context *molder_multiplexer_init(molder_t *mold, void **p)
{
    molder_param_t *args = (molder_param_t *) p;
    multiplexer_t *multiplexer = (multiplexer_t *) malloc(sizeof(*multiplexer));
    multiplexer->entry_size = (uintptr_t) args->unused[0];
    multiplexer->entries    = (entry_t *) malloc(sizeof(entry_t) * multiplexer->entry_size);

    molder_multiplexer_param_t *params = (molder_multiplexer_param_t *) args->unused[1];
    int i;
    for (i = 0; i < multiplexer->entry_size; i++) {
        entry_t *e = multiplexer->entries + i;
        e->api  = params[i].api;
        e->lctx = params[i].api->fn_init(mold, (void**)params[i].param);
    }
    return (struct mold_context *) multiplexer;
}

static void molder_multiplexer_close(molder_t *mold)
{
    multiplexer_t *multiplexer = (multiplexer_t *) mold->fctx;
    int i;
    for (i = 0; i < multiplexer->entry_size; i++) {
        entry_t *e = multiplexer->entries + i;
        molder_context_switch(mold, e->lctx);
        e->api->fn_close(mold);
    }
    free(multiplexer->entries);
    free(multiplexer);
}

static void molder_multiplexer_flush(molder_t *mold, void **args)
{
    molder_format_flush(mold);
}

static int molder_multiplexer_priority(molder_t *mold, int priority)
{
    multiplexer_t *multiplexer = (multiplexer_t *) mold->fctx;
    int i, res = 0;
    for (i = 0; i < multiplexer->entry_size; i++) {
        entry_t *e = multiplexer->entries + i;
        molder_context_switch(mold, e->lctx);
        res |= e->api->fn_priority(mold, priority);
    }
    molder_context_switch(mold, (mold_context_t *) multiplexer);
    return res;
}

#define multiplexer_exec(Fn) do {\
    int i;\
    multiplexer_t *multiplexer = (multiplexer_t *) mold->fctx;\
    for (i = 0; i < multiplexer->entry_size; i++) {\
        entry_t *e = multiplexer->entries + i;\
        molder_context_switch(mold, e->lctx);\
        e->api->Fn(mold, key, v, klen, vlen);\
    }\
    molder_context_switch(mold, (mold_context_t *) multiplexer);\
} while (0)

#define DEFINE_OPERATOR_LIST(OP)\
    OP(null)\
    OP(bool)\
    OP(int)\
    OP(hex)\
    OP(float)\
    OP(char)\
    OP(string)\
    OP(raw)

#define DEFINE_MULTIPLEXER(NAME)\
static void molder_multiplexer_##NAME(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)\
{  multiplexer_exec(fn_##NAME); }
DEFINE_OPERATOR_LIST(DEFINE_MULTIPLEXER);
#undef DEFINE_MULTIPLEXER

static void molder_multiplexer_delim(molder_t *mold)
{
    multiplexer_t *multiplexer = (multiplexer_t *) mold->fctx;
    int i;
    for (i = 0; i < multiplexer->entry_size; i++) {
        entry_t *e = multiplexer->entries + i;
        molder_context_switch(mold, e->lctx);
        e->api->fn_delim(mold);
    }
    molder_context_switch(mold, (mold_context_t *) multiplexer);
}

struct format_api MULTIPLEXER_API = {
    molder_multiplexer_key,
    molder_multiplexer_null,
    molder_multiplexer_bool,
    molder_multiplexer_int,
    molder_multiplexer_hex,
    molder_multiplexer_float,
    molder_multiplexer_char,
    molder_multiplexer_string,
    molder_multiplexer_raw,
    molder_multiplexer_delim,
    molder_multiplexer_flush,
    molder_multiplexer_close,
    molder_multiplexer_priority,
    molder_multiplexer_init
};

#ifdef __cplusplus
}
#endif
