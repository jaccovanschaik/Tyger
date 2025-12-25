/* libtyger.c: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016-2025 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-31
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>
#include <iconv.h>
#include <errno.h>

#include "libtyger.h"
#include "utf8.h"

static char *indent_string = NULL;
static int   indent_length = 0;

#define SWAP(from, to) do { char temp = to; to = from; from = temp; } while (0)

/*
 * Reserve room for another <num_bytes> bytes in the buffer pointed to by <*buf>, updating it and
 * <*size> as necessary.
 */
static void reserve(size_t num_bytes, char **buf, size_t *size, size_t pos)
{
    size_t new_size = *size;

    while (pos + num_bytes > new_size) {
        if (new_size == 0) {
            new_size = 1024;
        }
        else {
            new_size *= 2;
        }
    }

    if (new_size != *size) {
        *size = new_size;

        *buf = realloc(*buf, *size);
    }
}

static Buffer *check(size_t num_bytes, size_t size, size_t pos)
{
    if (size - pos < num_bytes) {
        return bufCreate("Overflow getting %lu bytes at position %lu of %lu",
                num_bytes, size, pos);
    }
    else {
        return NULL;
    }
}

/* =============================== Aliases ===============================
 *
 * Aliases for functions from libjvs.
 */
void (*destroy_astring)(astring *) = asDestroy;
void (*destroy_wstring)(wstring *) = wsDestroy;
void (*clear_astring)(astring *) = asClear;
void (*clear_wstring)(wstring *) = wsClear;

/* =============================== Indent handling ===============================
 *
 * Set the indentation string.
 */
void setIndent(const char *str)
{
    if (indent_string != NULL) {
        free(indent_string);
    }

    indent_string = strdup(str);
    indent_length = strlen(indent_string);
}

/*
 * Return an indentation string for level <level>.
 */
const char *indent(int level)
{
    static char *buf = NULL;
    static int   available = -1;

    if (indent_string == NULL) {
        setIndent("    ");
    }

    int required = level * indent_length;

    if (required > available) {
        int i;

        buf = realloc(buf, required + 1);

        available = required;

        for (i = 0; i < available; i++) {
            buf[i] = indent_string[i % indent_length];
        }

        buf[i] = '\0';
    }

    return buf + available - level * indent_length;
}

/* =============================== "Clear" functions ===============================
 *
 * Clear the contents of <data>.
 */
