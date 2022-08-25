/* libtyger.c: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
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
 * Resize <buf>, which currently has size <size>, so that it can hold at
 * least <requirement> bytes.
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

// =============================== Indent handling ===============================

/*
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

// =============================== "Length" functions ===============================

size_t bufferLen(const buffer *buf, size_t pos)
{
    return buf->len - pos;
}

// =============================== "Get" functions ===============================

const uint8_t *bufferGet(const buffer *buf, size_t pos)
{
    return buf->data + pos;
}

uint8_t bufferGetC(const buffer *buf, size_t pos)
{
    return buf->data[pos];
}

const char *astringGet(const astring *astr)
{
    return astr->data;
}

const wchar_t *ustringGet(const ustring *ustr)
{
    return ustr->data;
}

// =============================== "Clear" functions ===============================

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

void bufferClear(buffer *buf)
{
    free(buf->data);

    memset(buf, 0, sizeof(buffer));
}

astring *astringClear(astring *astr)
{
    free(astr->data);

    memset(astr, 0, sizeof(astring));

    return astr;
}

ustring *ustringClear(ustring *ustr)
{
    free(ustr->data);

    memset(ustr, 0, sizeof(ustring));

    return ustr;
}

// =============================== "Destroy" functions ===============================

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

// =============================== "Add" functions ===============================

buffer *bufferAdd(buffer *buf, const void *add_data, size_t add_size)
{
    buffer_expand(buf, buf->len + add_size);

    memcpy(buf->data + buf->len, add_data, add_size);

    buf->len += add_size;

    return buf;
}

buffer *bufferAddC(buffer *buf, uint8_t add_data)
{
    return bufferAdd(buf, &add_data, 1);
}

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

astring *astringAddZ(astring *astr, const char *data)
{
    return astringAdd(astr, data, strlen(data));
}

ustring *ustringAddZ(ustring *ustr, const wchar_t *data)
{
    return ustringAdd(ustr, data, wcslen(data));
}

// =============================== "Rewind" functions ===============================

astring *astringRewind(astring *astr)
{
    astr->len = 0;

    if (astr->cap > 0) astr->data[0] = 0;

    return astr;
}

ustring *ustringRewind(ustring *ustr)
{
    ustr->len = 0;

    if (ustr->cap > 0) ustr->data[0] = 0;

    return ustr;
}

// =============================== "Set" functions ===============================

astring *astringSet(astring *astr, const char *data, size_t data_len)
{
    return astringAdd(astringRewind(astr), data, data_len);
}

ustring *ustringSet(ustring *ustr, const wchar_t *data, size_t data_len)
{
    return ustringAdd(ustringRewind(ustr), data, data_len);
}

astring *astringSetZ(astring *astr, const char *data)
{
    return astringSet(astringRewind(astr), data, strlen(data));
}

ustring *ustringSetZ(ustring *ustr, const wchar_t *data)
{
    return ustringSet(ustringRewind(ustr), data, wcslen(data));
}

// =============================== "Make" functions ===============================

astring astringMake(const char *str)
{
    astring new_str = { };

    astringSetZ(&new_str, str);

    return new_str;
}

ustring ustringMake(const wchar_t *str)
{
    ustring new_str = { };

    ustringSetZ(&new_str, str);

    return new_str;
}

// =============================== "Create" functions ===============================

astring *astringCreate(const char *str)
{
    astring *new_str = calloc(1, sizeof(astring));

    astringSetZ(new_str, str);

    return new_str;
}

ustring *ustringCreate(const wchar_t *str)
{
    ustring *new_str = calloc(1, sizeof(ustring));

    ustringSetZ(new_str, str);

    return new_str;
}

// =============================== "PackSize" functions ===============================

/*
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

// =============================== "Pack" functions ===============================

/*
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
buffer *boolPack(const bool data, buffer *buf)
{
    bufferAddC(buf, data ? 1 : 0);

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

// =============================== "Unpack" functions ===============================

/*
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

size_t int8Unpack(const buffer *buf, size_t pos, int8_t *data)
{
    return uint8Unpack(buf, pos, (uint8_t *) data);
}

size_t int16Unpack(const buffer *buf, size_t pos, int16_t *data)
{
    return uint16Unpack(buf, pos, (uint16_t *) data);
}

size_t int32Unpack(const buffer *buf, size_t pos, int32_t *data)
{
    return uint32Unpack(buf, pos, (uint32_t *) data);
}

size_t int64Unpack(const buffer *buf, size_t pos, int64_t *data)
{
    return uint64Unpack(buf, pos, (uint64_t *) data);
}

/*
 * Unpack a float from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
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
 * Unpack a double from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
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

#if 0
/*
 * Read <size> bytes from <fd> into <data>. Returns the number of bytes read
 * (which might be less than <size> if an error occurred).
 */
