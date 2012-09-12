#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mold.h"

#ifdef __cplusplus
extern "C" {
#endif

static void molder_init(molder_t *mold, struct format_api *api, molder_param_t *param)
{
    mold->format_capacity = param->format_capacity;
    assert(mold->format_capacity > 0);
    mold->records = (format_record_t *) malloc(sizeof(format_record_t) * mold->format_capacity);
    mold->api  = api;
    mold->fctx = api->fn_init(mold, (void**)param);
    mold->format_size  = 0;
}

static void append_fmtdata(molder_t *mold, const char *key, uint64_t v, fmtFn f, short klen, short vlen)
{
    assert(mold->format_size < mold->format_capacity);
    mold->records[mold->format_size].i.fn  = f;
    mold->records[mold->format_size].k.key = key;
    mold->records[mold->format_size].v.u   = v;
    mold->records[mold->format_size].klen  = klen;
    mold->records[mold->format_size].vlen  = vlen;
    ++mold->format_size;
}

static void molder_flush(molder_t *mold, void *args)
{
    mold->api->fn_flush(mold, args);
}

static int molder_init_format_key(molder_t *mold, int priority, uint64_t v, short klen)
{
    int emitLog = mold->api->fn_priority(mold, priority);
    if (emitLog == 0)
        return 0;
    mold->records[0].i.fn = mold->api->fn_key;
    mold->records[0].v.u  = v;
    mold->records[0].klen = klen;
    mold->records[0].vlen = 0;
    mold->format_size = 1;
    return emitLog;
}

void molder_record_list(molder_t *mold, void *args, int priority, const char *traceId, int recordSize, format_record_t *r)
{
    format_record_t *end = r + recordSize;
    fmtFn f;

    assert(recordSize < mold->format_capacity);
    if (molder_init_format_key(mold, priority, (uint64_t)traceId, strlen(traceId)) == 0)
        return;
    for (r = r + 1; r != end; ++r) {
        f = ((fmtFn*)mold->api)[r->i.type];
        append_fmtdata(mold, r->k.key, r->v.u, f, r->klen, r->vlen);
    }
    molder_flush(mold, args);
}

void molder_record_ap(molder_t *mold, void *args, int priority, const char *trace_id, va_list ap)
{
    int i;
    format_record_t records[mold->format_capacity];
    for (i = 1; i < mold->format_capacity; ++i) {
        records[i].i.type = va_arg(ap, unsigned);
        if (records[i].i.type == LOG_END)
            break;
        records[i].k.key = va_arg(ap, char *);
        records[i].v.s   = va_arg(ap, char *);
        records[i].klen  = strlen(records[i].k.key);
        records[i].vlen  = (records[i].i.type == LOG_str)? strlen(records[i].v.s):0;
    }
    molder_record_list(mold, args, priority, trace_id, i, records);
}

void molder_record(molder_t *mold, void *args, int priority, const char *trace_id, ...)
{
    va_list ap;
    va_start(ap, trace_id);
    molder_record_ap(mold, args, priority, trace_id, ap);
    va_end(ap);
}

void molder_format_flush(molder_t *mold)
{
    format_record_t *records = mold->records;
    size_t i, size = mold->format_size;
    void (*fn_delim)(molder_t *) = mold->api->fn_delim;
    records->i.fn(mold, records->k.key, records->v.u, records->klen, records->vlen);
    ++records;
    for (i = 1; i < size; ++i, ++records) {
        fn_delim(mold);
        records->i.fn(mold, records->k.key, records->v.u, records->klen, records->vlen);
    }
    mold->format_size = 1;
    ++(mold->records[0].k.seq);
}

molder_t *molder_open(struct format_api *api, molder_param_t *p)
{
    molder_t *m = (molder_t *) malloc(sizeof(*m));
    molder_init(m, api, p);
    return m;
}

void molder_close(molder_t *mold)
{
    mold->api->fn_close(mold);
    free(mold->records);
    free(mold);
}

#ifdef __cplusplus
}
#endif