void clear_uint8(uint8_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_uint16(uint16_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_uint32(uint32_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_uint64(uint64_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_int8(int8_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_int16(int16_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_int32(int32_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_int64(int64_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void clear_bool(bool *data)
{
    *data = false;
}

/*
 * Clear the contents of <data>.
 */
void clear_float32(float *data)
{
    *data = 0.0;
}

/*
 * Clear the contents of <data>.
 */
void clear_float64(double *data)
{
    *data = 0.0;
}

/* =============================== "Destroy" functions ===============================
 *
 * Destroy the contents of <data>.
 */
void destroy_uint8(uint8_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_uint16(uint16_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_uint32(uint32_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_uint64(uint64_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_int8(int8_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_int16(int16_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_int32(int32_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_int64(int64_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_bool(bool *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_float32(float *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void destroy_float64(double *data)
{
    free(data);
}

/* =============================== "PackSize" functions ===============================
 *
 * Return the number of bytes required to pack a bool.
 */
size_t size_bool(void)
{
    return 1;
}

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t size_uint8(void)
{
    return sizeof(uint8_t);
}

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t size_uint16(void)
{
    return sizeof(uint16_t);
}

/*
 * Return the number of bytes required to pack a uint32_t.
 */
size_t size_uint32(void)
{
    return sizeof(uint32_t);
}

/*
 * Return the number of bytes required to pack a uint64_t.
 */
size_t size_uint64(void)
{
    return sizeof(uint64_t);
}

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t size_int8(void)
{
    return sizeof(int8_t);
}

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t size_int16(void)
{
    return sizeof(int16_t);
}

/*
 * Return the number of bytes required to pack a int32_t.
 */
size_t size_int32(void)
{
    return sizeof(int32_t);
}

/*
 * Return the number of bytes required to pack a int64_t.
 */
size_t size_int64(void)
{
    return sizeof(int64_t);
}

/*
 * Return the number of bytes required to pack a float32.
 */
size_t size_float32(void)
{
    return sizeof(float);
}

/*
 * Return the number of bytes required to pack a float64.
 */
size_t size_float64(void)
{
    return sizeof(double);
}

/*
 * Return the number of bytes required to pack the char *pointed to by <data>.
 */
size_t size_astring(const astring *as)
{
    return size_uint32() + asLen(as);
}

/*
 * Return the number of bytes required to pack the wchar_t *pointed to by <data>.
 */
size_t size_wstring(const wstring *ws)
{
    size_t pack_size = size_uint32();

    if (wsGet(ws) == NULL || wsLen(ws) == 0) return pack_size;

    uint32_t utf8_size;

    wchar_to_utf8(wsGet(ws), wsLen(ws), &utf8_size);

    return pack_size + utf8_size;
}

/* =============================== "Pack" functions ===============================
 *
 * Pack the least-significant <num_bytes> of <data> into <buf>, updating
 * <size> and <pos>.
 */
Buffer *pack_uint(uint64_t data, size_t num_bytes, char **buf, size_t *size, size_t *pos)
{
    Buffer *err = NULL;

    reserve(num_bytes, buf, size, *pos);

    for (int i = num_bytes - 1; i >= 0 && err == NULL; i--) {
        uint8_t byte = (data >> (8 * i)) & 0xFF;

        (*buf)[(*pos)++] = byte;
    }

    return err;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint8(uint8_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 1, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint16(uint16_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 2, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint32(uint32_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 4, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint64(uint64_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 8, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int8(int8_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 1, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int16(int16_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 2, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int32(int32_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 4, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int64(int64_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint(data, 8, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_float32(const float data, char **buf, size_t *size, size_t *pos)
{
    union {
        float f;
        char c[sizeof(float)];
    } u;

    u.f = data;

    Buffer *err = NULL;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (int i = sizeof(float) - 1; i >= 0 && err == NULL; --i) {
        err = pack_uint(u.c[i], 1, buf, size, pos);
    }
#else
    err = pack_uint(&u.f, sizeof(float), buf, size, pos);
#endif

    return err;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_float64(const double data, char **buf, size_t *size, size_t *pos)
{
    union {
        double f;
        char c[sizeof(double)];
    } u;

    u.f = data;

    Buffer *err = NULL;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (int i = sizeof(double) - 1; i >= 0 && err == NULL; --i) {
        err = pack_uint(u.c[i], 1, buf, size, pos);
    }
#else
    err = pack_uint(&u.f, sizeof(double), buf, size, pos);
#endif

    return err;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_bool(const bool data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint8(data ? 1 : 0, buf, size, pos);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_astring(const astring *as, char **buf, size_t *size, size_t *pos)
{
    const char *str = asGet(as);
    uint32_t    len = asLen(as);

    Buffer *err = NULL;

    if (str == NULL || len == 0) {
        err = pack_uint32(0, buf, size, pos);
    }
    else if ((err = pack_uint32(len, buf, size, pos)) == NULL) {
        reserve(len, buf, size, *pos);

        memcpy((*buf) + *pos, str, len);

        (*pos) += len;
    }

    return err;
}

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
Buffer *pack_wstring(const wstring *ws, char **buf, size_t *size, size_t *pos)
{
    const wchar_t *wstr = wsGet(ws);
    uint32_t       wlen = wsLen(ws);

    uint32_t utf8_size;
    const uint8_t *utf8_text;

    Buffer *err = NULL;

    if (wstr == NULL || wlen == 0) {
        err = pack_uint32(0, buf, size, pos);
    }
    else if ((utf8_text = wchar_to_utf8(wstr, wlen, &utf8_size)) == NULL) {
        err = bufCreate("Could not convert string \"%S\" to UTF-8", wstr);
    }
    else if ((err = pack_uint32(utf8_size, buf, size, pos)) == NULL) {
        reserve(utf8_size, buf, size, *pos);

        memcpy((*buf) + *pos, utf8_text, utf8_size);

        (*pos) += utf8_size;
    }

    return err;
}

/* =============================== "Unpack" functions ===============================
 *
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
Buffer *unpack_uint(size_t num_bytes, const char *buf, size_t size, size_t *pos, uint64_t *data)
{
    Buffer *err = NULL;

    if (size - *pos < num_bytes) {
        err = bufCreate("Buffer overflow while unpacking %lu bytes at position %lu of %lu",
                num_bytes, *pos, size);
    }
    else {
        *data = 0;

        for (int i = 0; i < num_bytes; i++) {
            *data <<= 8;

            *data |= buf[(*pos)++];
        }
    }

    return err;
}

/*
 * Unpack a uint8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint8(const char *buf, size_t size, size_t *pos, uint8_t *data)
{
    Buffer *err = NULL;

    if ((err = check(sizeof(*data), size, *pos)) == NULL) {
        *data = 0;

        for (int i = 0; i < sizeof(*data); i++) {
            *data = (*data << 8) | buf[(*pos)++];
        }
    }

    return err;
}

/*
 * Unpack a uint16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint16(const char *buf, size_t size, size_t *pos, uint16_t *data)
{
    Buffer *err = NULL;

    if ((err = check(sizeof(*data), size, *pos)) == NULL) {
        *data = 0;

        for (int i = 0; i < sizeof(*data); i++) {
            *data = (*data << 8) | buf[(*pos)++];
        }
    }

    return err;
}

/*
 * Unpack a uint32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint32(const char *buf, size_t size, size_t *pos, uint32_t *data)
{
    Buffer *err = NULL;

    if ((err = check(sizeof(*data), size, *pos)) == NULL) {
        *data = 0;

        for (int i = 0; i < sizeof(*data); i++) {
            *data = (*data << 8) | buf[(*pos)++];
        }
    }

    return err;
}

/*
 * Unpack a uint64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint64(const char *buf, size_t size, size_t *pos, uint64_t *data)
{
    Buffer *err = NULL;

    if ((err = check(sizeof(*data), size, *pos)) == NULL) {
        *data = 0;

        for (int i = 0; i < sizeof(*data); i++) {
            *data = (*data << 8) | buf[(*pos)++];
        }
    }

    return err;
}

/*
 * Unpack an int8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int8(const char *buf, size_t size, size_t *pos, int8_t *data)
{
    return unpack_uint8(buf, size, pos, (uint8_t *) data);
}

/*
 * Unpack an int16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int16(const char *buf, size_t size, size_t *pos, int16_t *data)
{
    return unpack_uint16(buf, size, pos, (uint16_t *) data);
}

/*
 * Unpack an int32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int32(const char *buf, size_t size, size_t *pos, int32_t *data)
{
    return unpack_uint32(buf, size, pos, (uint32_t *) data);
}

/*
 * Unpack an int64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int64(const char *buf, size_t size, size_t *pos, int64_t *data)
{
    return unpack_uint64(buf, size, pos, (uint64_t *) data);
}

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
Buffer *unpack_bool(const char *buf, size_t size, size_t *pos, bool *data)
{
    Buffer *err = NULL;

    if ((err = check(1, size, *pos)) == NULL) {
        *data = (buf[(*pos)++] == 1);
    }

    return err;
}

/*
 * Unpack a float (aka. float32) from <buf> (which has size <size>) and put it at <data>. Return the
 * new <pos>.
 */
Buffer *unpack_float32(const char *buf, size_t size, size_t *pos, float *data)
{
    Buffer *err = NULL;

    if ((err = check(sizeof(*data), size, *pos)) == NULL) {
        union {
            typeof(*data) f;
            char c[sizeof(*data)];
        } u;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        for (int i = sizeof(*data) - 1; i >= 0 && err == NULL; --i) {
            u.c[i] = buf[(*pos)++];
        }
#else
        memcpy(u.c, buf + *pos, sizeof(*data));

        (*pos) += sizeof(*data);
#endif

        *data = u.f;
    }

    return err;
}

/*
 * Unpack a double (aka. float64) from <buf> (which has size <size>) and put it at <data>. Return
 * the new <pos>.
 */
Buffer *unpack_float64(const char *buf, size_t size, size_t *pos, double *data)
{
    Buffer *err = NULL;

    if ((err = check(sizeof(*data), size, *pos)) == NULL) {
        union {
            typeof(*data) f;
            char c[sizeof(*data)];
        } u;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        for (int i = sizeof(*data) - 1; i >= 0 && err == NULL; --i) {
            u.c[i] = buf[(*pos)++];
        }
#else
        memcpy(u.c, buf + *pos, sizeof(*data));

        (*pos) += sizeof(*data);
#endif

        *data = u.f;
    }

    return err;
}

/*
 * Unpack a char from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
Buffer *unpack_astring(const char *buf, size_t size, size_t *pos, astring *data)
{
    Buffer *err = NULL;

    uint32_t len;

    if ((err = unpack_uint32(buf, size, pos, &len)) == NULL &&
        (err = check(len, size, *pos)) == NULL)
    {
        asSet(data, buf + *pos, len);

        (*pos) += len;
    }

    return err;
}

/*
 * Unpack a UTF-8 encoded Unicode string from <buf> (which has size <size>), write it to a newly
 * allocated wide-character string whose starting address is written to <wchar_str>, and return the
 * number of bytes consumed from <buf>.
 */
Buffer *unpack_wstring(const char *buf, size_t size, size_t *pos, wstring *data)
{
    Buffer *err = NULL;

    uint32_t utf8_len;

    if ((err = unpack_uint32(buf, size, pos, &utf8_len)) == NULL &&
        (err = check(utf8_len, size, *pos)) == NULL)
    {
        size_t wchar_len;
        const wchar_t *wchar_str = utf8_to_wchar((uint8_t *) buf + *pos, utf8_len, &wchar_len);

        wsSet(data, wchar_str, wchar_len);

        (*pos) += utf8_len;
    }

    return err;
}

/* =============================== "Copy" functions ===============================
 *
 * Copy string <src> to <dst>.
 */
void copy_astring(astring *dst, const astring *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    asClear(dst);

    asAdd(dst, asGet(src), asLen(src));
}

/*
 * Copy string <src> to <dst>.
 */
void copy_wstring(wstring *dst, const wstring *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    wsClear(dst);

    wsAdd(dst, wsGet(src), wsLen(src));
}

/* =============================== "Print" functions ===============================
 *
 * Print an ASCII representation of <data> to <fp>.
 */
void print_uint8(FILE *fp, uint8_t data, int indent)
{
    fprintf(fp, "%" PRIu8, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int8(FILE *fp, int8_t data, int indent)
{
    fprintf(fp, "%" PRId8, data);
}

void print_uint16(FILE *fp, uint16_t data, int indent)
{
    fprintf(fp, "%" PRIu16, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int16(FILE *fp, int16_t data, int indent)
{
    fprintf(fp, "%" PRId16, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_uint32(FILE *fp, uint32_t data, int indent)
{
    fprintf(fp, "%" PRIu32, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int32(FILE *fp, int32_t data, int indent)
{
    fprintf(fp, "%" PRId32, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_uint64(FILE *fp, uint64_t data, int indent)
{
    fprintf(fp, "%" PRIu64, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int64(FILE *fp, int64_t data, int indent)
{
    fprintf(fp, "%" PRId64, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_bool(FILE *fp, bool data, int indent)
{
    fprintf(fp, "%s", data ? "true" : "false");
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_float32(FILE *fp, float data, int indent)
{
    fprintf(fp, "%g", data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_float64(FILE *fp, double data, int indent)
{
    fprintf(fp, "%g", data);
}

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void print_astring(FILE *fp, const astring *as, int indent)
{
    fprintf(fp, "\"%s\"", as->data);
}

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void print_wstring(FILE *fp, const wstring *ws, int indent)
{
    fprintf(fp, "\"%ls\"", wsGet(ws));
}

/* =============================== "Dup" functions ===============================
 *
 * Duplicate astring <str>.
 */
astring *dup_astring(astring *str)
{
    return asCreate("%s", asGet(str));
}

/*
 * Duplicate wstring <str>.
 */
wstring *dup_wstring(wstring *str)
{
    return wsCreate(L"%s", wsGet(str));
}

#ifdef TEST
int main(int argc, char *argv[])
{
    char  *buf  = NULL;
    size_t size = 0;
    size_t wpos = 0;
    size_t rpos = 0;

    float f32;
    double f64;

    uint8_t  u8;
    int8_t   i8;
    uint16_t u16;
    int16_t  i16;
    uint32_t u32;
    int32_t  i32;
    uint64_t u64;
    int64_t  i64;

    f32 = 1.0;

    assert(pack_float32(f32, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x3F\x80\x00\x00", 4) == 0);
    assert(wpos == 4);

    assert(unpack_float32(buf, size, &rpos, &f32) == NULL);
    assert(f32 == 1.0);
    assert(rpos == 4);

    f64 = 2.0;

    assert(pack_float64(f64, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x40\x00\x00\x00\x00\x00\x00\x00", 8) == 0);
    assert(wpos == 12);

    assert(unpack_float64(buf, size, &rpos, &f64) == NULL);
    assert(f64 == 2.0);

    u8 = 8;

    assert(pack_uint8(u8, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x08", 1) == 0);
    assert(wpos == 13);

    assert(unpack_uint8(buf, size, &rpos, &u8) == NULL);
    assert(u8 == 8);
    assert(rpos == wpos);

    i8 = 8;

    assert(pack_int8(i8, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x08", 1) == 0);
    assert(wpos == 14);

    assert(unpack_int8(buf, size, &rpos, &i8) == NULL);
    assert(i8 == 8);
    assert(rpos == wpos);

    u8 = -8;

    assert(pack_uint8(u8, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xF8", 1) == 0);
    assert(wpos == 15);

    assert(unpack_uint8(buf, size, &rpos, &u8) == NULL);
    assert(u8 == (256 - 8));
    assert(rpos == wpos);

    i8 = -8;

    assert(pack_int8(i8, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xF8", 1) == 0);
    assert(wpos == 16);

    assert(unpack_int8(buf, size, &rpos, &i8) == NULL);
    assert(i8 == -8);
    assert(rpos == wpos);

    u16 = 16;

    assert(pack_uint16(u16, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x10", 2) == 0);
    assert(wpos == 18);

    assert(unpack_uint16(buf, size, &rpos, &u16) == NULL);
    assert(u16 == 16);
    assert(rpos == wpos);

    i16 = 16;

    assert(pack_int16(i16, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x10", 2) == 0);
    assert(wpos == 20);

    assert(unpack_int16(buf, size, &rpos, &i16) == NULL);
    assert(i16 == 16);
    assert(rpos == wpos);

    u16 = -16;

    assert(pack_uint16(u16, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xFF\xF0", 2) == 0);
    assert(wpos == 22);

    assert(unpack_uint16(buf, size, &rpos, &u16) == NULL);
    assert(u16 == (65536 - 16));
    assert(rpos == wpos);

    i16 = -16;

    assert(pack_int16(i16, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xFF\xF0", 2) == 0);
    assert(wpos == 24);

    assert(unpack_int16(buf, size, &rpos, &i16) == NULL);
    assert(i16 == -16);
    assert(rpos == wpos);

    u32 = 32;

    assert(pack_uint32(u32, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x00\x00\x20", 4) == 0);
    assert(wpos == 28);

    assert(unpack_uint32(buf, size, &rpos, &u32) == NULL);
    assert(u32 == 32);
    assert(rpos == wpos);

    i32 = 32;

    assert(pack_int32(i32, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x00\x00\x20", 4) == 0);
    assert(wpos == 32);

    assert(unpack_int32(buf, size, &rpos, &i32) == NULL);
    assert(i32 == 32);
    assert(rpos == wpos);

    u32 = -32;

    assert(pack_uint32(u32, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xFF\xFF\xFF\xE0", 4) == 0);
    assert(wpos == 36);

    assert(unpack_uint32(buf, size, &rpos, &u32) == NULL);
    assert(u32 == (0x100000000L - 32));
    assert(rpos == wpos);

    i32 = -32;

    assert(pack_int32(i32, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xFF\xFF\xFF\xE0", 4) == 0);
    assert(wpos == 40);

    assert(unpack_int32(buf, size, &rpos, &i32) == NULL);
    assert(i32 == -32);
    assert(rpos == wpos);

    u64 = 64;

    assert(pack_uint64(u64, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);
    assert(wpos == 48);

    assert(unpack_uint64(buf, size, &rpos, &u64) == NULL);
    assert(u64 == 64);
    assert(rpos == wpos);

    i64 = 64;

    assert(pack_int64(i64, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);
    assert(wpos == 56);

    assert(unpack_int64(buf, size, &rpos, &i64) == NULL);
    assert(i64 == 64);
    assert(rpos == wpos);

    u64 = -64;

    assert(pack_uint64(u64, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);
    assert(wpos == 64);

    assert(unpack_uint64(buf, size, &rpos, &u64) == NULL);
    assert(u64 == (0xFFFFFFFFFFFFFFFF - 63));
    assert(rpos == wpos);

    i64 = -64;

    assert(pack_int64(i64, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);
    assert(wpos == 72);

    assert(unpack_int64(buf, size, &rpos, &i64) == NULL);
    assert(i64 == -64);
    assert(rpos == wpos);

    astring *as_w = asCreate("Hoi");

    assert(pack_astring(as_w, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x00\x00\x03" "Hoi", 7) == 0);
    assert(wpos == 79);

    astring *as_r = asCreate(NULL);

    assert(unpack_astring(buf, size, &rpos, as_r) == NULL);
    assert(strcmp(asGet(as_r), "Hoi") == 0);
    assert(rpos == wpos);

    wstring *ws_w = wsCreate(L"αß¢");

    assert(pack_wstring(ws_w, &buf, &size, &wpos) == NULL);
    assert(memcmp(buf + rpos, "\x00\x00\x00\x06\xCE\xB1\xC3\x9F\xC2\xA2", 10) == 0);
    assert(wpos == 89);

    wstring *ws_r = wsCreate(NULL);

    assert(unpack_wstring(buf, size, &rpos, ws_r) == NULL);
    assert(wcscmp(wsGet(ws_r), L"αß¢") == 0);
    assert(rpos == wpos);

    return 0;
}
#endif