static size_t read_FD(int fd, void *data, size_t size)
{
    size_t count = 0;

    while (count < size) {
        int status = read(fd, (char *) data + count, size - count);

        if (status > 0) {
            count += status;
            continue;
        }
        else if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
            continue;
        }
        else {
            break;
        }
    }

    return count;
}

/*
 * Write <size> bytes from <data> to <fd>. Returns the number of bytes written
 * (which might be less than <size> if an error occurred).
 */
static size_t write_FD(int fd, const void *data, size_t size)
{
    size_t count = 0;

    while (count < size) {
        int status = write(fd, (char *) data + count, size - count);

        if (status > 0) {
            count += status;
            continue;
        }
        else if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
            continue;
        }
        else {
            break;
        }
    }

    return count;
}

/*
 * Read <size> bytes from <fp> into <data>. Returns the number of bytes read
 * (which might be less than <size> if an error occurred).
 */
static size_t read_FP(FILE *fp, void *data, size_t size)
{
    return size * fread(data, size, 1, fp);
}

/*
 * Write <size> bytes from <data> to <fp>. Returns the number of bytes written
 * (which might be less than <size> if an error occurred).
 */
static size_t write_FP(FILE *fp, const void *data, size_t size)
{
    return size * fwrite(data, size, 1, fp);
}

/*
 * Copy <size> bytes from <to> to <from>, reversing them if we are on a
 * little-endian architecture. It's OK if <from> and <to> overlap or point to
 * the same data.
 */
static void reverse_if_little_endian(const char *from, char *to, size_t size)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    char temp[size];
    int f, t;

    for (f = 0, t = size - 1; f < size; f++, t--) {
        temp[t] = from[f];
    }

    memcpy(to, temp, size);
#else
    if (from != to) {
        memmove(to, from, size);
    }
#endif
}

/*
 * Take the least-significant <size> bytes from <data> and write them to <buf>
 * (which is assumed to have <size> bytes or more of room), in a big-endian
 * (most-significant byte first) layout.
 */
static void h_to_be(unsigned int data, char *buf, size_t size)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (int i = 0; i < size; i++) {
        int right_shift = 8 * (sizeof(data) - i - 1);
        buf[i] = (data >> right_shift) & 0xFF;
    }
#else
    memcpy(buf, ((char *) &data) + sizeof(data) - size, size);
#endif
}

/*
 * Take the <size> bytes from <buf>, which is assumed to contain <size> bytes
 * in a big-endian layout, and write them as a host-native integer to <data>.
 */
static void be_to_h(const char *buf, size_t size, unsigned int *data)
{
    *data = 0;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (int i = 0; i < size; i++) {
        *data <<= 8;
        *data |= buf[i];
    }
#else
    memcpy(((char *) &data) + sizeof(data) - size, buf, size);
#endif
}

static size_t read_FD_BE(int fd, void *data, size_t size)
{
    size_t count = read_FD(fd, data, size);

    reverse_if_little_endian(data, data, size);

    return count;
}

static size_t write_FD_BE(int fd, const void *data, size_t size)
{
    char temp[size];

    reverse_if_little_endian(data, temp, size);

    return write_FD(fd, temp, size);
}

static size_t read_FP_BE(FILE *fp, void *data, size_t size)
{
    size_t count = read_FP(fp, data, size);

    reverse_if_little_endian(data, data, size);

    return count;
}

static size_t write_FP_BE(FILE *fp, const void *data, size_t size)
{
    char temp[size];

    reverse_if_little_endian(data, temp, size);

    return write_FP(fp, temp, size);
}

