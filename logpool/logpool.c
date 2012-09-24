#include "logpool.h"
#include "mold/mold.h"
#include "stream/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

struct logpool {
    molder_t *mold;
    stream_t *io;
};

logpool_t *logpool_open(JSON param)
{
    logpool_t *lp = malloc(sizeof(*lp));
    extern struct format_api FILE_API;
    molder_param_t molder_param = {1024, 8, {"test.log"}};
    lp->mold = molder_open(&FILE_API, &molder_param);
    return lp;
}

void logpool_close(logpool_t *lp)
{
    molder_close(lp->mold);
    free(lp);
}

void logpool_record(logpool_t *lp, void *args, int priority, char *trace_id, ...)
{
    va_list ap;
    va_start(ap, trace_id);
    molder_record_ap(lp->mold, args, priority, trace_id, ap);
    va_end(ap);
}

//typedef struct format_record {
//    union record_info {
//        unsigned type;
//        fmtFn fn;
//    } i;
//    union fmt_key {
//        uint64_t seq;
//        const char *key;
//    } k;
//    union fmt_val {
//        uint64_t u;
//        double f;
//        char *s;
//    } v;
//    short klen;
//    short vlen;
//} format_record_t;

void logpool_record_list(logpool_t *lp, void *args, int priority, char *trace_id, int recordSize, struct logdata *logs)
{
    molder_record_list(lp->mold, args, priority, trace_id, recordSize, /*TODO*/(format_record_t *) logs);
}

#ifdef __cplusplus
} /* extern "C" */
#endif
