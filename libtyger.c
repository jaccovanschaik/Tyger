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

// =============================== Internals ===============================

/*
 * Resize <buf> so that it can hold at least <req> bytes.
 *
 * If the size is insufficient it will be doubled until it is. If it is
 * currently 0, it will be initialized to 1024.
 */
static void buffer_expand(buffer *buf, size_t req)
{
    bool must_realloc = false;

    while (buf->cap < req) {
        buf->cap = (buf->cap == 0) ? 1024 : 2 * buf->cap;

        must_realloc = true;
    }

    if (must_realloc) {
        buf->data = realloc(buf->data, buf->cap * sizeof(uint8_t));
    }
}

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

/* =============================== "Length" functions ===============================
 *
 * Return the number of used bytes in <buf>, counted from <pos>.
 */
size_t bufferLen(const buffer *buf, size_t pos)
{
    return buf->len - pos;
}

/* =============================== "Get" functions ===============================
 *
 * Get a pointer to the contents of <buf>, starting at <pos>.
 */
const uint8_t *bufferGet(const buffer *buf, size_t pos)
{
    return buf->data + pos;
}

/*
 * Get the byte at <pos> in <buf>.
 */
uint8_t bufferGetC(const buffer *buf, size_t pos)
{
    return buf->data[pos];
}

/*
 * Get a pointer to the contents of <astr>.
 */
const char *astringGet(const astring *astr)
{
    return astr->data;
}

/*
 * Get a pointer to the contents of <astr>.
 */
const wchar_t *ustringGet(const ustring *ustr)
{
    return ustr->data;
}

/* =============================== "Clear" functions ===============================
 *
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

/*
 * Clear the contents of <buf>.
 */
void bufferClear(buffer *buf)
{
    free(buf->data);

    memset(buf, 0, sizeof(buffer));
}

/*
 * Clear the contents of <astr>.
 */
astring *astringClear(astring *astr)
{
    free(astr->data);

    memset(astr, 0, sizeof(astring));

    return astr;
}

/*
 * Clear the contents of <ustr>.
 */
ustring *ustringClear(ustring *ustr)
{
    free(ustr->data);

    memset(ustr, 0, sizeof(ustring));

    return ustr;
}

/* =============================== "Destroy" functions ===============================
 *
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

/*
 * Destroy buffer <buf>.
 */
void bufferDestroy(buffer *buf)
{
    bufferClear(buf);

    free(buf);
}

/*
 * Destroy astring <astr>.
 */
void astringDestroy(astring *astr)
{
    astringClear(astr);

    free(astr);
}

/*
 * Destroy ustring <ustr>.
 */
void ustringDestroy(ustring *ustr)
{
    ustringClear(ustr);

    free(ustr);
}

/* =============================== "Add" functions ===============================
 *
 * Add <add_data>, which has size <add_size> to <buf>.
 */
buffer *bufferAdd(buffer *buf, const void *add_data, size_t add_size)
{
    buffer_expand(buf, buf->len + add_size);

    memcpy(buf->data + buf->len, add_data, add_size);

    buf->len += add_size;

    return buf;
}

/*
 * Add the character <add_data> to <buf>.
 */
buffer *bufferAddC(buffer *buf, uint8_t add_data)
{
    return bufferAdd(buf, &add_data, 1);
}

/*
 * Add the string at <data>, which has size <data_len> to <astr>.
 */
astring *astringAdd(astring *astr, const char *data, size_t data_len)
{
    size_t req_cap = sizeof(char) * (astr->len + data_len + 1);

    if (astr->cap < req_cap) {
        astr->cap = req_cap;

        astr->data = realloc(astr->data, req_cap);
    }

    memcpy(astr->data + astr->len, data, data_len);

    astr->len += data_len;

    astr->data[astr->len] = 0;

    return astr;
}

/*
 * Add the string at <data>, which has size <data_len> to <ustr>.
 */
ustring *ustringAdd(ustring *ustr, const wchar_t *data, size_t data_len)
{
    size_t req_cap = sizeof(wchar_t) * (ustr->len + data_len + 1);

    if (ustr->cap < req_cap) {
        ustr->cap = req_cap;

        ustr->data = realloc(ustr->data, req_cap);
    }

    wmemcpy(ustr->data + ustr->len, data, data_len);

    ustr->len += data_len;

    ustr->data[ustr->len] = 0;

    return ustr;
}

/*
 * Add the zero-terminated string at <data> to <astr>.
 */
astring *astringAddZ(astring *astr, const char *data)
{
    return astringAdd(astr, data, strlen(data));
}