/*
 * Read <num_bytes> bytes from <fd> and put them towards the least-significant
 * side of <data>.
 */
size_t uintReadFromFD(int fd, size_t num_bytes, unsigned int *data)
{
    char temp[num_bytes];

    int r = read_FD(fd, temp, num_bytes);

    be_to_h(temp, num_bytes, data);

    return r;
}

/*
 * Write the <num_bytes> least-significant bytes from <data> to <fd>
 */
size_t uintWriteToFD(int fd, size_t num_bytes, unsigned int data)
{
    char temp[num_bytes];

    h_to_be(data, temp, num_bytes);

    return write_FD(fd, temp, num_bytes);
}

/*
 * <num_bytes> bytes from <fp> and put them towards the least-significant side
 * of <data>.
 */
size_t uintReadFromFP(FILE *fp, size_t num_bytes, unsigned int *data)
{
    char temp[num_bytes];

    int r = read_FP(fp, temp, num_bytes);

    be_to_h(temp, num_bytes, data);

    return r;
}

/*
 * Write the <num_bytes> least-significant bytes from <data> to <fp>
 */
size_t uintWriteToFP(FILE *fp, size_t num_bytes, unsigned int data)
{
    char temp[num_bytes];

    h_to_be(data, temp, num_bytes);

    return write_FP(fp, temp, num_bytes);
}

/*
 * Read an char *from <fd> into <data).
 */
size_t astringReadFromFD(int fd, char **data)
{
    size_t count = 0;
    uint32_t length;

    count += uint32ReadFromFD(fd, &length);

    if (*data != NULL) free(*data);

    *data = calloc(length + 1, sizeof(char));

    count += read_FD(fd, *data, length);

    return count;
}

/*
 * Write an char *to <fd> from <data).
 */
size_t astringWriteToFD(int fd, const char *data)
{
    size_t count = 0;
    uint32_t length = (data == NULL) ? 0 : strlen(data);

    count += uint32WriteToFD(fd, length);

    count += write_FD(fd, data, length);

    return count;
}

/*
 * Read an char *from <fp> into <data).
 */
size_t astringReadFromFP(FILE *fp, char **data)
{
    size_t count = 0;
    uint32_t length;

    count += uint32ReadFromFP(fp, &length);

    if (*data != NULL) free(*data);

    *data = calloc(length + 1, sizeof(char));

    count += read_FP(fp, *data, length);

    return count;
}

/*
 * Write an char *to <fp> from <data).
 */
