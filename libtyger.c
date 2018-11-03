/* libtyger.c: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-31
 * Version:   $Id: libtyger.c 151 2018-11-03 19:54:39Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>
#include <iconv.h>
#include <errno.h>

#include "libtyger.h"

static char *indent_string = NULL;
static int   indent_length = 0;

#define SWAP(from, to) do { char temp = to; to = from; from = temp; } while (0)

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
    static char *buffer = NULL;
    static int   available = -1;

    if (indent_string == NULL) {
        setIndent("    ");
    }

    int required = level * indent_length;

    if (required > available) {
        int i;

        buffer = realloc(buffer, required + 1);

        available = required;

        for (i = 0; i < available; i++) {
            buffer[i] = indent_string[i % indent_length];
        }

        buffer[i] = '\0';
    }

    return buffer + available - level * indent_length;
}

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

static void *memdup(const void *src, unsigned int size)
{
    void *dup = malloc(size);

    memcpy(dup, src, size);

    return dup;
}

static void reverse(char *data, size_t size)
{
    int i;

    for (i = 0; i < size / 2; i++) {
        SWAP(data[i], data[size - 1 - i]);
    }
}

static size_t read_FD_BE(int fd, void *data, size_t size)
{
    size_t count = read_FD(fd, data, size);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse(data, size);
#endif

    return count;
}

static size_t write_FD_BE(int fd, const void *data, size_t size)
{
    char *dup = memdup(data, size);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse(dup, size);
#endif

    size_t count = write_FD(fd, dup, size);

    free(dup);

    return count;
}

static size_t read_FP_BE(FILE *fp, void *data, size_t size)
{
    size_t count = read_FP(fp, data, size);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse(data, size);
#endif

    return count;
}

static size_t write_FP_BE(FILE *fp, const void *data, size_t size)
{
    char *dup = memdup(data, size);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse(dup, size);
#endif

    size_t count = write_FP(fp, dup, size);

    free(dup);

    return count;
}

/*
 * Resize <buffer>, which currently has size <size>, so that it can hold at
 * least <requirement> bytes.
 *
 * If the size is insufficient it will be doubled until it is. If it is
 * currently 0, it will be initialized to 1024.
 */
static void size_buffer(char **buffer, size_t *size, size_t requirement)
{
    while (*size < requirement) {
        *size = (*size == 0) ? 1024 : 2 * *size;
        *buffer = realloc(*buffer, *size);
    }
}

/*
 * Clear the contents of <data>.
 */
void astringClear(char **data)
{
    free(*data);

    *data = NULL;
}

/*
 * Destroy <data>.
 */