/*
 * Add the zero-terminated string at <data> to <ustr>.
 */
ustring *ustringAddZ(ustring *ustr, const wchar_t *data)
{
    return ustringAdd(ustr, data, wcslen(data));
}

/* =============================== "Rewind" functions ===============================
 *
 * Rewind <astr> back to the beginning.
 */
astring *astringRewind(astring *astr)
{
    astr->len = 0;

    if (astr->cap > 0) astr->data[0] = 0;

    return astr;
}

/*
 * Rewind <ustr> back to the beginning.
 */
ustring *ustringRewind(ustring *ustr)
{
    ustr->len = 0;

    if (ustr->cap > 0) ustr->data[0] = 0;

    return ustr;
}

/* =============================== "Set" functions ===============================
 *
 * Set <astr> to the string starting at <data>, which has length <data_len>.
 */
astring *astringSet(astring *astr, const char *data, size_t data_len)
{
    return astringAdd(astringRewind(astr), data, data_len);
}

/*
 * Set <ustr> to the string starting at <data>, which has length <data_len>.
 */
ustring *ustringSet(ustring *ustr, const wchar_t *data, size_t data_len)
{
    return ustringAdd(ustringRewind(ustr), data, data_len);
}

/*
 * Set <astr> to the null-terminated string starting at <data>.
 */
astring *astringSetZ(astring *astr, const char *data)
{
    return astringSet(astr, data, strlen(data));
}

/*
 * Set <ustr> to the null-terminated string starting at <data>.
 */
ustring *ustringSetZ(ustring *ustr, const wchar_t *data)
{
    return ustringSet(ustr, data, wcslen(data));
}

/* =============================== "Make" functions ===============================
 *
 * Make a new astring with the given null-terminated string as its original contents and return it
 * by value.
 */
astring astringMake(const char *str)
{
    astring new_str = { };

    astringSetZ(&new_str, str);

    return new_str;
}

/*
 * Make a new astring with the given null-terminated string as its original contents and return it
 * by value.
 */
ustring ustringMake(const wchar_t *str)
{
    ustring new_str = { };

    ustringSetZ(&new_str, str);

    return new_str;
}

/* =============================== "Create" functions ===============================
 *
 * Create a new astring with the given null-terminated string as its original contents and return a
 * pointer to it.
 */
astring *astringCreate(const char *str)
{
    astring *new_str = calloc(1, sizeof(astring));

    astringSetZ(new_str, str);

    return new_str;
}

/*
 * Create a new ustring with the given null-terminated string as its original contents and return a
 * pointer to it.
 */
