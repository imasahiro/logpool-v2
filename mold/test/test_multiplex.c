#include "../mold.h"
#include "../string_builder.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static struct mold_context *molder_string_init_no_malloc(molder_t *mold, void **p)
{
    molder_param_t *args = (molder_param_t *) p;
    struct mold_context *buf;
    uintptr_t size = (uintptr_t) args->buffer_size;
    buf = (struct mold_context *) malloc(sizeof(*buf) + size - 1);
    buf->buffer  = (char*) args->unused[0];
    buf->buffer_base = buf->buffer;
    return (struct mold_context *) buf;
}

static void molder_string_close_no_malloc(molder_t *mold)
{
    mold->fctx = NULL;
}

struct format_api STRING_API_TEST = {
    molder_key_string,
    molder_string_null,
    molder_string_bool,
    molder_string_int,
    molder_string_hex,
    molder_string_float,
    molder_string_char,
    molder_string_string,
    molder_string_raw,
    molder_string_delim,
    molder_string_flush__,
    molder_string_close_no_malloc,
    molder_default_priority,
    molder_string_init_no_malloc
};

int main(int argc, char const* argv[])
{
    char *buffer0 = (char *) malloc(1024);
    char *buffer1 = (char *) malloc(1024);
    molder_param_t param0 = {1024, 8, {buffer0}};
    molder_param_t param1 = {1024, 8, {buffer1}};
    molder_multiplexer_param_t mparams[] = {
        {&STRING_API_TEST, &param0},
        {&STRING_API_TEST, &param1}
    };
    molder_param_t param  = {1024, 8, {(void*)2L, (void*)&mparams}};
    extern struct format_api MULTIPLEXER_API;
    molder_t *mold = molder_open(&MULTIPLEXER_API, &param);
    molder_record(mold, NULL, 1, "TEST", LOG_str, "a", "b", LOG_int, "c", 0xaL, LOG_END);
    molder_close(mold);
    assert(strncmp(buffer0, "TraceIDTEST,a'b',c10", 20) == 0);
    assert(strncmp(buffer1, "TraceIDTEST,a'b',c10", 20) == 0);
    free(buffer0);
    free(buffer1);
    return 0;
}