void astringDestroy(char **data)
{
    astringClear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the astring pointed to by <data>.
 */
size_t astringPackSize(const char *const *data)
{
    return (*data == NULL) ? 4 : 4 + strlen(*data);
}

/*
 * Unpack an astring from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t astringUnpack(const char *buffer, size_t size, char **data)
{
    uint32_t length;

    if (size < sizeof(uint32_t)) {
        return sizeof(uint32_t);
    }

    uint32Unpack(buffer, size, &length);

    buffer += sizeof(uint32_t);

    if (size < sizeof(uint32_t) + length * sizeof(char)) {
        return sizeof(uint32_t) + length * sizeof(char);
    }

    if (*data != NULL) free(*data);

    *data = calloc(length + 1, sizeof(char));

    memcpy(*data, buffer, length * sizeof(char));

    return sizeof(uint32_t) + length * sizeof(char);
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t astringPack(const char *const *data, char **buffer, size_t *size, size_t *pos)
{
    size_t byte_count = 0;
    uint32_t str_len = strlen(*data);

    byte_count += uint32Pack(&str_len, buffer, size, pos);

    size_buffer(buffer, size, *pos + str_len);

    memcpy(*buffer + *pos, *data, str_len);

    *pos += str_len;

    return sizeof(uint32_t) + str_len;
}

/*
 * Read an astring from <fd> into <data).
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
 * Write an astring to <fd> from <data).
 */
size_t astringWriteToFD(int fd, const char *const *data)
{
    size_t count = 0;
    uint32_t length = (*data == NULL) ? 0 : strlen(*data);

    count += uint32WriteToFD(fd, &length);

    count += write_FD(fd, *data, length);

    return count;
}

/*
 * Read an astring from <fp> into <data).
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
 * Write an astring to <fp> from <data).
 */
size_t astringWriteToFP(FILE *fp, const char *const *data)
{
    size_t count = 0;
    uint32_t length = (*data == NULL) ? 0 : strlen(*data);

    count += uint32WriteToFP(fp, &length);

    count += write_FP(fp, *data, length);

    return count;
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void astringPrint(FILE *fp, const char *const *data, int indent)
{
    fprintf(fp, "\"%s\"", *data);
}

/*
 * Copy string <src> to <dst>.
 */
void astringCopy(char **dst, const char *const *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    if (*dst != NULL) {
        free(*dst);
        *dst = NULL;
    }

    if (*src != NULL) {
        *dst = strdup(*src);
    }
}

/*
 * Clear the contents of <data>.
 */
void ustringClear(wchar_t **data)
{
    free(*data);

    *data = NULL;
}

/*
 * Destroy <data>.
 */
void ustringDestroy(wchar_t **data)
{
    ustringClear(data);

    free(data);
}

/*
 * Convert the <count> wide characters pointed to by <in> to UTF-8 characters.
 * Returns a pointer to a statically allocated buffer containing the UTF-8 text,
 * and returns the number of bytes contained therein through <size>.
 */
static const char *wchar_to_utf8(const wchar_t *in, size_t count, uint32_t *size)
{
    static iconv_t wchar_to_utf8_conv = NULL;

    static char *out = NULL;
    static size_t out_size = 16;

    size_t inbytesleft;
    size_t outbytesleft;

    if (wchar_to_utf8_conv == NULL) {
        if ((wchar_to_utf8_conv = iconv_open("UTF-8", "wchar_t")) == (iconv_t) -1) {
            perror("wchar_to_utf8: iconv_open");
            wchar_to_utf8_conv = NULL;
        }
    }

    if (out == NULL) {
        out = calloc(1, out_size);
    }
    else {
        memset(out, 0, out_size);
    }

    size_t in_size = sizeof(wchar_t) * count;

    while (1) {
        inbytesleft = in_size;
        outbytesleft = out_size;

#ifdef __WIN32
        const
#endif
        char *in_ptr = (char *) in;
        char *out_ptr = out;

        size_t r = iconv(wchar_to_utf8_conv,
                &in_ptr, &inbytesleft,
                &out_ptr, &outbytesleft);

        if (r != -1) {
            break;
        }
        else if (errno == E2BIG) {
            out_size *= 2;
            out = realloc(out, out_size);
        }
        else {
            return NULL;
        }
    }

    *size = out_size - outbytesleft;

    return out;
}

/*
 * Convert the <count> UTF-8 bytes pointed to by <in> to wide characters.
 * Returns a pointer to a statically allocated buffer that contains the wide
 * characters, and returns the number of characters contained therein through
 * <size>.
 */
static const wchar_t *utf8_to_wchar(char *in, size_t count, size_t *size)
{
    static iconv_t utf8_to_wchar_conv = NULL;

    static wchar_t *out = NULL;
    static size_t out_size = 16;

    size_t inbytesleft;
    size_t outbytesleft;

    if (utf8_to_wchar_conv == NULL) {
        if ((utf8_to_wchar_conv = iconv_open("wchar_t", "UTF-8")) == (iconv_t) -1) {
            perror("utf8_to_wchar: iconv_open");
            utf8_to_wchar_conv = NULL;
        }
    }

    if (out == NULL) {
        out = calloc(1, out_size);
    }
    else {
        memset(out, 0, out_size);
    }

    size_t in_size = sizeof(char) * count;

    while (1) {
        inbytesleft = in_size;
        outbytesleft = out_size;

#ifdef __WIN32
        const
#endif
        char *in_ptr = in;
        char *out_ptr = (char *) out;

        size_t r = iconv(utf8_to_wchar_conv,
                &in_ptr, &inbytesleft,
                &out_ptr, &outbytesleft);

        if (r != -1) {
            break;
        }
        else if (errno == E2BIG) {
            out_size *= 2;
            out = realloc(out, out_size);
        }
        else {
            return NULL;
        }
    }

    *size = (out_size - outbytesleft) / sizeof(wchar_t);

    return out;
}

/*
 * Return the number of bytes required to pack the ustring pointed to by <data>.
 */
size_t ustringPackSize(const wchar_t *const *data)
{
    uint32_t out_size;

    if (*data == NULL) {
        out_size = 0;
    }
    else {
        wchar_to_utf8(*data, wcslen(*data), &out_size);
    }

    return sizeof(uint32_t) + out_size;
}

/*
 * Unpack a ustring from <buffer> (which has size <size>) and put it at
 * the address pointed to by <data>.
 */
size_t ustringUnpack(const char *buffer, size_t size, wchar_t **data)
{
    size_t wchar_count;
    uint32_t length;

    if (size < sizeof(uint32_t)) {
        return sizeof(uint32_t);
    }

    uint32Unpack(buffer, size, &length);

    buffer += sizeof(uint32_t);
    size -= sizeof(uint32_t);

    if (size < length) {
        return sizeof(uint32_t) + length;
    }

    const wchar_t *wchar = utf8_to_wchar((char *) buffer, length, &wchar_count);

    if (*data != NULL) free(*data);

    *data = calloc(wchar_count + 1, sizeof(wchar_t));

    memcpy(*data, wchar, wchar_count * sizeof(wchar_t));

    return sizeof(uint32_t) + length;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t ustringPack(const wchar_t *const *data, char **buffer, size_t *size, size_t *pos)
{
    uint32_t utf8_size;
    const char *utf8_text;

    if (data == NULL) {
        utf8_size = 0;
    }
    else {
        utf8_text = wchar_to_utf8(*data, wcslen(*data), &utf8_size);
    }

    uint32Pack(&utf8_size, buffer, size, pos);

    size_buffer(buffer, size, *pos + utf8_size);

    memcpy(*buffer + *pos, utf8_text, utf8_size);

    *pos += utf8_size;

    return sizeof(uint32_t) + utf8_size;
}

/*
 * Read a ustring from <fd> into <data).
 */
size_t ustringReadFromFD(int fd, wchar_t **data)
{
    size_t wchar_count, count = 0;
    uint32_t uint8_len;

    count += uint32ReadFromFD(fd, &uint8_len);

    char *uint8_buf = calloc(1, uint8_len + 1);

    count += read_FD(fd, uint8_buf, uint8_len);

    const wchar_t *wchar = utf8_to_wchar(uint8_buf, uint8_len, &wchar_count);

    if (*data != NULL) free(*data);

    *data = calloc(wchar_count + 1, sizeof(wchar_t));

    memcpy(*data, wchar, (wchar_count + 1) * sizeof(wchar_t));

    return count;
}

/*
 * Write a ustring to <fd> from <data).
 */
size_t ustringWriteToFD(int fd, const wchar_t *const *data)
{
    uint32_t count = 0;
    uint32_t utf8_size;
    const char *utf8_text;

    if (*data == NULL) {
        utf8_size = 0;
    }
    else {
        utf8_text = wchar_to_utf8(*data, wcslen(*data), &utf8_size);
    }

    count += uint32WriteToFD(fd, &utf8_size);

    count += write_FD(fd, utf8_text, utf8_size);

    return count;
}

/*
 * Read a ustring from <fp> into <data).
 */
size_t ustringReadFromFP(FILE *fp, wchar_t **data)
{
    size_t wchar_count, count = 0;
    uint32_t uint8_len;

    count += uint32ReadFromFP(fp, &uint8_len);

    char *uint8_buf = calloc(1, uint8_len + 1);

    count += read_FP(fp, uint8_buf, uint8_len);

    const wchar_t *wchar = utf8_to_wchar(uint8_buf, uint8_len, &wchar_count);

    if (*data != NULL) free(*data);

    *data = calloc(wchar_count + 1, sizeof(wchar_t));

    memcpy(*data, wchar, (wchar_count + 1) * sizeof(wchar_t));

    return count;
}

/*
 * Write a ustring to <fp> from <data).
 */
size_t ustringWriteToFP(FILE *fp, const wchar_t **data)
{
    uint32_t count = 0;
    uint32_t utf8_size;
    const char *utf8_text;

    if (*data == NULL) {
        utf8_size = 0;
    }
    else {
        utf8_text = wchar_to_utf8(*data, wcslen(*data), &utf8_size);
    }

    count += uint32WriteToFP(fp, &utf8_size);

    count += write_FP(fp, utf8_text, utf8_size);

    return count;
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void ustringPrint(FILE *fp, const wchar_t *const *data, int indent)
{
    fprintf(fp, "\"%ls\"", *data);
}

/*
 * Copy string <src> to <dst>.
 */
void ustringCopy(wchar_t **dst, const wchar_t *const *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    if (*dst != NULL) {
        free(*dst);
        *dst = NULL;
    }

    if (*src != NULL) {
        *dst = wcsdup(*src);
    }
}

/*
 * Clear the contents of <data>.
 */
void float32Clear(float *data)
{
    *data = 0.0;
}

/*
 * Destroy <data>.
 */
void float32Destroy(float *data)
{
    float32Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the float pointed to by <data>.
 */
size_t float32PackSize(const float *data)
{
    return sizeof(*data);
}

/*
 * Unpack a float from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t float32Unpack(const char *buffer, size_t size, float *data)
{
    size_t req = float32PackSize(data);

    union {
        float f;
        char c[4];
    } u;

    if (size >= req) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        u.c[3] = buffer[0];
        u.c[2] = buffer[1];
        u.c[1] = buffer[2];
        u.c[0] = buffer[3];
#else
        memcpy(u.c, buffer, 4);
#endif
    }

    *data = u.f;

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t float32Pack(const float *data, char **buffer, size_t *size, size_t *pos)
{
    union {
        float f;
        char c[4];
    } u;

    u.f = *data;

    size_buffer(buffer, size, *pos + sizeof(float));

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    (*buffer)[(*pos)++] = u.c[3];
    (*buffer)[(*pos)++] = u.c[2];
    (*buffer)[(*pos)++] = u.c[1];
    (*buffer)[(*pos)++] = u.c[0];
#else
    memcpy(buffer, u.c, 4);
#endif

    return sizeof(float);
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
size_t float32WriteToFD(int fd, const float *data)
{
    return write_FD_BE(fd, data, sizeof(float));
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
size_t float32WriteToFP(FILE *fp, const float *data)
{
    return write_FP_BE(fp, data, sizeof(float));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float32Print(FILE *fp, const float *data, int indent)
{
    fprintf(fp, "%g", *data);
}

/*
 * Copy <src> to <dst>.
 */
void float32Copy(float *dst, const float *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void float64Clear(double *data)
{
    *data = 0.0;
}

/*
 * Destroy <data>.
 */
void float64Destroy(double *data)
{
    float64Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the double pointed to by <data>.
 */
size_t float64PackSize(const double *data)
{
    return sizeof(*data);
}

/*
 * Unpack a double from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t float64Unpack(const char *buffer, size_t size, double *data)
{
    size_t req = float64PackSize(data);

    union {
        double f;
        char c[8];
    } u;

    if (size >= req) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        u.c[7] = buffer[0];
        u.c[6] = buffer[1];
        u.c[5] = buffer[2];
        u.c[4] = buffer[3];
        u.c[3] = buffer[4];
        u.c[2] = buffer[5];
        u.c[1] = buffer[6];
        u.c[0] = buffer[7];
#else
        memcpy(u.c, buffer, 8);
#endif
    }

    *data = u.f;

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t float64Pack(const double *data, char **buffer, size_t *size, size_t *pos)
{
    union {
        double f;
        char c[8];
    } u;

    u.f = *data;

    size_buffer(buffer, size, *pos + sizeof(double));

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    (*buffer)[(*pos)++] = u.c[7];
    (*buffer)[(*pos)++] = u.c[6];
    (*buffer)[(*pos)++] = u.c[5];
    (*buffer)[(*pos)++] = u.c[4];
    (*buffer)[(*pos)++] = u.c[3];
    (*buffer)[(*pos)++] = u.c[2];
    (*buffer)[(*pos)++] = u.c[1];
    (*buffer)[(*pos)++] = u.c[0];
#else
    memcpy(buffer, u.c, 4);
#endif

    return sizeof(double);
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
size_t float64WriteToFD(int fd, const double *data)
{
    return write_FD_BE(fd, data, sizeof(double));
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
size_t float64WriteToFP(FILE *fp, const double *data)
{
    return write_FP_BE(fp, data, sizeof(double));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float64Print(FILE *fp, const double *data, int indent)
{
    fprintf(fp, "%g", *data);
}

/*
 * Copy <src> to <dst>.
 */
void float64Copy(double *dst, const double *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void uint8Clear(uint8_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void uint8Destroy(uint8_t *data)
{
    uint8Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the uint8_t pointed to by <data>.
 */
size_t uint8PackSize(const uint8_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack a uint8_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint8Unpack(const char *buffer, size_t size, uint8_t *data)
{
    size_t req = uint8PackSize(data);

    if (size >= req) {
        *data = buffer[0];
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint8Pack(const uint8_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(uint8_t));

    (*buffer)[(*pos)++] = *data;

    return sizeof(uint8_t);
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
size_t uint8WriteToFD(int fd, const uint8_t *data)
{
    return write_FD(fd, data, sizeof(uint8_t));
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
size_t uint8WriteToFP(FILE *fp, const uint8_t *data)
{
    return write_FP(fp, data, sizeof(uint8_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint8Print(FILE *fp, const uint8_t *data, int indent)
{
    fprintf(fp, "%" PRIu8, *data);
}

/*
 * Copy <src> to <dst>.
 */
void uint8Copy(uint8_t *dst, const uint8_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void int8Clear(int8_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void int8Destroy(int8_t *data)
{
    int8Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the int8_t pointed to by <data>.
 */
size_t int8PackSize(const int8_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack an int8_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int8Unpack(const char *buffer, size_t size, int8_t *data)
{
    size_t req = int8PackSize(data);

    if (size >= req) {
        *data = buffer[0];
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int8Pack(const int8_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(int8_t));

    (*buffer)[(*pos)++] = *data;

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
size_t int8WriteToFD(int fd, const int8_t *data)
{
    return write_FD(fd, data, sizeof(int8_t));
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
size_t int8WriteToFP(FILE *fp, const int8_t *data)
{
    return write_FP(fp, data, sizeof(int8_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int8Print(FILE *fp, const int8_t *data, int indent)
{
    fprintf(fp, "%" PRId8, *data);
}

/*
 * Copy <src> to <dst>.
 */
void int8Copy(int8_t *dst, const int8_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void uint16Clear(uint16_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void uint16Destroy(uint16_t *data)
{
    uint16Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the uint16_t pointed to by <data>.
 */
size_t uint16PackSize(const uint16_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack a uint16_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint16Unpack(const char *buffer, size_t size, uint16_t *data)
{
    size_t req = uint16PackSize(data);

    if (size >= req) {
        *data = (buffer[0] << 8)
              | (buffer[1]);
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint16Pack(const uint16_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(uint16_t));

    (*buffer)[(*pos)++] = (*data & 0xFF00) >> 8;
    (*buffer)[(*pos)++] = (*data & 0x00FF);

    return sizeof(uint16_t);
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
size_t uint16WriteToFD(int fd, const uint16_t *data)
{
    return write_FD_BE(fd, data, sizeof(uint16_t));
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
size_t uint16WriteToFP(FILE *fp, const uint16_t *data)
{
    return write_FP_BE(fp, data, sizeof(uint16_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint16Print(FILE *fp, const uint16_t *data, int indent)
{
    fprintf(fp, "%" PRIu16, *data);
}

/*
 * Copy <src> to <dst>.
 */
void uint16Copy(uint16_t *dst, const uint16_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void int16Clear(int16_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void int16Destroy(int16_t *data)
{
    int16Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the int16_t pointed to by <data>.
 */
size_t int16PackSize(const int16_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack an int16_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int16Unpack(const char *buffer, size_t size, int16_t *data)
{
    size_t req = int16PackSize(data);

    if (size >= req) {
        *data = (buffer[0] << 8)
              | (buffer[1]);
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int16Pack(const int16_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(int16_t));

    (*buffer)[(*pos)++] = (*data & 0xFF00) >> 8;
    (*buffer)[(*pos)++] = (*data & 0x00FF);

    return sizeof(int16_t);
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
size_t int16WriteToFD(int fd, const int16_t *data)
{
    return write_FD_BE(fd, data, sizeof(int16_t));
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
size_t int16WriteToFP(FILE *fp, const int16_t *data)
{
    return write_FP_BE(fp, data, sizeof(int16_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int16Print(FILE *fp, const int16_t *data, int indent)
{
    fprintf(fp, "%" PRId16, *data);
}

/*
 * Copy <src> to <dst>.
 */
void int16Copy(int16_t *dst, const int16_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void uint32Clear(uint32_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void uint32Destroy(uint32_t *data)
{
    uint32Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the uint32_t pointed to by <data>.
 */
size_t uint32PackSize(const uint32_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack a uint32_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint32Unpack(const char *buffer, size_t size, uint32_t *data)
{
    size_t req = uint32PackSize(data);

    if (size >= req) {
        *data = (buffer[0] << 24)
              | (buffer[1] << 16)
              | (buffer[2] <<  8)
              | (buffer[3]);
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint32Pack(const uint32_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(uint32_t));

    (*buffer)[(*pos)++] = (*data >> 24) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 16) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 8)  & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 0)  & 0xFF;

    return sizeof(uint32_t);
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
size_t uint32WriteToFD(int fd, const uint32_t *data)
{
    return write_FD_BE(fd, data, sizeof(uint32_t));
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
size_t uint32WriteToFP(FILE *fp, const uint32_t *data)
{
    return write_FP_BE(fp, data, sizeof(uint32_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint32Print(FILE *fp, const uint32_t *data, int indent)
{
    fprintf(fp, "%" PRIu32, *data);
}

/*
 * Copy <src> to <dst>.
 */
void uint32Copy(uint32_t *dst, const uint32_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void int32Clear(int32_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void int32Destroy(int32_t *data)
{
    int32Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the int32_t pointed to by <data>.
 */
size_t int32PackSize(const int32_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack an int32_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int32Unpack(const char *buffer, size_t size, int32_t *data)
{
    size_t req = int32PackSize(data);

    if (size >= req) {
        *data = (buffer[0] << 24)
              | (buffer[1] << 16)
              | (buffer[2] <<  8)
              | (buffer[3]);
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int32Pack(const int32_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(int32_t));

    (*buffer)[(*pos)++] = (*data >> 24) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 16) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 8)  & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 0)  & 0xFF;

    return sizeof(uint32_t);
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
size_t int32WriteToFD(int fd, const int32_t *data)
{
    return write_FD_BE(fd, data, sizeof(int32_t));
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
size_t int32WriteToFP(FILE *fp, const int32_t *data)
{
    return write_FP_BE(fp, data, sizeof(int32_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int32Print(FILE *fp, const int32_t *data, int indent)
{
    fprintf(fp, "%" PRId32, *data);
}

/*
 * Copy <src> to <dst>.
 */
void int32Copy(int32_t *dst, const int32_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void uint64Clear(uint64_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void uint64Destroy(uint64_t *data)
{
    uint64Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the uint64_t pointed to by <data>.
 */
size_t uint64PackSize(const uint64_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack a uint64_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint64Unpack(const char *buffer, size_t size, uint64_t *data)
{
    size_t req = uint64PackSize(data);

    if (size >= req) {
        *data = ((uint64_t) buffer[0] << 56)
              | ((uint64_t) buffer[1] << 48)
              | ((uint64_t) buffer[2] << 40)
              | ((uint64_t) buffer[3] << 32)
              | ((uint64_t) buffer[4] << 24)
              | ((uint64_t) buffer[5] << 16)
              | ((uint64_t) buffer[6] <<  8)
              | ((uint64_t) buffer[7]);
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint64Pack(const uint64_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(uint64_t));

    (*buffer)[(*pos)++] = (*data >> 56) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 48) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 40) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 32) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 24) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 16) & 0xFF;
    (*buffer)[(*pos)++] = (*data >>  8) & 0xFF;
    (*buffer)[(*pos)++] = (*data >>  0) & 0xFF;

    return sizeof(uint64_t);
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
size_t uint64WriteToFD(int fd, const uint64_t *data)
{
    return write_FD_BE(fd, data, sizeof(uint64_t));
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
size_t uint64WriteToFP(FILE *fp, const uint64_t *data)
{
    return write_FP_BE(fp, data, sizeof(uint64_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint64Print(FILE *fp, const uint64_t *data, int indent)
{
    fprintf(fp, "%" PRIu64, *data);
}

/*
 * Copy <src> to <dst>.
 */
void uint64Copy(uint64_t *dst, const uint64_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

/*
 * Clear the contents of <data>.
 */
void int64Clear(int64_t *data)
{
    *data = 0;
}

/*
 * Destroy <data>.
 */
void int64Destroy(int64_t *data)
{
    int64Clear(data);

    free(data);
}

/*
 * Return the number of bytes required to pack the int64_t pointed to by <data>.
 */
size_t int64PackSize(const int64_t *data)
{
    return sizeof(*data);
}

/*
 * Unpack an int64_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int64Unpack(const char *buffer, size_t size, int64_t *data)
{
    size_t req = int64PackSize(data);

    if (size >= req) {
        *data = ((uint64_t) buffer[0] << 56)
              | ((uint64_t) buffer[1] << 48)
              | ((uint64_t) buffer[2] << 40)
              | ((uint64_t) buffer[3] << 32)
              | ((uint64_t) buffer[4] << 24)
              | ((uint64_t) buffer[5] << 16)
              | ((uint64_t) buffer[6] <<  8)
              | ((uint64_t) buffer[7]);
    }

    return req;
}

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int64Pack(const int64_t *data, char **buffer, size_t *size, size_t *pos)
{
    size_buffer(buffer, size, *pos + sizeof(int64_t));

    (*buffer)[(*pos)++] = (*data >> 56) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 48) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 40) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 32) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 24) & 0xFF;
    (*buffer)[(*pos)++] = (*data >> 16) & 0xFF;
    (*buffer)[(*pos)++] = (*data >>  8) & 0xFF;
    (*buffer)[(*pos)++] = (*data >>  0) & 0xFF;

    return sizeof(int64_t);
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
size_t int64WriteToFD(int fd, const int64_t *data)
{
    return write_FD_BE(fd, data, sizeof(int64_t));
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
size_t int64WriteToFP(FILE *fp, const int64_t *data)
{
    return write_FP_BE(fp, data, sizeof(int64_t));
}

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int64Print(FILE *fp, const int64_t *data, int indent)
{
    fprintf(fp, "%" PRId64, *data);
}

/*
 * Copy <src> to <dst>.
 */
void int64Copy(int64_t *dst, const int64_t *src)
{
    assert(dst != NULL);
    assert(src != NULL);

    *dst = *src;
}

#ifdef TEST
static void reset_buffer(char **buffer, size_t *size, size_t *pos)
{
    if (*buffer != NULL) {
        free(*buffer);
        *buffer = NULL;
    }

    *size = 0;
    *pos = 0;
}

int main(int argc, char *argv[])
{
    char *astring_data;
    wchar_t *ustring_data;

    float float32_data;
    double float64_data;

    uint8_t  uint8_data;
    int8_t   int8_data;
    uint16_t uint16_data;
    int16_t  int16_data;
    uint32_t uint32_data;
    int32_t  int32_data;
    uint64_t uint64_data;
    int64_t  int64_data;

    char *buffer = NULL;
    size_t size = 0, pos = 0;

    astring_data = strdup("Hoi");

    assert(astringPackSize((const char **) &astring_data) == 7);
    astringPack((const char **) &astring_data, &buffer, &size, &pos);
    assert(pos == 7);
    assert(size >= 7);
    assert(memcmp(buffer, "\x00\x00\x00\x03Hoi", 7) == 0);
    astringUnpack(buffer, pos, &astring_data);
    assert(strcmp(astring_data, "Hoi") == 0);

    reset_buffer(&buffer, &size, &pos);

    ustring_data = wcsdup(L"αß¢");

    assert(ustringPackSize((const wchar_t **) &ustring_data) == 10);
    ustringPack((const wchar_t **) &ustring_data, &buffer, &size, &pos);
    assert(pos == 10);
    assert(size >= 10);
    assert(memcmp(buffer, "\x00\x00\x00\x06\xCE\xB1\xC3\x9F\xC2\xA2", 10) == 0);
    ustringUnpack(buffer, pos, &ustring_data);
    assert(wcscmp(ustring_data, L"αß¢") == 0);

    reset_buffer(&buffer, &size, &pos);

    float32_data = 1.0;

    float32Pack(&float32_data, &buffer, &size, &pos);
    float32Unpack(buffer, pos, &float32_data);
    assert(float32PackSize(&float32_data) == 4);
    assert(memcmp(buffer, "\x3F\x80\x00\x00", 4) == 0);
    assert(float32_data == 1.0);

    reset_buffer(&buffer, &size, &pos);

    float64_data = 2.0;
    float64Pack(&float64_data, &buffer, &size, &pos);
    float64Unpack(buffer, pos, &float64_data);
    assert(float64PackSize(&float64_data) == 8);
    assert(memcmp(buffer, "\x40\x00\x00\x00\x00\x00\x00\x00", 8) == 0);
    assert(float64_data == 2.0);

    reset_buffer(&buffer, &size, &pos);

    uint8_data = 8;
    uint8Pack(&uint8_data, &buffer, &size, &pos);
    uint8Unpack(buffer, pos, &uint8_data);
    assert(memcmp(buffer, "\x08", 1) == 0);
    assert(uint8_data == 8);

    reset_buffer(&buffer, &size, &pos);

    int8_data = 8;
    int8Pack(&int8_data, &buffer, &size, &pos);
    int8Unpack(buffer, pos, &int8_data);
    assert(memcmp(buffer, "\x08", 1) == 0);
    assert(int8_data == 8);

    reset_buffer(&buffer, &size, &pos);

    uint8_data = -8;
    uint8Pack(&uint8_data, &buffer, &size, &pos);
    uint8Unpack(buffer, pos, &uint8_data);
    assert(memcmp(buffer, "\xF8", 1) == 0);
    assert(uint8_data == (256 - 8));

    reset_buffer(&buffer, &size, &pos);

    int8_data = -8;
    int8Pack(&int8_data, &buffer, &size, &pos);
    int8Unpack(buffer, pos, &int8_data);
    assert(memcmp(buffer, "\xF8", 1) == 0);
    assert(int8_data == -8);

    reset_buffer(&buffer, &size, &pos);

    uint16_data = 16;
    uint16Pack(&uint16_data, &buffer, &size, &pos);
    uint16Unpack(buffer, pos, &uint16_data);
    assert(memcmp(buffer, "\x00\x10", 2) == 0);
    assert(uint16_data == 16);

    reset_buffer(&buffer, &size, &pos);

    int16_data = 16;
    int16Pack(&int16_data, &buffer, &size, &pos);
    int16Unpack(buffer, pos, &int16_data);
    assert(memcmp(buffer, "\x00\x10", 2) == 0);
    assert(int16_data == 16);

    reset_buffer(&buffer, &size, &pos);

    uint16_data = -16;
    uint16Pack(&uint16_data, &buffer, &size, &pos);
    uint16Unpack(buffer, pos, &uint16_data);
    assert(memcmp(buffer, "\xFF\xF0", 2) == 0);
    assert(uint16_data == (65536 - 16));

    reset_buffer(&buffer, &size, &pos);

    int16_data = -16;
    int16Pack(&int16_data, &buffer, &size, &pos);
    int16Unpack(buffer, pos, &int16_data);
    assert(memcmp(buffer, "\xFF\xF0", 2) == 0);
    assert(int16_data == -16);

    reset_buffer(&buffer, &size, &pos);

    uint32_data = 32;
    uint32Pack(&uint32_data, &buffer, &size, &pos);
    uint32Unpack(buffer, pos, &uint32_data);
    assert(memcmp(buffer, "\x00\x00\x00\x20", 4) == 0);
    assert(uint32_data == 32);

    reset_buffer(&buffer, &size, &pos);

    int32_data = 32;
    int32Pack(&int32_data, &buffer, &size, &pos);
    int32Unpack(buffer, pos, &int32_data);
    assert(memcmp(buffer, "\x00\x00\x00\x20", 4) == 0);
    assert(int32_data == 32);

    reset_buffer(&buffer, &size, &pos);

    uint32_data = -32;
    uint32Pack(&uint32_data, &buffer, &size, &pos);
    uint32Unpack(buffer, pos, &uint32_data);
    assert(memcmp(buffer, "\xFF\xFF\xFF\xE0", 4) == 0);
    assert(uint32_data == (0x100000000L - 32));

    reset_buffer(&buffer, &size, &pos);

    int32_data = -32;
    int32Pack(&int32_data, &buffer, &size, &pos);
    int32Unpack(buffer, pos, &int32_data);
    assert(memcmp(buffer, "\xFF\xFF\xFF\xE0", 4) == 0);
    assert(int32_data == -32);

    reset_buffer(&buffer, &size, &pos);

    uint64_data = 64;
    uint64Pack(&uint64_data, &buffer, &size, &pos);
    uint64Unpack(buffer, pos, &uint64_data);
    assert(memcmp(buffer, "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);
    assert(uint64_data == 64);

    reset_buffer(&buffer, &size, &pos);

    int64_data = 64;
    int64Pack(&int64_data, &buffer, &size, &pos);
    int64Unpack(buffer, pos, &int64_data);
    assert(memcmp(buffer, "\x00\x00\x00\x00\x00\x00\x00\x40", 8) == 0);
    assert(int64_data == 64);

    reset_buffer(&buffer, &size, &pos);

    uint64_data = -64;
    uint64Pack(&uint64_data, &buffer, &size, &pos);
    uint64Unpack(buffer, pos, &uint64_data);
    assert(memcmp(buffer, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);
    assert(uint64_data == (0xFFFFFFFFFFFFFFFF - 63));

    reset_buffer(&buffer, &size, &pos);

    int64_data = -64;
    int64Pack(&int64_data, &buffer, &size, &pos);
    int64Unpack(buffer, pos, &int64_data);
    assert(memcmp(buffer, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0", 8) == 0);
    assert(int64_data == -64);

    reset_buffer(&buffer, &size, &pos);

    uint32_data = 256;

    uint32Pack(&uint32_data, &buffer, &size, &pos);

    assert(buffer != NULL);
    assert(size >= 4);
    assert(pos == 4);
    assert(memcmp(buffer, "\x00\x00\x01\x00", 4) == 0);

    astringPack((const char **) &astring_data, &buffer, &size, &pos);

    assert(buffer != NULL);
    assert(size >= 11);
    assert(pos == 11);
    assert(memcmp(buffer, "\x00\x00\x01\x00\x00\x00\x00\x03" "Hoi", 11) == 0);

    ustringPack((const wchar_t **) &ustring_data, &buffer, &size, &pos);

    assert(buffer != NULL);
    assert(size >= 21);
    assert(pos == 21);
    assert(memcmp(buffer,
                "\x00\x00\x01\x00\x00\x00\x00\x03" "Hoi"
                "\x00\x00\x00\x06\xCE\xB1\xC3\x9F\xC2\xA2", 21) == 0);

    return 0;
}
#endif