size_t astringWriteToFP(FILE *fp, const char *data)
{
    size_t count = 0;
    uint32_t length = (data == NULL) ? 0 : strlen(data);

    count += uint32WriteToFP(fp, length);

    count += write_FP(fp, data, length);

    return count;
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void astringPrint(FILE *fp, const char *data, int indent)
{
    fprintf(fp, "\"%s\"", data);
}

/*
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
 * Read a wchar_t *from <fd> into <data).
 */
size_t ustringReadFromFD(int fd, wchar_t **data)
{
    size_t wchar_count, count = 0;
    uint32_t uint8_len;

    count += uint32ReadFromFD(fd, &uint8_len);

    uint8_t *uint8_buf = calloc(1, uint8_len + 1);

    count += read_FD(fd, uint8_buf, uint8_len);

    const wchar_t *wchar = utf8_to_wchar(uint8_buf, uint8_len, &wchar_count);

    if (*data != NULL) free(*data);

    *data = calloc(wchar_count + 1, sizeof(wchar_t));

    memcpy(*data, wchar, (wchar_count + 1) * sizeof(wchar_t));

    return count;
}

/*
 * Write a wchar_t *to <fd> from <data).
 */
size_t ustringWriteToFD(int fd, const wchar_t *data)
{
    uint32_t count = 0;
    uint32_t utf8_size;
    const char *utf8_text;

    if (data == NULL) {
        utf8_size = 0;
    }
    else {
        utf8_text = wchar_to_utf8(data, wcslen(data), &utf8_size);
    }

    count += uint32WriteToFD(fd, utf8_size);

    count += write_FD(fd, utf8_text, utf8_size);

    return count;
}

/*
 * Read a wchar_t *from <fp> into <data).
 */
size_t ustringReadFromFP(FILE *fp, wchar_t **data)
{
    size_t wchar_count, count = 0;
    uint32_t uint8_len;

    count += uint32ReadFromFP(fp, &uint8_len);

    uint8_t *uint8_buf = calloc(1, uint8_len + 1);

    count += read_FP(fp, uint8_buf, uint8_len);

    const wchar_t *wchar = utf8_to_wchar(uint8_buf, uint8_len, &wchar_count);

    if (*data != NULL) free(*data);

    *data = calloc(wchar_count + 1, sizeof(wchar_t));

    memcpy(*data, wchar, (wchar_count + 1) * sizeof(wchar_t));

    return count;
}

/*
 * Write a wchar_t *to <fp> from <data).
 */
size_t ustringWriteToFP(FILE *fp, const wchar_t *data)
{
    uint32_t count = 0;
    uint32_t utf8_size;
    const char *utf8_text;

    if (data == NULL) {
        utf8_size = 0;
    }
    else {
        utf8_text = wchar_to_utf8(data, wcslen(data), &utf8_size);
    }

    count += uint32WriteToFP(fp, utf8_size);

    count += write_FP(fp, utf8_text, utf8_size);

    return count;
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void ustringPrint(FILE *fp, const wchar_t *data, int indent)
{
    fprintf(fp, "\"%ls\"", data);
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

/*
 * Read a float from <fd> into <data).
 */
size_t float32ReadFromFD(int fd, float *data)
{
    return read_FD_BE(fd, data, sizeof(float));
}

/*
 * Write a float to <fd> from <data).
 */
size_t float32WriteToFD(int fd, float data)
{
    return write_FD_BE(fd, &data, sizeof(float));
}

/*
 * Read a float from <fp> into <data).
 */
size_t float32ReadFromFP(FILE *fp, float *data)
{
    return read_FP_BE(fp, data, sizeof(float));
}

/*
 * Write a float to <fp> from <data).
 */
size_t float32WriteToFP(FILE *fp, float data)
{
    return write_FP_BE(fp, &data, sizeof(float));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float32Print(FILE *fp, float data, int indent)
{
    fprintf(fp, "%g", data);
}

/*
 * Read a double from <fd> into <data).
 */
size_t float64ReadFromFD(int fd, double *data)
{
    return read_FD_BE(fd, data, sizeof(double));
}

/*
 * Write a double to <fd> from <data).
 */
size_t float64WriteToFD(int fd, double data)
{
    return write_FD_BE(fd, &data, sizeof(double));
}

/*
 * Read a double from <fp> into <data).
 */
size_t float64ReadFromFP(FILE *fp, double *data)
{
    return read_FP_BE(fp, data, sizeof(double));
}

/*
 * Write a double to <fp> from <data).
 */
size_t float64WriteToFP(FILE *fp, double data)
{
    return write_FP_BE(fp, &data, sizeof(double));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float64Print(FILE *fp, double data, int indent)
{
    fprintf(fp, "%g", data);
}

/*
 * Read a bool from <fd> into <data).
 */
size_t boolReadFromFD(int fd, bool *data)
{
    uint8_t encoded;

    size_t r = read_FD(fd, &encoded, sizeof(encoded));

    *data = bool_decode(encoded);

    return r;
}

/*
 * Write a bool to <fd> from <data).
 */
size_t boolWriteToFD(int fd, bool data)
{
    uint8_t encoded = bool_encode(data);

    return write_FD(fd, &encoded, sizeof(encoded));
}

/*
 * Read a bool from <fp> into <data).
 */
size_t boolReadFromFP(FILE *fp, bool *data)
{
    uint8_t encoded;

    size_t r = read_FP(fp, &encoded, sizeof(encoded));

    *data = bool_decode(encoded);

    return r;
}

/*
 * Write a bool to <fp> from <data).
 */
size_t boolWriteToFP(FILE *fp, bool data)
{
    uint8_t encoded = bool_encode(data);

    return write_FP(fp, &encoded, sizeof(encoded));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void boolPrint(FILE *fp, bool data, int indent)
{
    fprintf(fp, "%s", data ? "true" : "false");
}

/*
 * Copy <src> to <dst>.
 */
void boolCopy(bool *dst, bool src)
{
    assert(dst != NULL);

    *dst = src;
}

/*
 * Read a uint8_t from <fd> into <data).
 */
size_t uint8ReadFromFD(int fd, uint8_t *data)
{
    return read_FD(fd, data, sizeof(uint8_t));
}

/*
 * Write a uint8_t to <fd> from <data).
 */
size_t uint8WriteToFD(int fd, uint8_t data)
{
    return write_FD(fd, &data, sizeof(uint8_t));
}

/*
 * Read a uint8_t from <fp> into <data).
 */
size_t uint8ReadFromFP(FILE *fp, uint8_t *data)
{
    return read_FP(fp, data, sizeof(uint8_t));
}

/*
 * Write a uint8_t to <fp> from <data).
 */
size_t uint8WriteToFP(FILE *fp, uint8_t data)
{
    return write_FP(fp, &data, sizeof(uint8_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint8Print(FILE *fp, uint8_t data, int indent)
{
    fprintf(fp, "%" PRIu8, data);
}

/*
 * Return the number of bytes required to pack an int8_t.
 */
size_t int8PackSize(void)
{
    return sizeof(int8_t);
}

/*
 * Unpack an int8_t from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int8Unpack(const buffer *buf, size_t pos, int8_t *data)
{
    size_t req = int8PackSize();

    assert(bufferLen(buf, pos) >= req);

        *data = buf[0];

    return req;
}

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int8Pack(const int8_t data, buffer *buf)
{
    buffer_expand(buf, size, *pos + sizeof(int8_t));

    (*buf)[(*pos)++] = data;

    return sizeof(uint8_t);
}

/*
 * Read an int8_t from <fd> into <data).
 */
size_t int8ReadFromFD(int fd, int8_t *data)
{
    return read_FD(fd, data, sizeof(int8_t));
}

/*
 * Write an int8_t to <fd> from <data).
 */
size_t int8WriteToFD(int fd, int8_t data)
{
    return write_FD(fd, &data, sizeof(int8_t));
}

/*
 * Read an int8_t from <fp> into <data).
 */
size_t int8ReadFromFP(FILE *fp, int8_t *data)
{
    return read_FP(fp, data, sizeof(int8_t));
}

/*
 * Write an int8_t to <fp> from <data).
 */
size_t int8WriteToFP(FILE *fp, int8_t data)
{
    return write_FP(fp, &data, sizeof(int8_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int8Print(FILE *fp, int8_t data, int indent)
{
    fprintf(fp, "%" PRId8, data);
}

/*
 * Read a uint16_t from <fd> into <data).
 */
size_t uint16ReadFromFD(int fd, uint16_t *data)
{
    return read_FD_BE(fd, data, sizeof(uint16_t));
}

/*
 * Write a uint16_t to <fd> from <data).
 */
size_t uint16WriteToFD(int fd, uint16_t data)
{
    return write_FD_BE(fd, &data, sizeof(uint16_t));
}

/*
 * Read a uint16_t from <fp> into <data).
 */
size_t uint16ReadFromFP(FILE *fp, uint16_t *data)
{
    return read_FP_BE(fp, data, sizeof(uint16_t));
}

/*
 * Write a uint16_t to <fp> from <data).
 */
size_t uint16WriteToFP(FILE *fp, uint16_t data)
{
    return write_FP_BE(fp, &data, sizeof(uint16_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint16Print(FILE *fp, uint16_t data, int indent)
{
    fprintf(fp, "%" PRIu16, data);
}

/*
 * Read a int16_t from <fd> into <data).
 */
size_t int16ReadFromFD(int fd, int16_t *data)
{
    return read_FD_BE(fd, data, sizeof(uint16_t));
}

/*
 * Write a int16_t to <fd> from <data).
 */
size_t int16WriteToFD(int fd, int16_t data)
{
    return write_FD_BE(fd, &data, sizeof(int16_t));
}

/*
 * Read a int16_t from <fp> into <data).
 */
size_t int16ReadFromFP(FILE *fp, int16_t *data)
{
    return read_FP_BE(fp, data, sizeof(int16_t));
}

/*
 * Write a int16_t to <fp> from <data).
 */
size_t int16WriteToFP(FILE *fp, int16_t data)
{
    return write_FP_BE(fp, &data, sizeof(int16_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int16Print(FILE *fp, int16_t data, int indent)
{
    fprintf(fp, "%" PRId16, data);
}

/*
 * Read a uint32_t from <fd> into <data).
 */
size_t uint32ReadFromFD(int fd, uint32_t *data)
{
    return read_FD_BE(fd, data, sizeof(uint32_t));
}

/*
 * Write a uint32_t to <fd> from <data).
 */
size_t uint32WriteToFD(int fd, uint32_t data)
{
    return write_FD_BE(fd, &data, sizeof(uint32_t));
}

/*
 * Read a uint32_t from <fp> into <data).
 */
size_t uint32ReadFromFP(FILE *fp, uint32_t *data)
{
    return read_FP_BE(fp, data, sizeof(uint32_t));
}

/*
 * Write a uint32_t to <fp> from <data).
 */
size_t uint32WriteToFP(FILE *fp, uint32_t data)
{
    return write_FP_BE(fp, &data, sizeof(uint32_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint32Print(FILE *fp, uint32_t data, int indent)
{
    fprintf(fp, "%" PRIu32, data);
}

/*
 * Read an int32_t from <fd> into <data).
 */
size_t int32ReadFromFD(int fd, int32_t *data)
{
    return read_FD_BE(fd, data, sizeof(uint32_t));
}

/*
 * Write an int32_t to <fd> from <data).
 */
size_t int32WriteToFD(int fd, int32_t data)
{
    return write_FD_BE(fd, &data, sizeof(int32_t));
}

/*
 * Read an int32_t from <fp> into <data).
 */
size_t int32ReadFromFP(FILE *fp, int32_t *data)
{
    return read_FP_BE(fp, data, sizeof(int32_t));
}

/*
 * Write an int32_t to <fp> from <data).
 */
size_t int32WriteToFP(FILE *fp, int32_t data)
{
    return write_FP_BE(fp, &data, sizeof(int32_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int32Print(FILE *fp, int32_t data, int indent)
{
    fprintf(fp, "%" PRId32, data);
}

/*
 * Read a uint64_t from <fd> into <data).
 */
size_t uint64ReadFromFD(int fd, uint64_t *data)
{
    return read_FD_BE(fd, data, sizeof(uint64_t));
}

/*
 * Write a uint64_t to <fd> from <data).
 */
size_t uint64WriteToFD(int fd, uint64_t data)
{
    return write_FD_BE(fd, &data, sizeof(uint64_t));
}

/*
 * Read a uint64_t from <fp> into <data).
 */
size_t uint64ReadFromFP(FILE *fp, uint64_t *data)
{
    return read_FP_BE(fp, data, sizeof(uint64_t));
}

/*
 * Write a uint64_t to <fp> from <data).
 */
size_t uint64WriteToFP(FILE *fp, uint64_t data)
{
    return write_FP_BE(fp, &data, sizeof(uint64_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint64Print(FILE *fp, uint64_t data, int indent)
{
    fprintf(fp, "%" PRIu64, data);
}

/*
 * Read an int64_t from <fd> into <data).
 */
size_t int64ReadFromFD(int fd, int64_t *data)
{
    return read_FD_BE(fd, data, sizeof(int64_t));
}

/*
 * Write an int64_t to <fd> from <data).
 */
size_t int64WriteToFD(int fd, int64_t data)
{
    return write_FD_BE(fd, &data, sizeof(int64_t));
}

/*
 * Read an int64_t from <fp> into <data).
 */
size_t int64ReadFromFP(FILE *fp, int64_t *data)
{
    return read_FP_BE(fp, data, sizeof(int64_t));
}

/*
 * Write an int64_t to <fp> from <data).
 */
size_t int64WriteToFP(FILE *fp, int64_t data)
{
    return write_FP_BE(fp, &data, sizeof(int64_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int64Print(FILE *fp, int64_t data, int indent)
{
    fprintf(fp, "%" PRId64, data);
}
#endif

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
