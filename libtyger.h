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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *data;      // Buffer data.
    size_t len;         // Number of bytes used.
    size_t cap;         // Number of bytes allocated.
} buffer;

typedef struct {
    char *data;         // String contents.
    size_t len;         // Number of characters used.
    size_t cap;         // Number of characters allocated.
} astring;

typedef struct {
    wchar_t *data;      // String contents.
    size_t len;         // Number of characters used.
    size_t cap;         // Number of characters allocated.
} ustring;

/* =============================== Indent handling ===============================
 *
 * Set the indentation string.
 */
void setIndent(const char *str);

/*
 * Return an indentation string for level <level>.
 */
const char *indent(int level);

/* =============================== "Length" functions ===============================
 *
 * Return the number of used bytes in <pos>, counted from <pos>.
 */
size_t bufferLen(const buffer *buf, size_t pos);

/* =============================== "Get" functions ===============================
 *
 * Get a pointer to the contents of <buf>, starting at <pos>.
 */
const uint8_t *bufferGet(const buffer *buf, size_t pos);

/*
 * Get the byte at <pos> in <buf>.
 */
uint8_t bufferGetC(const buffer *buf, size_t pos);

/*
 * Get a pointer to the contents of <astr>.
 */
const char *astringGet(const astring *astr);

/*
 * Get a pointer to the contents of <astr>.
 */
const wchar_t *ustringGet(const ustring *ustr);

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

/*
 * Clear the contents of <buf>.
 */
void bufferClear(buffer *buf);

/*
 * Clear the contents of <astr>.
 */
astring *astringClear(astring *astr);

/*
 * Clear the contents of <ustr>.
 */
ustring *ustringClear(ustring *ustr);

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

/*
 * Destroy buffer <buf>.
 */
void bufferDestroy(buffer *buf);

/*
 * Destroy astring <astr>.
 */
void astringDestroy(astring *astr);

/*
 * Destroy ustring <ustr>.
 */
void ustringDestroy(ustring *ustr);

/* =============================== "Add" functions ===============================
 *
 * Add <add_data>, which has size <add_size> to <buf>.
 */
buffer *bufferAdd(buffer *buf, const void *add_data, size_t add_size);

/*
 * Add the character <add_data> to <buf>.
 */
buffer *bufferAddC(buffer *buf, uint8_t add_data);

/*
 * Add the string at <data>, which has size <data_len> to <astr>.
 */
astring *astringAdd(astring *astr, const char *data, size_t data_len);

/*
 * Add the string at <data>, which has size <data_len> to <ustr>.
 */
ustring *ustringAdd(ustring *ustr, const wchar_t *data, size_t data_len);

/*
 * Add the zero-terminated string at <data> to <astr>.
 */
astring *astringAddZ(astring *astr, const char *data);

/*
 * Add the zero-terminated string at <data> to <ustr>.
 */
ustring *ustringAddZ(ustring *ustr, const wchar_t *data);

/* =============================== "Rewind" functions ===============================
 *
 * Rewind <astr> back to the beginning.
 */
astring *astringRewind(astring *astr);

/*
 * Rewind <ustr> back to the beginning.
 */
ustring *ustringRewind(ustring *ustr);

/* =============================== "Set" functions ===============================
 *
 * Set <astr> to the string starting at <data>, which has length <data_len>.
 */
astring *astringSet(astring *astr, const char *data, size_t data_len);

/*
 * Set <ustr> to the string starting at <data>, which has length <data_len>.
 */
ustring *ustringSet(ustring *ustr, const wchar_t *data, size_t data_len);

/*
 * Set <astr> to the null-terminated string starting at <data>.
 */
astring *astringSetZ(astring *astr, const char *data);

/*
 * Set <ustr> to the null-terminated string starting at <data>.
 */
ustring *ustringSetZ(ustring *ustr, const wchar_t *data);

/* =============================== "Make" functions ===============================
 *
 * Make a new astring with the given null-terminated string as its original contents and return it
 * by value.
 */
