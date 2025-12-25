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
Buffer *pack_uint(unsigned int data, size_t num_bytes, char **buf, size_t *size, size_t *pos)
{
    for (int i = num_bytes - 1; i >= 0; i--) {
        pack_uint8((data >> (8 * i)) & 0xFF, buf, size, pos);
    }
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint8(uint8_t data, char **buf, size_t *size, size_t *pos)
{
    size_t pack_size = size_uint8();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint16(uint16_t data, char **buf, size_t *size, size_t *pos)
{
    size_t pack_size = size_uint16();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint32(uint32_t data, char **buf, size_t *size, size_t *pos)
{
    size_t pack_size = size_uint32();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint64(uint64_t data, char **buf, size_t *size, size_t *pos)
{
    size_t pack_size = size_uint64();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int8(int8_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint8(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int16(int16_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint16(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int32(int32_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint32(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int64(int64_t data, char **buf, size_t *size, size_t *pos)
{
    return pack_uint64(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_float32(const float data, char **buf, size_t *size, size_t *pos)
{
    union {
        float f;
        char c[4];
    } u;

    u.f = data;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    bufAddC(buf, u.c[3]);
    bufAddC(buf, u.c[2]);
    bufAddC(buf, u.c[1]);
    bufAddC(buf, u.c[0]);
#else
    bufAdd(buf, &u.f);
#endif

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_float64(const double data, char **buf, size_t *size, size_t *pos)
{
    union {
        double f;
        char c[8];
    } u;

    u.f = data;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    bufAddC(buf, u.c[7]);
    bufAddC(buf, u.c[6]);
    bufAddC(buf, u.c[5]);
    bufAddC(buf, u.c[4]);
    bufAddC(buf, u.c[3]);
    bufAddC(buf, u.c[2]);
    bufAddC(buf, u.c[1]);
    bufAddC(buf, u.c[0]);
#else
    bufAdd(buf, &u.f, 8);
#endif

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_bool(const bool data, char **buf, size_t *size, size_t *pos)
{
    bufAddC(buf, data ? 1 : 0);

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_astring(const astring *as, char **buf, size_t *size, size_t *pos)
{
    pack_uint32(asLen(as), buf);

    if (as->data != NULL && asLen(as) > 0) bufAdd(buf, as->data, asLen(as));

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
Buffer *pack_wstring(const wstring *ws, char **buf, size_t *size, size_t *pos)
{
    if (wsGet(ws) == NULL || wsLen(ws) == 0) {
        pack_uint32(0, buf);
    }
    else {
        uint32_t utf8_size;
        const uint8_t *utf8_text = wchar_to_utf8(wsGet(ws), wsLen(ws), &utf8_size);

        pack_uint32(utf8_size, buf);

        bufAdd(buf, utf8_text, utf8_size);
    }

    return buf;
}

/* =============================== "Unpack" functions ===============================
 *
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
size_t unpack_uint(size_t num_bytes, const char *buf, size_t size, size_t *pos, unsigned int *data)
{
    assert(bufLen(buf) - pos >= num_bytes);

    *data = 0;

    for (int i = 0; i < num_bytes; i++, pos++) {
        *data <<= 8;

        *data |= bufGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_uint8(const char *buf, size_t size, size_t *pos, uint8_t *data)
{
    size_t pack_size = size_uint8();

    assert(bufLen(buf) - pos >= pack_size);

    *data = bufGetC(buf, pos++);

    return pos;
}

/*
 * Unpack a uint16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_uint16(const char *buf, size_t size, size_t *pos, uint16_t *data)
{
    size_t pack_size = size_uint16();

    assert(bufLen(buf) - pos >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_uint32(const char *buf, size_t size, size_t *pos, uint32_t *data)
{
    size_t pack_size = size_uint32();

    assert(bufLen(buf) - pos >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_uint64(const char *buf, size_t size, size_t *pos, uint64_t *data)
{
    size_t pack_size = size_uint64();

    assert(bufLen(buf) - pos >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack an int8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_int8(const char *buf, size_t size, size_t *pos, int8_t *data)
{
    return unpack_uint8(buf, pos, (uint8_t *) data);
}

/*
 * Unpack an int16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_int16(const char *buf, size_t size, size_t *pos, int16_t *data)
{
    return unpack_uint16(buf, pos, (uint16_t *) data);
}

/*
 * Unpack an int32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_int32(const char *buf, size_t size, size_t *pos, int32_t *data)
{
    return unpack_uint32(buf, pos, (uint32_t *) data);
}

/*
 * Unpack an int64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t unpack_int64(const char *buf, size_t size, size_t *pos, int64_t *data)
{
    return unpack_uint64(buf, pos, (uint64_t *) data);
}

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t unpack_bool(const char *buf, size_t size, size_t *pos, bool *data)
{
    size_t pack_size = size_bool();

    assert(bufLen(buf) - pos >= pack_size);

    *data = (bufGetC(buf, pos) == 1);

    pos += pack_size;

    return pos;
}

/*
 * Unpack a float (aka. float32) from <buf> (which has size <size>) and put it at <data>. Return the
 * new <pos>.
 */
size_t unpack_float32(const char *buf, size_t size, size_t *pos, float *data)
{
    size_t req = size_float32();

    union {
        float f;
        char c[4];
    } u;

    assert(bufLen(buf) - pos >= req);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    u.c[3] = bufGetC(buf, pos + 0);
    u.c[2] = bufGetC(buf, pos + 1);
    u.c[1] = bufGetC(buf, pos + 2);
    u.c[0] = bufGetC(buf, pos + 3);
#else
    memcpy(u.c, bufGet(buf) + pos, 4);
#endif

    *data = u.f;

    return req;
}

/*
 * Unpack a double (aka. float64) from <buf> (which has size <size>) and put it at <data>. Return
 * the new <pos>.
 */
size_t unpack_float64(const char *buf, size_t size, size_t *pos, double *data)
{
    size_t req = size_float64();

    union {
        double f;
        char c[8];
    } u;

    assert(bufLen(buf) - pos >= req);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    u.c[7] = bufGetC(buf, pos + 0);
    u.c[6] = bufGetC(buf, pos + 1);
    u.c[5] = bufGetC(buf, pos + 2);
    u.c[4] = bufGetC(buf, pos + 3);
    u.c[3] = bufGetC(buf, pos + 4);
    u.c[2] = bufGetC(buf, pos + 5);
    u.c[1] = bufGetC(buf, pos + 6);
    u.c[0] = bufGetC(buf, pos + 7);
#else
    memcpy(u.c, bufGet(buf) + pos, 8);
#endif

    *data = u.f;

    return req;
}

/*
 * Unpack a char from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t unpack_astring(const char *buf, size_t size, size_t *pos, astring *data)
{
    size_t size_of_string_length = size_uint32();

    assert(bufLen(buf) - pos >= size_of_string_length);

    uint32_t string_length;

    pos = unpack_uint32(buf, pos, &string_length);

    assert(bufLen(buf) - pos >= string_length);

    asSet(data, (char *) bufGet(buf) + pos, string_length);

    pos += string_length;

    return pos;
}

/*
 * Unpack a UTF-8 encoded Unicode string from <buf> (which has size <size>), write it to a newly
 * allocated wide-character string whose starting address is written to <wchar_str>, and return the
 * number of bytes consumed from <buf>.
 */
size_t unpack_wstring(const char *buf, size_t size, size_t *pos, wstring *data)
{
    size_t size_of_utf8_len = size_uint32();

    assert(bufLen(buf) - pos >= size_of_utf8_len);

    uint32_t utf8_len;

    pos = unpack_uint32(buf, pos, &utf8_len);

    assert(bufLen(buf) - pos >= utf8_len);

    size_t wchar_len;
    const wchar_t *wchar_tmp = utf8_to_wchar((uint8_t *) bufGet(buf) + pos, utf8_len, &wchar_len);

    if (wchar_tmp) {
        wsSet(data, wchar_tmp, wchar_len);
    }

    pos += utf8_len;

    return pos;
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
    Buffer  buf  = { };
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

    assert(pack_float32(f32, &buf) == &buf);
    assert(size_float32() == 4);
    assert(memcmp(bufGet(&buf), "\x3F\x80\x00\x00", 4) == 0);

    assert(unpack_float32(&buf, 0, &f32) == 4);
    assert(f32 == 1.0);

    bufClear(&buf);

    f64 = 2.0;

    assert(pack_float64(f64, &buf) == &buf);
    assert(size_float64() == 8);
    assert(memcmp(bufGet(&buf), "\x40\x00\x00\x00\x00\x00\x00\x00", 8) == 0);

    assert(unpack_float64(&buf, 0, &f64) == 8);
    assert(f64 == 2.0);

    bufClear(&buf);

    u8 = 8;
    assert(pack_uint8(u8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x08", 1) == 0);

    assert(unpack_uint8(&buf, 0, &u8) == 1);
    assert(u8 == 8);

    bufClear(&buf);

    i8 = 8;
    assert(pack_int8(i8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x08", 1) == 0);

    assert(unpack_int8(&buf, 0, &i8) == 1);
    assert(i8 == 8);

    bufClear(&buf);

    u8 = -8;
    assert(pack_uint8(u8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xF8", 1) == 0);

    assert(unpack_uint8(&buf, 0, &u8) == 1);
    assert(u8 == (256 - 8));

    bufClear(&buf);

    i8 = -8;
    assert(pack_int8(i8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xF8", 1) == 0);

    assert(unpack_int8(&buf, 0, &i8) == 1);
    assert(i8 == -8);

    bufClear(&buf);

    u16 = 16;
    assert(pack_uint16(u16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x10", 2) == 0);

    assert(unpack_uint16(&buf, 0, &u16) == 2);
    assert(u16 == 16);

    bufClear(&buf);

    i16 = 16;
    assert(pack_int16(i16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x10", 2) == 0);

    assert(unpack_int16(&buf, 0, &i16) == 2);
    assert(i16 == 16);

    bufClear(&buf);

    u16 = -16;
    assert(pack_uint16(u16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xF0", 2) == 0);

    assert(unpack_uint16(&buf, 0, &u16) == 2);
    assert(u16 == (65536 - 16));

    bufClear(&buf);

    i16 = -16;
    assert(pack_int16(i16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xF0", 2) == 0);

    assert(unpack_int16(&buf, 0, &i16) == 2);
    assert(i16 == -16);

    bufClear(&buf);

    u32 = 32;
    assert(pack_uint32(u32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x20", 4) == 0);

    assert(unpack_uint32(&buf, 0, &u32) == 4);
    assert(u32 == 32);

    bufClear(&buf);

    i32 = 32;
    assert(pack_int32(i32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x20", 4) == 0);

    assert(unpack_int32(&buf, 0, &i32) == 4);
    assert(i32 == 32);

    bufClear(&buf);

    u32 = -32;
    assert(pack_uint32(u32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xE0", 4) == 0);

    assert(unpack_uint32(&buf, 0, &u32) == 4);
    assert(u32 == (0x100000000L - 32));

    bufClear(&buf);

    i32 = -32;
    assert(pack_int32(i32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xE0", 4) == 0);

    assert(unpack_int32(&buf, 0, &i32) == 4);
    assert(i32 == -32);

    bufClear(&buf);

    u64 = 64;
    assert(pack_uint64(u64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);

    assert(unpack_uint64(&buf, 0, &u64) == 8);
    assert(u64 == 64);

    bufClear(&buf);

    i64 = 64;
    assert(pack_int64(i64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);

    assert(unpack_int64(&buf, 0, &i64) == 8);
    assert(i64 == 64);

    bufClear(&buf);

    u64 = -64;
    assert(pack_uint64(u64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);

    assert(unpack_uint64(&buf, 0, &u64) == 8);
    assert(u64 == (0xFFFFFFFFFFFFFFFF - 63));

    bufClear(&buf);

    i64 = -64;
    assert(pack_int64(i64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);

    assert(unpack_int64(&buf, 0, &i64) == 8);
    assert(i64 == -64);

    bufClear(&buf);

    u32 = 256;

    assert(pack_uint32(u32, &buf) == &buf);

    assert(bufLen(&buf) == 4);
    assert(memcmp(bufGet(&buf), "\x00\x00\x01\x00", 4) == 0);

    astring *as = asCreate("Hoi");

    assert(pack_astring(as, &buf) == &buf);

    assert(bufLen(&buf) == 11);
    assert(memcmp(bufGet(&buf), "\x00\x00\x01\x00\x00\x00\x00\x03" "Hoi", 11) == 0);

    wstring *ws = wsCreate(L"αß¢");

    assert(pack_wstring(ws, &buf) == &buf);

    assert(bufLen(&buf) == 21);
    assert(memcmp(bufGet(&buf),
                "\x00\x00\x01\x00"
                "\x00\x00\x00\x03" "Hoi"
                "\x00\x00\x00\x06\xCE\xB1\xC3\x9F\xC2\xA2", 21) == 0);

    return 0;
}
#endif
