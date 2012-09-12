#ifndef MOLD_STRING_H_
#define MOLD_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BITS (sizeof(void*) * 8)
#define CLZ(n) __builtin_clzl(n)
#define ALIGN(x,n)  (((x)+((n)-1))&(~((n)-1)))
struct mold;
struct mold_context *molder_string_init(struct mold *mold, void **p);
void molder_string_close(struct mold *mold);
void molder_string_null(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_bool(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_int(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_hex(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_float(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_char(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_string(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_raw(struct mold *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_string_delim(struct mold *mold);
void molder_string_flush(struct mold *mold);
void molder_string_flush__(molder_t *mold, void **args);
void molder_key_hex(molder_t *mold, const char *key, uint64_t v, short klen, short vlen);
void molder_key_string(molder_t *mold, const char *key, uint64_t v, short klen, short vlen);

static inline void molder_string_reset(struct mold *mold)
{
    struct mold_context *fctx = (struct mold_context *) mold->fctx;
    fctx->buffer = fctx->buffer_base;
}

static inline int molder_default_priority(molder_t *mold, int priority)
{
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* end of include guard */
