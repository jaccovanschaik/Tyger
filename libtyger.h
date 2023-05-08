#ifndef LIBTYGER_H
#define LIBTYGER_H

/* libtyger.h: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
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
extern void (*astringClear)(astring *);
extern void (*wstringClear)(wstring *);
extern void (*astringDestroy)(astring *);
extern void (*wstringDestroy)(wstring *);

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
void uint8Clear(uint8_t *data);

/*
 * Clear the contents of <data>.
 */
void uint16Clear(uint16_t *data);

/*
 * Clear the contents of <data>.
 */
void uint32Clear(uint32_t *data);

/*
 * Clear the contents of <data>.
 */
void uint64Clear(uint64_t *data);

/*
 * Clear the contents of <data>.
 */
void int8Clear(int8_t *data);

/*
 * Clear the contents of <data>.
 */
void int16Clear(int16_t *data);

/*
 * Clear the contents of <data>.
 */
void int32Clear(int32_t *data);

/*
 * Clear the contents of <data>.
 */
void int64Clear(int64_t *data);

/*
 * Clear the contents of <data>.
 */
void boolClear(bool *data);

/*
 * Clear the contents of <data>.
 */
void float32Clear(float *data);

/*
 * Clear the contents of <data>.
 */
void float64Clear(double *data);

/* =============================== "Destroy" functions ===============================
 *
 * Destroy the contents of <data>.
 */
void uint8Destroy(uint8_t *data);

/*
 * Destroy the contents of <data>.
 */
void uint16Destroy(uint16_t *data);

/*
 * Destroy the contents of <data>.
 */
void uint32Destroy(uint32_t *data);

/*
 * Destroy the contents of <data>.
 */
void uint64Destroy(uint64_t *data);

/*
 * Destroy the contents of <data>.
 */
void int8Destroy(int8_t *data);

/*
 * Destroy the contents of <data>.
 */
void int16Destroy(int16_t *data);

/*
 * Destroy the contents of <data>.
 */
void int32Destroy(int32_t *data);

/*
 * Destroy the contents of <data>.
 */
void int64Destroy(int64_t *data);

/*
 * Destroy the contents of <data>.
 */
void boolDestroy(bool *data);

/*
 * Destroy the contents of <data>.
 */
void float32Destroy(float *data);

/*
 * Destroy the contents of <data>.
 */
void float64Destroy(double *data);

/* =============================== "PackSize" functions ===============================
 *
 * Return the number of bytes required to pack a bool.
 */
size_t boolPackSize(void);

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t uint8PackSize(void);

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t uint16PackSize(void);

/*
 * Return the number of bytes required to pack a uint32_t.
 */
size_t uint32PackSize(void);

/*
 * Return the number of bytes required to pack a uint64_t.
 */
size_t uint64PackSize(void);

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t int8PackSize(void);

/*
 * Return the number of bytes required to pack a int8_t.
 */
size_t int16PackSize(void);

/*
 * Return the number of bytes required to pack a int32_t.
 */
size_t int32PackSize(void);

/*
 * Return the number of bytes required to pack a int64_t.
 */
size_t int64PackSize(void);

/*
 * Return the number of bytes required to pack a float32.
 */
size_t float32PackSize(void);

/*
 * Return the number of bytes required to pack a float64.
 */
size_t float64PackSize(void);

/*
 * Return the number of bytes required to pack the char *pointed to by <data>.
 */
size_t astringPackSize(const astring *as);

/*
 * Return the number of bytes required to pack the wchar_t *pointed to by <data>.
 */
size_t wstringPackSize(const wstring *ws);

/* =============================== "Pack" functions ===============================
 *
 * Pack the least-significant <num_bytes> of <data> into <buf>, updating
 * <size> and <pos>.
 */
Buffer *uintPack(unsigned int data, size_t num_bytes, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint8Pack(uint8_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint16Pack(uint16_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint32Pack(uint32_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *uint64Pack(uint64_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int8Pack(int8_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int16Pack(int16_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int32Pack(int32_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *int64Pack(int64_t data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *float32Pack(const float data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *float64Pack(const double data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *boolPack(const bool data, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
Buffer *astringPack(const astring *as, Buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
Buffer *wstringPack(const wstring *ws, Buffer *buf);

/* =============================== "Unpack" functions ===============================
 *
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
size_t uintUnpack(size_t num_bytes, const Buffer *buf, size_t pos, unsigned int *data);

/*
 * Unpack a uint8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint8Unpack(const Buffer *buf, size_t pos, uint8_t *data);

/*
 * Unpack a uint16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint16Unpack(const Buffer *buf, size_t pos, uint16_t *data);

/*
 * Unpack a uint32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint32Unpack(const Buffer *buf, size_t pos, uint32_t *data);

/*
 * Unpack a uint64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint64Unpack(const Buffer *buf, size_t pos, uint64_t *data);

/*
 * Unpack an int8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int8Unpack(const Buffer *buf, size_t pos, int8_t *data);

/*
 * Unpack an int16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int16Unpack(const Buffer *buf, size_t pos, int16_t *data);

/*
 * Unpack an int32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int32Unpack(const Buffer *buf, size_t pos, int32_t *data);

/*
 * Unpack an int64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int64Unpack(const Buffer *buf, size_t pos, int64_t *data);

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t boolUnpack(const Buffer *buf, size_t pos, bool *data);

/*
 * Unpack a float (aka. float32) from <buf> (which has size <size>) and put it at <data>. Return the
 * new <pos>.
 */
size_t float32Unpack(const Buffer *buf, size_t pos, float *data);

/*
 * Unpack a double (aka. float64) from <buf> (which has size <size>) and put it at <data>. Return
 * the new <pos>.
 */
size_t float64Unpack(const Buffer *buf, size_t pos, double *data);

/*
 * Unpack a char from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t astringUnpack(const Buffer *buf, size_t pos, astring *data);

/*
 * Unpack a UTF-8 encoded Unicode string from <buf> (which has size <size>), write it to a newly
 * allocated wide-character string whose starting address is written to <wchar_str>, and return the
 * number of bytes consumed from <buf>.
 */
size_t wstringUnpack(const Buffer *buf, size_t pos, wstring *data);

/* =============================== "Copy" functions ===============================
 *
 * Copy string <src> to <dst>.
 */
void astringCopy(astring *dst, const astring *src);

/*
 * Copy string <src> to <dst>.
 */
void wstringCopy(wstring *dst, const wstring *src);

/* =============================== "Print" functions ===============================
 *
 * Print an ASCII representation of <data> to <fp>.
 */
void uint8Print(FILE *fp, uint8_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int8Print(FILE *fp, int8_t data, int indent);

void uint16Print(FILE *fp, uint16_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int16Print(FILE *fp, int16_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint32Print(FILE *fp, uint32_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int32Print(FILE *fp, int32_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint64Print(FILE *fp, uint64_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int64Print(FILE *fp, int64_t data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void boolPrint(FILE *fp, bool data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float32Print(FILE *fp, float data, int indent);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float64Print(FILE *fp, double data, int indent);

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void astringPrint(FILE *fp, const astring *as, int indent);

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void wstringPrint(FILE *fp, const wstring *ws, int indent);

/* =============================== "Dup" functions ===============================
 *
 * Duplicate astring <str>.
 */
astring *astringDup(astring *str);

/*
 * Duplicate wstring <str>.
 */
wstring *wstringDup(wstring *str);

#ifdef __cplusplus
}
#endif

#endif
