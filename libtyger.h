#ifndef LIBTYGER_H
#define LIBTYGER_H

/* libtyger.h: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016-2025 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-31
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <libjvs/buffer.h>
#include <libjvs/astring.h>
#include <libjvs/wstring.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* =============================== Aliases ===============================
 *
 * Aliases for functions from libjvs.
 */
extern void (*clear_astring)(astring *);
extern void (*clear_wstring)(wstring *);
extern void (*destroy_astring)(astring *);
extern void (*destroy_wstring)(wstring *);

/* =============================== Indent handling ===============================
 *
 * Set the indentation string.
 */
void setIndent(const char *str);

/*
 * Return an indentation string for level <level>.
 */
const char *indent(int level);

/* =============================== "Clear" functions ===============================
 *
 * Clear the contents of <data>.
 */
void clear_uint8(uint8_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_uint16(uint16_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_uint32(uint32_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_uint64(uint64_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_int8(int8_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_int16(int16_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_int32(int32_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_int64(int64_t *data);

/*
 * Clear the contents of <data>.
 */
void clear_bool(bool *data);

/*
 * Clear the contents of <data>.
 */
void clear_float32(float *data);

/*
 * Clear the contents of <data>.
 */
void clear_float64(double *data);

/* =============================== "Destroy" functions ===============================
 *
 * Destroy the contents of <data>.
 */
void destroy_uint8(uint8_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_uint16(uint16_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_uint32(uint32_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_uint64(uint64_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_int8(int8_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_int16(int16_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_int32(int32_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_int64(int64_t *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_bool(bool *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_float32(float *data);

/*
 * Destroy the contents of <data>.
 */
void destroy_float64(double *data);

/* =============================== "PackSize" functions ===============================
 *
 * Return the number of bytes required to pack a bool.
 */
size_t size_bool(void);

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t size_uint8(void);

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t size_uint16(void);

/*
 * Return the number of bytes required to pack a uint32_t.
 */
size_t size_uint32(void);

/*
 * Return the number of bytes required to pack a uint64_t.
 */
size_t size_uint64(void);

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t size_int8(void);

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t size_int16(void);

/*
 * Return the number of bytes required to pack a int32_t.
 */
size_t size_int32(void);

/*
 * Return the number of bytes required to pack a int64_t.
 */
size_t size_int64(void);

/*
 * Return the number of bytes required to pack a float32.
 */
size_t size_float32(void);

/*
 * Return the number of bytes required to pack a float64.
 */
size_t size_float64(void);

/*
 * Return the number of bytes required to pack the char *pointed to by <data>.
 */
size_t size_astring(const astring *as);

/*
 * Return the number of bytes required to pack the wchar_t *pointed to by <data>.
 */
size_t size_wstring(const wstring *ws);

/* =============================== "Pack" functions ===============================
 *
 * Pack the least-significant <num_bytes> of <data> into <buf>, updating
 * <size> and <pos>.
 */
Buffer *pack_uint(uint64_t data, size_t num_bytes, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint8(uint8_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint16(uint16_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint32(uint32_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_uint64(uint64_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int8(int8_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int16(int16_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int32(int32_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_int64(int64_t data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_float32(const float data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_float64(const double data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_bool(const bool data, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *pack_astring(const astring *as, char **buf, size_t *size, size_t *pos);

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
Buffer *pack_wstring(const wstring *ws, char **buf, size_t *size, size_t *pos);

/* =============================== "Unpack" functions ===============================
 *
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
Buffer *unpack_uint(size_t num_bytes, const char *buf, size_t size, size_t *pos, uint64_t *data);

/*
 * Unpack a uint8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint8(const char *buf, size_t size, size_t *pos, uint8_t *data);

/*
 * Unpack a uint16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint16(const char *buf, size_t size, size_t *pos, uint16_t *data);

/*
 * Unpack a uint32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint32(const char *buf, size_t size, size_t *pos, uint32_t *data);

/*
 * Unpack a uint64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_uint64(const char *buf, size_t size, size_t *pos, uint64_t *data);

/*
 * Unpack an int8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int8(const char *buf, size_t size, size_t *pos, int8_t *data);

/*
 * Unpack an int16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int16(const char *buf, size_t size, size_t *pos, int16_t *data);

/*
 * Unpack an int32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int32(const char *buf, size_t size, size_t *pos, int32_t *data);

/*
 * Unpack an int64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
Buffer *unpack_int64(const char *buf, size_t size, size_t *pos, int64_t *data);

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
Buffer *unpack_bool(const char *buf, size_t size, size_t *pos, bool *data);

/*
 * Unpack a float (aka. float32) from <buf> (which has size <size>) and put it at <data>. Return the
 * new <pos>.
 */
Buffer *unpack_float32(const char *buf, size_t size, size_t *pos, float *data);

/*
 * Unpack a double (aka. float64) from <buf> (which has size <size>) and put it at <data>. Return
 * the new <pos>.
 */
Buffer *unpack_float64(const char *buf, size_t size, size_t *pos, double *data);

/*
 * Unpack a char from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
Buffer *unpack_astring(const char *buf, size_t size, size_t *pos, astring *data);

/*
 * Unpack a UTF-8 encoded Unicode string from <buf> (which has size <size>), write it to a newly
 * allocated wide-character string whose starting address is written to <wchar_str>, and return the
 * number of bytes consumed from <buf>.
 */
Buffer *unpack_wstring(const char *buf, size_t size, size_t *pos, wstring *data);

/* =============================== "Copy" functions ===============================
 *
 * Copy string <src> to <dst>.
 */
void copy_astring(astring *dst, const astring *src);

/*
 * Copy string <src> to <dst>.
 */
void copy_wstring(wstring *dst, const wstring *src);

/* =============================== "Print" functions ===============================
 *
 * Print an ASCII representation of <data> to <fp>.
 */
void print_uint8(FILE *fp, uint8_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int8(FILE *fp, int8_t data, int indent);

void print_uint16(FILE *fp, uint16_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int16(FILE *fp, int16_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_uint32(FILE *fp, uint32_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int32(FILE *fp, int32_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_uint64(FILE *fp, uint64_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_int64(FILE *fp, int64_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_bool(FILE *fp, bool data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_float32(FILE *fp, float data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void print_float64(FILE *fp, double data, int indent);

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void print_astring(FILE *fp, const astring *as, int indent);

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void print_wstring(FILE *fp, const wstring *ws, int indent);

/* =============================== "Dup" functions ===============================
 *
 * Duplicate astring <str>.
 */
astring *dup_astring(astring *str);

/*
 * Duplicate wstring <str>.
 */
wstring *dup_wstring(wstring *str);

#ifdef __cplusplus
}
#endif

#endif
