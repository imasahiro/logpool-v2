#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mold.h"
#include "string_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void put_uint64(char *p, uint64_t v)
{
    *(uint64_t*)p = v;
}

static inline void put_char2(char *p, int8_t c0, int8_t c1)
{
    uint16_t v = (c1 << 8) | c0;
    *(uint16_t*)p = v;
}

static inline void put_char4(char *p, int8_t c0, int8_t c1, int8_t c2, int8_t c3)
{
    uint32_t v = (c3 << 24) | (c2 << 16) | (c1 << 8) | c0;
    *(uint32_t*)p = v;
}

static inline void buf_put_char(struct mold_context *buf, char c)
{
    buf->buffer[0] = c;
    ++(buf->buffer);
}

static inline void buf_put_char2(struct mold_context *buf, char c0, char c1)
{
    put_char2(buf->buffer, c0, c1);
    buf->buffer += 2;
}

static inline void buf_put_char3(struct mold_context *buf, char c0, char c1, char c2)
{
    put_char4(buf->buffer, c0, c1, c2, 0);
    buf->buffer += 3;
}

static inline void buf_put_uint64(struct mold_context *buf, uint64_t v)
{
    put_uint64(buf->buffer, v);
    buf->buffer += sizeof(uint64_t);
}

static inline char *put_hex(char *const start, uint64_t v)
{
    static const char __digit__[] = "0123456789abcdef";
    register char *p = start;
    int i = ALIGN((v>0?(BITS-CLZ(v)):4), 4) - 4;
    do {
        unsigned char c = 0xf & (v >> i);
        *(p++) = __digit__[c];
    } while ((i -= 4) >= 0);
    return p;
}

static void reverse(char *const start, char *const end)
{
    char *m = start + (end - start) / 2;
    char tmp, *s = start, *e = end - 1;
    while (s < m) {
        tmp  = *s;
        *s++ = *e;
        *e-- = tmp;
    }
}

static inline char *put_d(char *p, uint64_t v)
{
    char *base = p;
    do {
        *p++ = '0' + ((uint8_t)(v % 10));
    } while ((v /= 10) != 0);

    reverse(base, p);
    return p;
}

static inline char *put_i(char *p, int64_t value)
{
    if(value < 0) {
        p[0] = '-'; p++;
        value = -value;
    }
    return put_d(p, (uint64_t)value);
}

static inline char *put_f(char *p, double f)
{
    int32_t s1, s2, s3, u, r;
    intptr_t value = (intptr_t) (f*1000);
    if(value < 0) {
        p[0] = '-'; p++;
        value = -value;
    }
    u = value / 1000;
    r = value % 1000;
    p = put_d(p, u);
    s3 = r % 100;
    u  = r / 100;
    /* s2 = s3 / 10; s1 = s3 % 10 */
    s2 = (s3 * 0xcd) >> 11;
    s1 = (s3) - 10*s2;
    put_char4(p, '.', ('0' + u), ('0' + s2), ('0' + s1));
    return p + 4;
}

static inline char *put_string(char *p, const char *s, short size)
{
    const char *e = s + size;
    while (s < e) {
        *p++ = *s++;
    }
    return p;
}

static inline void buf_put_string(struct mold_context *buf, const char *s, short size)
{
    buf->buffer = put_string(buf->buffer, s, size);
}


static inline double u2f(uint64_t u)
{
    union {uint64_t u; double f;} v;
    v.u = u;
    return v.f;
}

struct mold_context *molder_string_init(molder_t *mold, void **p)
{
    molder_param_t *args = (molder_param_t *) p;
    struct mold_context *buf;
    uintptr_t size = (uintptr_t) args->buffer_size;
    buf = (struct mold_context *) malloc(sizeof(*buf));
    buf->buffer_base = (char *) malloc(size);
    buf->buffer  = buf->buffer_base;
    return (void *) buf;
}

void molder_string_close(molder_t *mold)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    free(buf);
    mold->fctx = NULL;
}

void molder_string_null(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    buf_put_string(buf, key, klen);
    buf_put_string(buf, "null", 5);
}

void molder_string_bool(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    const char *s = (v != 0)?"true":"false";
    size_t len = (v != 0)? 4 : 5;
    buf_put_string(buf, key, klen);
    buf_put_string(buf, s, len);
}

void molder_string_int(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    intptr_t i = (intptr_t) v;
    buf_put_string(buf, key, klen);
    buf->buffer = put_i(buf->buffer, i);
}

void molder_string_hex(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    buf_put_string(buf, key, klen);
    buf_put_char2(buf, '0', 'x');
    buf->buffer = put_hex(buf->buffer, v);
}

void molder_string_float(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    double f = u2f(v);
    buf_put_string(buf, key, klen);
    buf->buffer = put_f(buf->buffer, f);
}

void molder_string_char(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    long c = (long) v;
    buf_put_string(buf, key, klen);
    buf_put_char(buf, (char)c);
}

void molder_string_string(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    char *s = (char *) v;
    buf_put_string(buf, key, klen);
    buf_put_char(buf, '\'');
    buf_put_string(buf, s, vlen);
    buf_put_char(buf, '\'');
}

void molder_string_raw(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    char *s = (char *) v;
    buf_put_string(buf, key, klen);
    buf_put_string(buf, s, vlen);
}

void molder_string_delim(molder_t *mold)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    buf_put_char(buf, ',');
}

void molder_string_flush(molder_t *mold)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    buf_put_char(buf, 0);
}

void molder_string_flush__(molder_t *mold, void **args)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    molder_format_flush(mold);
    molder_string_flush(mold);
    assert(buf->buffer[-1] == '\0');
    put_char2(buf->buffer-1, '\n', '\0');
    fwrite(buf->buffer_base, buf->buffer - buf->buffer_base, 1, stderr);
    molder_string_reset(mold);
}

void molder_key_hex(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    buf_put_string(buf, "TraceID", sizeof("TraceID")-1);
    buf_put_char2(buf, '0' ,'x');
    buf->buffer = put_hex(buf->buffer, v);
}

void molder_key_string(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    buf_put_string(buf, "TraceID", sizeof("TraceID")-1);
    char *s = (char *) v;
    buf_put_string(buf, s, klen);
}

struct format_api STRING_API = {
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
    molder_string_close,
    molder_default_priority,
    molder_string_init
};

struct format_api STRING_API_HEX = {
    molder_key_hex,
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
    molder_string_close,
    molder_default_priority,
    molder_string_init
};

static void molder_binary_int_float(molder_t *mold, const char *key, uint64_t v, short klen, short vlen)
{
    struct mold_context *buf = (struct mold_context *) mold->fctx;
    buf_put_string(buf, key, klen);
    buf_put_uint64(buf, v);
}

struct format_api BINARY_API = {
    molder_key_string,
    molder_string_null,
    molder_string_bool,
    molder_binary_int_float,
    molder_binary_int_float,
    molder_binary_int_float,
    molder_string_char,
    molder_string_string,
    molder_string_raw,
    molder_string_delim,
    molder_string_flush__,
    molder_string_close,
    molder_default_priority,
    molder_string_init
};

#ifdef __cplusplus
}
#endif
