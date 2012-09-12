#include "../mold.h"
#include "../string_builder.h"
#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct file_context {
    char *buffer;
    char *buffer_base;
    FILE *fp;
} file_context_t;

struct mold_context *molder_FILE_init(molder_t *mold, void **p)
{
    molder_param_t *args = (molder_param_t *) p;
    char *filename = (char *) args->unused[0];
    file_context_t *ctx = (file_context_t*) molder_string_init(mold, p);
    ctx->fp = fopen(filename, "w");
    return (struct mold_context *) ctx;
}

static void molder_FILE_close(molder_t *mold)
{
    file_context_t *ctx = (file_context_t *) mold->fctx;
    fclose(ctx->fp);
    molder_string_close(mold);
}

static void molder_FILE_flush(molder_t *mold, void **args)
{
    file_context_t *ctx = (file_context_t *) mold->fctx;
    molder_format_flush(mold);
    molder_string_flush(mold);
    assert(ctx->buffer[-1] == '\0');
    ctx->buffer[-1] = '\n';
    ctx->buffer[ 0] = '\0';
    fwrite(ctx->buffer_base, ctx->buffer - ctx->buffer_base, 1, ctx->fp);
    molder_string_reset(mold);
}

struct format_api FILE_API = {
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
    molder_FILE_flush,
    molder_FILE_close,
    molder_default_priority,
    molder_FILE_init
};

#ifdef __cplusplus
}
#endif
