/* libtyger.c: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
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
 * Aliases for functions from libjvs.
 */
void (*astringClear)(astring *) = asClear;
void (*wstringClear)(wstring *) = wsClear;

/*
 * Clear the contents of <data>.
 */
void uint8Clear(uint8_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void uint16Clear(uint16_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void uint32Clear(uint32_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void uint64Clear(uint64_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void int8Clear(int8_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void int16Clear(int16_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void int32Clear(int32_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void int64Clear(int64_t *data)
{
    *data = 0;
}

/*
 * Clear the contents of <data>.
 */
void boolClear(bool *data)
{
    *data = false;
}

/*
 * Clear the contents of <data>.
 */
void float32Clear(float *data)
{
    *data = 0.0;
}

/*
 * Clear the contents of <data>.
 */
void float64Clear(double *data)
{
    *data = 0.0;
}

/* =============================== "Destroy" functions ===============================
 *
 * Aliases for functions from libjvs.
 */
void (*astringDestroy)(astring *) = asDestroy;
void (*wstringDestroy)(wstring *) = wsDestroy;

/*
 * Destroy the contents of <data>.
 */
void uint8Destroy(uint8_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void uint16Destroy(uint16_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void uint32Destroy(uint32_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void uint64Destroy(uint64_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void int8Destroy(int8_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void int16Destroy(int16_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void int32Destroy(int32_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void int64Destroy(int64_t *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void boolDestroy(bool *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void float32Destroy(float *data)
{
    free(data);
}

/*
 * Destroy the contents of <data>.
 */
void float64Destroy(double *data)
{
    free(data);
}

/* =============================== "PackSize" functions ===============================
 *
 * Return the number of bytes required to pack a bool.
 */
size_t boolPackSize(void)
{
    return 1;
}

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t uint8PackSize(void)
{
    return sizeof(uint8_t);
}

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t uint16PackSize(void)
{
    return sizeof(uint16_t);
}

/*
 * Return the number of bytes required to pack a uint32_t.
 */
size_t uint32PackSize(void)
{
    return sizeof(uint32_t);
}

/*
 * Return the number of bytes required to pack a uint64_t.
 */
size_t uint64PackSize(void)
{
    return sizeof(uint64_t);
}

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t int8PackSize(void)
{
    return sizeof(int8_t);
}

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t int16PackSize(void)
{
    return sizeof(int16_t);
}

/*
 * Return the number of bytes required to pack a int32_t.
 */
size_t int32PackSize(void)
{
    return sizeof(int32_t);
}

/*
 * Return the number of bytes required to pack a int64_t.
 */
size_t int64PackSize(void)
{
    return sizeof(int64_t);
}

/*
 * Return the number of bytes required to pack a float32.
 */
size_t float32PackSize(void)
{
    return sizeof(float);
}

/*
 * Return the number of bytes required to pack a float64.
 */
size_t float64PackSize(void)
{
    return sizeof(double);
}

/*
 * Return the number of bytes required to pack the char *pointed to by <data>.
 */
size_t astringPackSize(const astring *as)
{
    return uint32PackSize() + asLen(as);
}

/*
 * Return the number of bytes required to pack the wchar_t *pointed to by <data>.
 */
size_t wstringPackSize(const wstring *ws)
{
    size_t pack_size = uint32PackSize();

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
Buffer *uintPack(unsigned int data, size_t num_bytes, Buffer *buf)
{
    for (int i = num_bytes - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint8Pack(uint8_t data, Buffer *buf)
{
    size_t pack_size = uint8PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint16Pack(uint16_t data, Buffer *buf)
{
    size_t pack_size = uint16PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint32Pack(uint32_t data, Buffer *buf)
{
    size_t pack_size = uint32PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint64Pack(uint64_t data, Buffer *buf)
{
    size_t pack_size = uint64PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int8Pack(int8_t data, Buffer *buf)
{
    return uint8Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int16Pack(int16_t data, Buffer *buf)
{
    return uint16Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int32Pack(int32_t data, Buffer *buf)
{
    return uint32Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int64Pack(int64_t data, Buffer *buf)
{
    return uint64Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *float32Pack(const float data, Buffer *buf)
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
Buffer *float64Pack(const double data, Buffer *buf)
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
Buffer *boolPack(const bool data, Buffer *buf)
{
    bufAddC(buf, data ? 1 : 0);

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *astringPack(const astring *as, Buffer *buf)
{
    uint32Pack(asLen(as), buf);

    if (as->data != NULL && asLen(as) > 0) bufAdd(buf, as->data, asLen(as));

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
Buffer *wstringPack(const wstring *ws, Buffer *buf)
{
    if (wsGet(ws) == NULL || wsLen(ws) == 0) {
        uint32Pack(0, buf);
    }
    else {
        uint32_t utf8_size;
        const uint8_t *utf8_text = wchar_to_utf8(wsGet(ws), wsLen(ws), &utf8_size);

        uint32Pack(utf8_size, buf);

        bufAdd(buf, utf8_text, utf8_size);
    }

    return buf;
}

/* =============================== "Unpack" functions ===============================
 *
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
size_t uintUnpack(size_t num_bytes, const Buffer *buf, size_t pos, unsigned int *data)
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
size_t uint8Unpack(const Buffer *buf, size_t pos, uint8_t *data)
{
    size_t pack_size = uint8PackSize();

    assert(bufLen(buf) - pos >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint16Unpack(const Buffer *buf, size_t pos, uint16_t *data)
{
    size_t pack_size = uint16PackSize();

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
size_t uint32Unpack(const Buffer *buf, size_t pos, uint32_t *data)
{
    size_t pack_size = uint32PackSize();

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
size_t uint64Unpack(const Buffer *buf, size_t pos, uint64_t *data)
{
    size_t pack_size = uint64PackSize();

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
size_t int8Unpack(const Buffer *buf, size_t pos, int8_t *data)
{
    return uint8Unpack(buf, pos, (uint8_t *) data);
}

/*
 * Unpack an int16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int16Unpack(const Buffer *buf, size_t pos, int16_t *data)
{
    return uint16Unpack(buf, pos, (uint16_t *) data);
}

/*
 * Unpack an int32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int32Unpack(const Buffer *buf, size_t pos, int32_t *data)
{
    return uint32Unpack(buf, pos, (uint32_t *) data);
}

/*
 * Unpack an int64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int64Unpack(const Buffer *buf, size_t pos, int64_t *data)
{
    return uint64Unpack(buf, pos, (uint64_t *) data);
}

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t boolUnpack(const Buffer *buf, size_t pos, bool *data)
{
    size_t pack_size = boolPackSize();

    assert(bufLen(buf) - pos >= pack_size);

    *data = (bufGetC(buf, pos) == 1);

    pos += pack_size;

    return pos;
}

/*
 * Unpack a float (aka. float32) from <buf> (which has size <size>) and put it at <data>. Return the
 * new <pos>.
 */
size_t float32Unpack(const Buffer *buf, size_t pos, float *data)
{
    size_t req = float32PackSize();

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
size_t float64Unpack(const Buffer *buf, size_t pos, double *data)
{
    size_t req = float64PackSize();

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
size_t astringUnpack(const Buffer *buf, size_t pos, astring *data)
{
    size_t packsize_of_string_length = uint32PackSize();

    assert(bufLen(buf) - pos >= packsize_of_string_length);

    uint32_t string_length;

    pos = uint32Unpack(buf, pos, &string_length);

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
size_t wstringUnpack(const Buffer *buf, size_t pos, wstring *data)
{
    size_t packsize_of_utf8_len = uint32PackSize();

    assert(bufLen(buf) - pos >= packsize_of_utf8_len);

    uint32_t utf8_len;

    pos = uint32Unpack(buf, pos, &utf8_len);

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
void astringCopy(char **dst, const char *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    if (*dst != NULL) {
        free(*dst);
        *dst = NULL;
    }

    *dst = strdup(src);
}

/*
 * Copy string <src> to <dst>.
 */
void wstringCopy(wchar_t **dst, const wchar_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    if (*dst != NULL) {
        free(*dst);
        *dst = NULL;
    }

    *dst = wcsdup(src);
}

/* =============================== "Print" functions ===============================
 *
 * Print an ASCII representation of <data> to <fp>.
 */
void uint8Print(FILE *fp, uint8_t data, int indent)
{
    fprintf(fp, "%" PRIu8, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int8Print(FILE *fp, int8_t data, int indent)
{
    fprintf(fp, "%" PRId8, data);
}

void uint16Print(FILE *fp, uint16_t data, int indent)
{
    fprintf(fp, "%" PRIu16, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int16Print(FILE *fp, int16_t data, int indent)
{
    fprintf(fp, "%" PRId16, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint32Print(FILE *fp, uint32_t data, int indent)
{
    fprintf(fp, "%" PRIu32, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int32Print(FILE *fp, int32_t data, int indent)
{
    fprintf(fp, "%" PRId32, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint64Print(FILE *fp, uint64_t data, int indent)
{
    fprintf(fp, "%" PRIu64, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int64Print(FILE *fp, int64_t data, int indent)
{
    fprintf(fp, "%" PRId64, data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void boolPrint(FILE *fp, bool data, int indent)
{
    fprintf(fp, "%s", data ? "true" : "false");
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float32Print(FILE *fp, float data, int indent)
{
    fprintf(fp, "%g", data);
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float64Print(FILE *fp, double data, int indent)
{
    fprintf(fp, "%g", data);
}

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void astringPrint(FILE *fp, const astring *as, int indent)
{
    fprintf(fp, "\"%s\"", as->data);
}

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void wstringPrint(FILE *fp, const wstring *ws, int indent)
{
    fprintf(fp, "\"%ls\"", wsGet(ws));
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

    assert(float32Pack(f32, &buf) == &buf);
    assert(float32PackSize() == 4);
    assert(memcmp(bufGet(&buf), "\x3F\x80\x00\x00", 4) == 0);

    assert(float32Unpack(&buf, 0, &f32) == 4);
    assert(f32 == 1.0);

    bufClear(&buf);

    f64 = 2.0;

    assert(float64Pack(f64, &buf) == &buf);
    assert(float64PackSize() == 8);
    assert(memcmp(bufGet(&buf), "\x40\x00\x00\x00\x00\x00\x00\x00", 8) == 0);

    assert(float64Unpack(&buf, 0, &f64) == 8);
    assert(f64 == 2.0);

    bufClear(&buf);

    u8 = 8;
    assert(uint8Pack(u8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x08", 1) == 0);

    assert(uint8Unpack(&buf, 0, &u8) == 1);
    assert(u8 == 8);

    bufClear(&buf);

    i8 = 8;
    assert(int8Pack(i8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x08", 1) == 0);

    assert(int8Unpack(&buf, 0, &i8) == 1);
    assert(i8 == 8);

    bufClear(&buf);

    u8 = -8;
    assert(uint8Pack(u8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xF8", 1) == 0);

    assert(uint8Unpack(&buf, 0, &u8) == 1);
    assert(u8 == (256 - 8));

    bufClear(&buf);

    i8 = -8;
    assert(int8Pack(i8, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xF8", 1) == 0);

    assert(int8Unpack(&buf, 0, &i8) == 1);
    assert(i8 == -8);

    bufClear(&buf);

    u16 = 16;
    assert(uint16Pack(u16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x10", 2) == 0);

    assert(uint16Unpack(&buf, 0, &u16) == 2);
    assert(u16 == 16);

    bufClear(&buf);

    i16 = 16;
    assert(int16Pack(i16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x10", 2) == 0);

    assert(int16Unpack(&buf, 0, &i16) == 2);
    assert(i16 == 16);

    bufClear(&buf);

    u16 = -16;
    assert(uint16Pack(u16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xF0", 2) == 0);

    assert(uint16Unpack(&buf, 0, &u16) == 2);
    assert(u16 == (65536 - 16));

    bufClear(&buf);

    i16 = -16;
    assert(int16Pack(i16, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xF0", 2) == 0);

    assert(int16Unpack(&buf, 0, &i16) == 2);
    assert(i16 == -16);

    bufClear(&buf);

    u32 = 32;
    assert(uint32Pack(u32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x20", 4) == 0);

    assert(uint32Unpack(&buf, 0, &u32) == 4);
    assert(u32 == 32);

    bufClear(&buf);

    i32 = 32;
    assert(int32Pack(i32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x20", 4) == 0);

    assert(int32Unpack(&buf, 0, &i32) == 4);
    assert(i32 == 32);

    bufClear(&buf);

    u32 = -32;
    assert(uint32Pack(u32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xE0", 4) == 0);

    assert(uint32Unpack(&buf, 0, &u32) == 4);
    assert(u32 == (0x100000000L - 32));

    bufClear(&buf);

    i32 = -32;
    assert(int32Pack(i32, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xE0", 4) == 0);

    assert(int32Unpack(&buf, 0, &i32) == 4);
    assert(i32 == -32);

    bufClear(&buf);

    u64 = 64;
    assert(uint64Pack(u64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);

    assert(uint64Unpack(&buf, 0, &u64) == 8);
    assert(u64 == 64);

    bufClear(&buf);

    i64 = 64;
    assert(int64Pack(i64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);

    assert(int64Unpack(&buf, 0, &i64) == 8);
    assert(i64 == 64);

    bufClear(&buf);

    u64 = -64;
    assert(uint64Pack(u64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);

    assert(uint64Unpack(&buf, 0, &u64) == 8);
    assert(u64 == (0xFFFFFFFFFFFFFFFF - 63));

    bufClear(&buf);

    i64 = -64;
    assert(int64Pack(i64, &buf) == &buf);
    assert(memcmp(bufGet(&buf), "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);

    assert(int64Unpack(&buf, 0, &i64) == 8);
    assert(i64 == -64);

    bufClear(&buf);

    u32 = 256;

    assert(uint32Pack(u32, &buf) == &buf);

    assert(bufLen(&buf) == 4);
    assert(memcmp(bufGet(&buf), "\x00\x00\x01\x00", 4) == 0);

    astring *as = asCreate("Hoi");

    assert(astringPack(as, &buf) == &buf);

    assert(bufLen(&buf) == 11);
    assert(memcmp(bufGet(&buf), "\x00\x00\x01\x00\x00\x00\x00\x03" "Hoi", 11) == 0);

    wstring *ws = wsCreate(L"αß¢");

    assert(wstringPack(ws, &buf) == &buf);

    assert(bufLen(&buf) == 21);
    assert(memcmp(bufGet(&buf),
                "\x00\x00\x01\x00"
                "\x00\x00\x00\x03" "Hoi"
                "\x00\x00\x00\x06\xCE\xB1\xC3\x9F\xC2\xA2", 21) == 0);

    return 0;
}
#endif
