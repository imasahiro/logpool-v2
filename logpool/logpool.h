#include <stdio.h>
#include "kjson/kjson.h"

#ifndef LOGPOOL_H
#define LOGPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

struct logpool;
typedef struct logpool logpool_t;

struct Log;
typedef struct logpool_param {
    unsigned memeory_size;
    unsigned file_size;
    long filter_context;
    int  (*filter)(struct Log *chunkedLog, unsigned nextOffset);
} logpool_param_t;

logpool_t *logpool_open(JSON param);
void logpool_close(logpool_t *logpool);

struct logdata {
    long  type;
    short klen;
    short vlen;
    char *key;
    char *val;
};

void logpool_record(logpool_t *logpool, void *args, int priority, char *trace_id, ...);
void logpool_record_list(logpool_t *logpool, void *args, int priority, char *trace_id, int recordSize, struct logdata *logs);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* end of include guard */