astring astringMake(const char *str);

/*
 * Make a new astring with the given null-terminated string as its original contents and return it
 * by value.
 */
ustring ustringMake(const wchar_t *str);

/* =============================== "Create" functions ===============================
 *
 * Create a new astring with the given null-terminated string as its original contents and return a
 * pointer to it.
 */
astring *astringCreate(const char *str);

/*
 * Create a new ustring with the given null-terminated string as its original contents and return a
 * pointer to it.
 */
ustring *ustringCreate(const wchar_t *str);

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
size_t astringPackSize(const astring *str);

/*
 * Return the number of bytes required to pack the wchar_t *pointed to by <data>.
 */
size_t ustringPackSize(const ustring *str);

/* =============================== "Pack" functions ===============================
 *
 * Pack the least-significant <num_bytes> of <data> into <buf>, updating
 * <size> and <pos>.
 */
buffer *uintPack(unsigned int data, size_t num_bytes, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint8Pack(uint8_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint16Pack(uint16_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint32Pack(uint32_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *uint64Pack(uint64_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int8Pack(int8_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int16Pack(int16_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int32Pack(int32_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int64Pack(int64_t data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *float32Pack(const float data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *float64Pack(const double data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *boolPack(const bool data, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *astringPack(const astring *str, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
buffer *ustringPack(const ustring *str, buffer *buf);

/* =============================== "Unpack" functions ===============================
 *
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
size_t uintUnpack(size_t num_bytes, const buffer *buf, size_t pos, unsigned int *data);

/*
 * Unpack a uint8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint8Unpack(const buffer *buf, size_t pos, uint8_t *data);

/*
 * Unpack a uint16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint16Unpack(const buffer *buf, size_t pos, uint16_t *data);

/*
 * Unpack a uint32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint32Unpack(const buffer *buf, size_t pos, uint32_t *data);

/*
 * Unpack a uint64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t uint64Unpack(const buffer *buf, size_t pos, uint64_t *data);

/*
 * Unpack an int8 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int8Unpack(const buffer *buf, size_t pos, int8_t *data);

/*
 * Unpack an int16 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int16Unpack(const buffer *buf, size_t pos, int16_t *data);

/*
 * Unpack an int32 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int32Unpack(const buffer *buf, size_t pos, int32_t *data);

/*
 * Unpack an int64 from position <pos> in <buf> and put it at <data>. Return the new <pos>.
 */
size_t int64Unpack(const buffer *buf, size_t pos, int64_t *data);

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t boolUnpack(const buffer *buf, size_t pos, bool *data);

/*
 * Unpack a float (aka. float32) from <buf> (which has size <size>) and put it at <data>. Return the
 * new <pos>.
 */
size_t float32Unpack(const buffer *buf, size_t pos, float *data);

/*
 * Unpack a double (aka. float64) from <buf> (which has size <size>) and put it at <data>. Return
 * the new <pos>.
 */
size_t float64Unpack(const buffer *buf, size_t pos, double *data);

/*
 * Unpack an char *from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t astringUnpack(const buffer *buf, size_t pos, astring *data);

/*
 * Unpack a UTF-8 encoded Unicode string from <buf> (which has size <size>), write it to a newly
 * allocated wide-character string whose starting address is written to <wchar_str>, and return the
 * number of bytes consumed from <buf>.
 */
size_t ustringUnpack(const buffer *buf, size_t pos, ustring *data);

/* =============================== "Copy" functions ===============================
 *
 * Copy string <src> to <dst>.
 */
void astringCopy(char **dst, const char *src);

/*
 * Copy string <src> to <dst>.
 */
void ustringCopy(wchar_t **dst, const wchar_t *src);

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
void astringPrint(FILE *fp, const astring *str, int indent);

/*
 * Print an ASCII representation of <str> to <fp>.
 */
void ustringPrint(FILE *fp, const ustring *str, int indent);

#ifdef __cplusplus
}
#endif

#endif