ustring *ustringCreate(const wchar_t *str)
{
    ustring *new_str = calloc(1, sizeof(ustring));

    ustringSetZ(new_str, str);

    return new_str;
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
size_t astringPackSize(const astring *str)
{
    return uint32PackSize() + str->len;
}

/*
 * Return the number of bytes required to pack the wchar_t *pointed to by <data>.
 */
size_t ustringPackSize(const ustring *str)
{
    size_t pack_size = uint32PackSize();

    if (str->data == NULL || str->len == 0) return pack_size;

    uint32_t utf8_size;

    wchar_to_utf8(str->data, str->len, &utf8_size);

    return pack_size + utf8_size;
}

/* =============================== "Pack" functions ===============================
 *
 * Pack the least-significant <num_bytes> of <data> into <buf>, updating
 * <size> and <pos>.
 */
buffer *uintPack(unsigned int data, size_t num_bytes, buffer *buf)
{
    for (int i = num_bytes - 1; i >= 0; i--) {
        bufferAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint8Pack(uint8_t data, buffer *buf)
{
    size_t pack_size = uint8PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufferAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint16Pack(uint16_t data, buffer *buf)
{
    size_t pack_size = uint16PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufferAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint32Pack(uint32_t data, buffer *buf)
{
    size_t pack_size = uint32PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufferAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint64Pack(uint64_t data, buffer *buf)
{
    size_t pack_size = uint64PackSize();

    for (int i = pack_size - 1; i >= 0; i--) {
        bufferAddC(buf, (data >> (8 * i)) & 0xFF);
    }

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int8Pack(int8_t data, buffer *buf)
{
    return uint8Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int16Pack(int16_t data, buffer *buf)
{
    return uint16Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int32Pack(int32_t data, buffer *buf)
{
    return uint32Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int64Pack(int64_t data, buffer *buf)
{
    return uint64Pack(data, buf);
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *float32Pack(const float data, buffer *buf)
{
    union {
        float f;
        char c[4];
    } u;

    u.f = data;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    bufferAddC(buf, u.c[3]);
    bufferAddC(buf, u.c[2]);
    bufferAddC(buf, u.c[1]);
    bufferAddC(buf, u.c[0]);
#else
    bufferAdd(buf, &u.f);
#endif

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *float64Pack(const double data, buffer *buf)
{
    union {
        double f;
        char c[8];
    } u;

    u.f = data;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    bufferAddC(buf, u.c[7]);
    bufferAddC(buf, u.c[6]);
    bufferAddC(buf, u.c[5]);
    bufferAddC(buf, u.c[4]);
    bufferAddC(buf, u.c[3]);
    bufferAddC(buf, u.c[2]);
    bufferAddC(buf, u.c[1]);
    bufferAddC(buf, u.c[0]);
#else
    bufferAdd(buf, &u.f, 8);
#endif

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *boolPack(const bool data, buffer *buf)
{
    bufferAddC(buf, data ? 1 : 0);

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *astringPack(const astring *str, buffer *buf)
{
    uint32Pack(str->len, buf);

    if (str->data != NULL && str->len > 0) bufferAdd(buf, str->data, str->len);

    return buf;
}

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
buffer *ustringPack(const ustring *str, buffer *buf)
{
    if (str->data == NULL || str->len == 0) {
        uint32Pack(0, buf);
    }
    else {
        uint32_t utf8_size;
        const uint8_t *utf8_text = wchar_to_utf8(str->data, str->len, &utf8_size);

        uint32Pack(utf8_size, buf);

        bufferAdd(buf, utf8_text, utf8_size);
    }

    return buf;
}

/* =============================== "Unpack" functions ===============================
 *
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
size_t uintUnpack(size_t num_bytes, const buffer *buf, size_t pos, unsigned int *data)
{
    assert(bufferLen(buf, pos) >= num_bytes);

    *data = 0;

    for (int i = 0; i < num_bytes; i++, pos++) {
        *data <<= 8;

        *data |= bufferGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint8Unpack(const buffer *buf, size_t pos, uint8_t *data)
{
    size_t pack_size = uint8PackSize();

    assert(bufferLen(buf, pos) >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufferGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint16Unpack(const buffer *buf, size_t pos, uint16_t *data)
{
    size_t pack_size = uint16PackSize();

    assert(bufferLen(buf, pos) >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufferGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint32Unpack(const buffer *buf, size_t pos, uint32_t *data)
{
    size_t pack_size = uint32PackSize();

    assert(bufferLen(buf, pos) >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufferGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack a uint64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint64Unpack(const buffer *buf, size_t pos, uint64_t *data)
{
    size_t pack_size = uint64PackSize();

    assert(bufferLen(buf, pos) >= pack_size);

    *data = 0;

    for (int i = 0; i < pack_size; i++, pos++) {
        *data <<= 8;

        *data |= bufferGetC(buf, pos);
    }

    return pos;
}

/*
 * Unpack an int8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int8Unpack(const buffer *buf, size_t pos, int8_t *data)
{
    return uint8Unpack(buf, pos, (uint8_t *) data);
}

/*
 * Unpack an int16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int16Unpack(const buffer *buf, size_t pos, int16_t *data)
{
    return uint16Unpack(buf, pos, (uint16_t *) data);
}

/*
 * Unpack an int32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int32Unpack(const buffer *buf, size_t pos, int32_t *data)
{
    return uint32Unpack(buf, pos, (uint32_t *) data);
}

/*
 * Unpack an int64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int64Unpack(const buffer *buf, size_t pos, int64_t *data)
{
    return uint64Unpack(buf, pos, (uint64_t *) data);
}

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t boolUnpack(const buffer *buf, size_t pos, bool *data)
{
    size_t pack_size = boolPackSize();

    assert(bufferLen(buf, pos) >= pack_size);

    *data = (bufferGetC(buf, pos) == 1);

    pos += pack_size;

    return pos;
}

/*
 * Unpack a float (aka. float32) from <buf> (which has size <size>) and put it at <data>. Return the
 * new <pos>.
 */
size_t float32Unpack(const buffer *buf, size_t pos, float *data)
{
    size_t req = float32PackSize();

    union {
        float f;
        char c[4];
    } u;

    assert(bufferLen(buf, pos) >= req);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    u.c[3] = bufferGetC(buf, pos + 0);
    u.c[2] = bufferGetC(buf, pos + 1);
    u.c[1] = bufferGetC(buf, pos + 2);
    u.c[0] = bufferGetC(buf, pos + 3);
#else
    memcpy(u.c, bufferGet(buf, pos), 4);
#endif

    *data = u.f;

    return req;
}

/*
 * Unpack a double (aka. float64) from <buf> (which has size <size>) and put it at <data>. Return
 * the new <pos>.
 */
size_t float64Unpack(const buffer *buf, size_t pos, double *data)
{
    size_t req = float64PackSize();

    union {
        double f;
        char c[8];
    } u;

    assert(bufferLen(buf, pos) >= req);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    u.c[7] = bufferGetC(buf, pos + 0);
    u.c[6] = bufferGetC(buf, pos + 1);
    u.c[5] = bufferGetC(buf, pos + 2);
    u.c[4] = bufferGetC(buf, pos + 3);
    u.c[3] = bufferGetC(buf, pos + 4);
    u.c[2] = bufferGetC(buf, pos + 5);
    u.c[1] = bufferGetC(buf, pos + 6);
    u.c[0] = bufferGetC(buf, pos + 7);
#else
    memcpy(u.c, bufferGet(buf, pos), 8);
#endif

    *data = u.f;

    return req;
}

/*
 * Unpack an char *from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t astringUnpack(const buffer *buf, size_t pos, astring *data)
{
    size_t packsize_of_string_length = uint32PackSize();

    assert(bufferLen(buf, pos) >= packsize_of_string_length);

    uint32_t string_length;

    pos = uint32Unpack(buf, pos, &string_length);

    assert(bufferLen(buf, pos) >= string_length);

    astringSet(data, (char *) bufferGet(buf, pos), string_length);

    pos += string_length;

    return pos;
}

/*
 * Unpack a UTF-8 encoded Unicode string from <buf> (which has size <size>), write it to a newly
 * allocated wide-character string whose starting address is written to <wchar_str>, and return the
 * number of bytes consumed from <buf>.
 */
size_t ustringUnpack(const buffer *buf, size_t pos, ustring *data)
{
    size_t packsize_of_utf8_len = uint32PackSize();

    assert(bufferLen(buf, pos) >= packsize_of_utf8_len);

    uint32_t utf8_len;

    pos = uint32Unpack(buf, pos, &utf8_len);

    assert(bufferLen(buf, pos) >= utf8_len);

    size_t wchar_len;
    const wchar_t *wchar_tmp = utf8_to_wchar(bufferGet(buf, pos), utf8_len, &wchar_len);

    if (wchar_tmp) {
        ustringSet(data, wchar_tmp, wchar_len);
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
void ustringCopy(wchar_t **dst, const wchar_t *src)
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
void astringPrint(FILE *fp, const astring *str, int indent)
{
    fprintf(fp, "\"%s\"", str->data);
}

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void ustringPrint(FILE *fp, const ustring *str, int indent)
{
    fprintf(fp, "\"%ls\"", str->data);
}

#ifdef TEST
int main(int argc, char *argv[])
{
    buffer  buf  = { };
    astring *astr = NULL;
    ustring *ustr = NULL;

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

    astr = astringCreate("Hoi");
    assert(astringPackSize(astr) == 7);

    assert(astringPack(astr, &buf) == &buf);
    assert(buf.len == 7);
    assert(buf.cap >= 7);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x00\x00\x03Hoi", 7) == 0);

    assert(astringUnpack(&buf, 0, astr) == 7);
    assert(strcmp(astringGet(astr), "Hoi") == 0);

    bufferClear(&buf);

    ustr = ustringCreate(L"αß¢");
    assert(ustringPackSize(ustr) == 10);

    assert(ustringPack(ustr, &buf) == &buf);
    assert(buf.len == 10);
    assert(buf.cap >= 10);
    assert(memcmp(buf.data, "\x00\x00\x00\x06\xCE\xB1\xC3\x9F\xC2\xA2", 10) == 0);

    assert(ustringUnpack(&buf, 0, ustr) == 10);
    assert(wcscmp(ustringGet(ustr), L"αß¢") == 0);

    bufferClear(&buf);

    f32 = 1.0;

    assert(float32Pack(f32, &buf) == &buf);
    assert(float32PackSize() == 4);
    assert(memcmp(bufferGet(&buf, 0), "\x3F\x80\x00\x00", 4) == 0);

    assert(float32Unpack(&buf, 0, &f32) == 4);
    assert(f32 == 1.0);

    bufferClear(&buf);

    f64 = 2.0;

    assert(float64Pack(f64, &buf) == &buf);
    assert(float64PackSize() == 8);
    assert(memcmp(bufferGet(&buf, 0), "\x40\x00\x00\x00\x00\x00\x00\x00", 8) == 0);

    assert(float64Unpack(&buf, 0, &f64) == 8);
    assert(f64 == 2.0);

    bufferClear(&buf);

    u8 = 8;
    assert(uint8Pack(u8, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x08", 1) == 0);

    assert(uint8Unpack(&buf, 0, &u8) == 1);
    assert(u8 == 8);

    bufferClear(&buf);

    i8 = 8;
    assert(int8Pack(i8, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x08", 1) == 0);

    assert(int8Unpack(&buf, 0, &i8) == 1);
    assert(i8 == 8);

    bufferClear(&buf);

    u8 = -8;
    assert(uint8Pack(u8, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xF8", 1) == 0);

    assert(uint8Unpack(&buf, 0, &u8) == 1);
    assert(u8 == (256 - 8));

    bufferClear(&buf);

    i8 = -8;
    assert(int8Pack(i8, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xF8", 1) == 0);

    assert(int8Unpack(&buf, 0, &i8) == 1);
    assert(i8 == -8);

    bufferClear(&buf);

    u16 = 16;
    assert(uint16Pack(u16, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x10", 2) == 0);

    assert(uint16Unpack(&buf, 0, &u16) == 2);
    assert(u16 == 16);

    bufferClear(&buf);

    i16 = 16;
    assert(int16Pack(i16, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x10", 2) == 0);

    assert(int16Unpack(&buf, 0, &i16) == 2);
    assert(i16 == 16);

    bufferClear(&buf);

    u16 = -16;
    assert(uint16Pack(u16, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xFF\xF0", 2) == 0);

    assert(uint16Unpack(&buf, 0, &u16) == 2);
    assert(u16 == (65536 - 16));

    bufferClear(&buf);

    i16 = -16;
    assert(int16Pack(i16, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xFF\xF0", 2) == 0);

    assert(int16Unpack(&buf, 0, &i16) == 2);
    assert(i16 == -16);

    bufferClear(&buf);

    u32 = 32;
    assert(uint32Pack(u32, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x00\x00\x20", 4) == 0);

    assert(uint32Unpack(&buf, 0, &u32) == 4);
    assert(u32 == 32);

    bufferClear(&buf);

    i32 = 32;
    assert(int32Pack(i32, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x00\x00\x20", 4) == 0);

    assert(int32Unpack(&buf, 0, &i32) == 4);
    assert(i32 == 32);

    bufferClear(&buf);

    u32 = -32;
    assert(uint32Pack(u32, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xFF\xFF\xFF\xE0", 4) == 0);

    assert(uint32Unpack(&buf, 0, &u32) == 4);
    assert(u32 == (0x100000000L - 32));

    bufferClear(&buf);

    i32 = -32;
    assert(int32Pack(i32, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xFF\xFF\xFF\xE0", 4) == 0);

    assert(int32Unpack(&buf, 0, &i32) == 4);
    assert(i32 == -32);

    bufferClear(&buf);

    u64 = 64;
    assert(uint64Pack(u64, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);

    assert(uint64Unpack(&buf, 0, &u64) == 8);
    assert(u64 == 64);

    bufferClear(&buf);

    i64 = 64;
    assert(int64Pack(i64, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);

    assert(int64Unpack(&buf, 0, &i64) == 8);
    assert(i64 == 64);

    bufferClear(&buf);

    u64 = -64;
    assert(uint64Pack(u64, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);

    assert(uint64Unpack(&buf, 0, &u64) == 8);
    assert(u64 == (0xFFFFFFFFFFFFFFFF - 63));

    bufferClear(&buf);

    i64 = -64;
    assert(int64Pack(i64, &buf) == &buf);
    assert(memcmp(bufferGet(&buf, 0), "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);

    assert(int64Unpack(&buf, 0, &i64) == 8);
    assert(i64 == -64);

    bufferClear(&buf);

    u32 = 256;

    assert(uint32Pack(u32, &buf) == &buf);

    assert(buf.cap >= 4);
    assert(buf.len == 4);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x00\x01\x00", 4) == 0);

    assert(astringPack(astr, &buf) == &buf);

    assert(buf.cap >= 11);
    assert(buf.len == 11);
    assert(memcmp(bufferGet(&buf, 0), "\x00\x00\x01\x00\x00\x00\x00\x03" "Hoi", 11) == 0);

    assert(ustringPack(ustr, &buf) == &buf);

    assert(buf.cap >= 21);
    assert(buf.len == 21);
    assert(memcmp(bufferGet(&buf, 0),
                "\x00\x00\x01\x00"
                "\x00\x00\x00\x03" "Hoi"
                "\x00\x00\x00\x06\xCE\xB1\xC3\x9F\xC2\xA2", 21) == 0);

    return 0;
}
#endif
