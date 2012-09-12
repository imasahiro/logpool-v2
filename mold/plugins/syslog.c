#include <syslog.h>
#include "../mold.h"
#include "../string_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

static void molder_syslog_flush(molder_t *mold, void **args)
{
    mold_context_t *ctx = mold->fctx;
    molder_format_flush(mold);
    molder_string_flush(mold);
#if 0
    syslog(LOG_NOTICE, ctx->buffer_base);
#else
    syslog(LOG_NOTICE, "%s", ctx->buffer_base);
#endif
    molder_string_reset(mold);
}

struct format_api SYSLOG_API = {
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
    molder_syslog_flush,
    molder_string_close,
    molder_default_priority,
    molder_string_init
};

#ifdef __cplusplus
}
#endif
