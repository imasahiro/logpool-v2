#include <stdint.h>
#include <stdarg.h>

#ifndef MOLDER_H
#define MOLDER_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_EMERG
#define LOG_EMERG   0 /* system is unusable */
#define LOG_ALERT   1 /* action must be taken immediately */
#define LOG_CRIT    2 /* critical conditions */
#define LOG_ERR     3 /* error conditions */
#define LOG_WARNING 4 /* warning conditions */
#define LOG_NOTICE  5 /* normal but significant condition */
#define LOG_INFO    6 /* informational */
#define LOG_DEBUG   7 /* debug-level messages */
#endif

struct mold;
typedef void  (*fmtFn)(struct mold *, const char *k, uint64_t v, short klen, short vlen);

typedef struct format_record {
    union record_info {
        unsigned type;
        fmtFn fn;
    } i;
    union fmt_key {
        uint64_t seq;
        const char *key;
    } k;
    union fmt_val {
        uint64_t u;
        double f;
        char *s;
    } v;
    short klen;
    short vlen;
} format_record_t;

typedef struct mold_context {
    char *buffer;
    char *buffer_base;
    void *unused;
} mold_context_t;

struct format_api {
    fmtFn fn_key;
    fmtFn fn_null;
    fmtFn fn_bool;
    fmtFn fn_int;
    fmtFn fn_hex;
    fmtFn fn_float;
    fmtFn fn_char;
    fmtFn fn_string;
    fmtFn fn_raw;
    void  (*fn_delim)(struct mold *);
    void  (*fn_flush)(struct mold *, void**);
    void  (*fn_close)(struct mold *);
    int   (*fn_priority)(struct mold *, int);
    struct mold_context *(*fn_init) (struct mold *, void**);
};

typedef struct mold {
    struct mold_context *fctx;
    const struct format_api  *api;
    format_record_t *records;
    unsigned format_size;
    unsigned format_capacity;
} molder_t;

typedef struct molder_param_t {
    unsigned buffer_size;
    unsigned format_capacity;
    void *unused[7];
} molder_param_t;

typedef struct molder_multiplexer_param_t {
    struct format_api *api;
    molder_param_t    *param;
} molder_multiplexer_param_t;

enum LOG_TYPE {
    LOG_KEY  =  0,
    LOG_null =  1,
    LOG_bool =  2,
    LOG_uint =  3,
    LOG_int  =  3,
    LOG_hex  =  4,
    LOG_flt  =  5,
    LOG_chr  =  6,
    LOG_str  =  7,
    LOG_raw  =  8,
    LOG_END  = -1,
};

molder_t *molder_open(struct format_api *api, molder_param_t *p);
void molder_close(molder_t *mold);
void molder_format_flush(molder_t *mold);
void molder_record(molder_t *mold, void *args, int priority, const char *trace_id, ...);
void molder_record_ap(molder_t *mold, void *args, int priority, const char *trace_id, va_list ap);
void molder_record_list(molder_t *mold, void *args, int priority,
        const char *traceId, int recordSize, format_record_t *r);

static inline void molder_context_switch(molder_t *mold, mold_context_t *lctx)
{
    mold->fctx = lctx;
}

#ifdef __cplusplus
}
#endif
#endif /* end of include guard */
