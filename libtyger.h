#ifndef LIBTYGER_H
#define LIBTYGER_H

/* libtyger.h: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
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

/*
 * Set the indentation string.
 */
void setIndent(const char *str);

/*
 * Return an indentation string for level <level>.
 */
const char *indent(int level);

// =============================== "Length" functions ===============================

size_t bufferLen(const buffer *buf, size_t pos);

// =============================== "Get" functions ===============================

const uint8_t *bufferGet(const buffer *buf, size_t pos);

uint8_t bufferGetC(const buffer *buf, size_t pos);

const char *astringGet(const astring *astr);

const wchar_t *ustringGet(const ustring *ustr);

/*
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

void bufferClear(buffer *buf);

astring *astringClear(astring *astr);

ustring *ustringClear(ustring *ustr);

/*
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

// =============================== "Add" functions ===============================

buffer *bufferAdd(buffer *buf, const void *add_data, size_t add_size);

buffer *bufferAddC(buffer *buf, uint8_t add_data);

astring *astringAdd(astring *astr, const char *data, size_t data_len);

ustring *ustringAdd(ustring *ustr, const wchar_t *data, size_t data_len);

astring *astringAddZ(astring *astr, const char *data);

ustring *ustringAddZ(ustring *ustr, const wchar_t *data);

// =============================== "Rewind" functions ===============================

astring *astringRewind(astring *astr);

ustring *ustringRewind(ustring *ustr);

// =============================== "Set" functions ===============================

astring *astringSet(astring *astr, const char *data, size_t data_len);

ustring *ustringSet(ustring *ustr, const wchar_t *data, size_t data_len);

astring *astringSetZ(astring *astr, const char *data);

ustring *ustringSetZ(ustring *ustr, const wchar_t *data);

// =============================== "Make" functions ===============================

astring astringMake(const char *str);

ustring ustringMake(const wchar_t *str);

// =============================== "Create" functions ===============================

astring *astringCreate(const char *str);

ustring *ustringCreate(const wchar_t *str);

/*
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

/*
 * Pack the least-significant <num_bytes> of <data> into <buf>, updating
 * <size> and <pos>.
 */
buffer *uintPack(unsigned int data, size_t num_bytes, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *boolPack(const bool data, buffer *buf);

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
buffer *astringPack(const astring *str, buffer *buf);

/*
 * Add <data> to position <pos> in <buf>, which currently has size <size>, enlarging it if
 * necessary. Return the number of bytes added to <buf>.
 */
buffer *ustringPack(const ustring *str, buffer *buf);

/*
 * Unpack <num_bytes> from buf (which has size <size>) and fill <data> with them.
 */
size_t uintUnpack(size_t num_bytes, const buffer *buf, size_t pos, unsigned int *data);

/*
 * Unpack a bool from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t boolUnpack(const buffer *buf, size_t pos, bool *data);

size_t uint8Unpack(const buffer *buf, size_t pos, uint8_t *data);

size_t uint16Unpack(const buffer *buf, size_t pos, uint16_t *data);

size_t uint32Unpack(const buffer *buf, size_t pos, uint32_t *data);

size_t uint64Unpack(const buffer *buf, size_t pos, uint64_t *data);

size_t int8Unpack(const buffer *buf, size_t pos, int8_t *data);

size_t int16Unpack(const buffer *buf, size_t pos, int16_t *data);

size_t int32Unpack(const buffer *buf, size_t pos, int32_t *data);

size_t int64Unpack(const buffer *buf, size_t pos, int64_t *data);

/*
 * Unpack a float from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t float32Unpack(const buffer *buf, size_t pos, float *data);

/*
 * Unpack a double from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
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

/*
 * Read <num_bytes> bytes from <fd> and put them towards the least-significant
 * side of <data>.
 */
size_t uintReadFromFD(int fd, size_t num_bytes, unsigned int *data);

/*
 * Write the <num_bytes> least-significant bytes from <data> to <fd>
 */
size_t uintWriteToFD(int fd, size_t num_bytes, unsigned int data);

/*
 * <num_bytes> bytes from <fp> and put them towards the least-significant side
 * of <data>.
 */
size_t uintReadFromFP(FILE *fp, size_t num_bytes, unsigned int *data);

/*
 * Write the <num_bytes> least-significant bytes from <data> to <fp>
 */
size_t uintWriteToFP(FILE *fp, size_t num_bytes, unsigned int data);

/*
 * Read an char *from <fd> into <data).
 */
size_t astringReadFromFD(int fd, char **data);

/*
 * Write an char *to <fd> from <data).
 */
size_t astringWriteToFD(int fd, const char *data);

/*
 * Read an char *from <fp> into <data).
 */
size_t astringReadFromFP(FILE *fp, char **data);

/*
 * Write an char *to <fp> from <data).
 */
size_t astringWriteToFP(FILE *fp, const char *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void astringPrint(FILE *fp, const char *data, int indent);

/*
 * Copy string <src> to <dst>.
 */
void astringCopy(char **dst, const char *src);

/*
 * Read a wchar_t *from <fd> into <data).
 */
size_t ustringReadFromFD(int fd, wchar_t **data);

/*
 * Write a wchar_t *to <fd> from <data).
 */
size_t ustringWriteToFD(int fd, const wchar_t *data);

/*
 * Read a wchar_t *from <fp> into <data).
 */
size_t ustringReadFromFP(FILE *fp, wchar_t **data);

/*
 * Write a wchar_t *to <fp> from <data).
 */
size_t ustringWriteToFP(FILE *fp, const wchar_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void ustringPrint(FILE *fp, const wchar_t *data, int indent);

/*
 * Copy string <src> to <dst>.
 */
void ustringCopy(wchar_t **dst, const wchar_t *src);

/*
 * Read a float from <fd> into <data).
 */
size_t float32ReadFromFD(int fd, float *data);

/*
 * Write a float to <fd> from <data).
 */
size_t float32WriteToFD(int fd, float data);

/*
 * Read a float from <fp> into <data).
 */
size_t float32ReadFromFP(FILE *fp, float *data);

/*
 * Write a float to <fp> from <data).
 */
size_t float32WriteToFP(FILE *fp, float data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float32Print(FILE *fp, float data, int indent);

/*
 * Read a double from <fd> into <data).
 */
size_t float64ReadFromFD(int fd, double *data);

/*
 * Write a double to <fd> from <data).
 */
size_t float64WriteToFD(int fd, double data);

/*
 * Read a double from <fp> into <data).
 */
size_t float64ReadFromFP(FILE *fp, double *data);

/*
 * Write a double to <fp> from <data).
 */
size_t float64WriteToFP(FILE *fp, double data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float64Print(FILE *fp, double data, int indent);

/*
 * Read a bool from <fd> into <data).
 */
size_t boolReadFromFD(int fd, bool *data);

/*
 * Write a bool to <fd> from <data).
 */
size_t boolWriteToFD(int fd, bool data);

/*
 * Read a bool from <fp> into <data).
 */
size_t boolReadFromFP(FILE *fp, bool *data);

/*
 * Write a bool to <fp> from <data).
 */
size_t boolWriteToFP(FILE *fp, bool data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void boolPrint(FILE *fp, bool data, int indent);

/*
 * Copy <src> to <dst>.
 */
void boolCopy(bool *dst, bool src);

/*
 * Read a uint8_t from <fd> into <data).
 */
size_t uint8ReadFromFD(int fd, uint8_t *data);

/*
 * Write a uint8_t to <fd> from <data).
 */
size_t uint8WriteToFD(int fd, uint8_t data);

/*
 * Read a uint8_t from <fp> into <data).
 */
size_t uint8ReadFromFP(FILE *fp, uint8_t *data);

/*
 * Write a uint8_t to <fp> from <data).
 */
size_t uint8WriteToFP(FILE *fp, uint8_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint8Print(FILE *fp, uint8_t data, int indent);

/*
 * Return the number of bytes required to pack an int8_t.
 */
size_t int8PackSize(void);

/*
 * Unpack an int8_t from <buf> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int8Unpack(const buffer *buf, size_t pos, int8_t *data);

/*
 * Add <data> to position <pos> in <buf>, which has size <size>, enlarging it
 * if necessary.
 */
buffer *int8Pack(const int8_t data, buffer *buf);

/*
 * Read an int8_t from <fd> into <data).
 */
size_t int8ReadFromFD(int fd, int8_t *data);

/*
 * Write an int8_t to <fd> from <data).
 */
size_t int8WriteToFD(int fd, int8_t data);

/*
 * Read an int8_t from <fp> into <data).
 */
size_t int8ReadFromFP(FILE *fp, int8_t *data);

/*
 * Write an int8_t to <fp> from <data).
 */
size_t int8WriteToFP(FILE *fp, int8_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int8Print(FILE *fp, int8_t data, int indent);

/*
 * Read a uint16_t from <fd> into <data).
 */
size_t uint16ReadFromFD(int fd, uint16_t *data);

/*
 * Write a uint16_t to <fd> from <data).
 */
size_t uint16WriteToFD(int fd, uint16_t data);

/*
 * Read a uint16_t from <fp> into <data).
 */
size_t uint16ReadFromFP(FILE *fp, uint16_t *data);

/*
 * Write a uint16_t to <fp> from <data).
 */
size_t uint16WriteToFP(FILE *fp, uint16_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint16Print(FILE *fp, uint16_t data, int indent);

/*
 * Read a int16_t from <fd> into <data).
 */
size_t int16ReadFromFD(int fd, int16_t *data);

/*
 * Write a int16_t to <fd> from <data).
 */
size_t int16WriteToFD(int fd, int16_t data);

/*
 * Read a int16_t from <fp> into <data).
 */
size_t int16ReadFromFP(FILE *fp, int16_t *data);

/*
 * Write a int16_t to <fp> from <data).
 */
size_t int16WriteToFP(FILE *fp, int16_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int16Print(FILE *fp, int16_t data, int indent);

/*
 * Read a uint32_t from <fd> into <data).
 */
size_t uint32ReadFromFD(int fd, uint32_t *data);

/*
 * Write a uint32_t to <fd> from <data).
 */
size_t uint32WriteToFD(int fd, uint32_t data);

/*
 * Read a uint32_t from <fp> into <data).
 */
size_t uint32ReadFromFP(FILE *fp, uint32_t *data);

/*
 * Write a uint32_t to <fp> from <data).
 */
size_t uint32WriteToFP(FILE *fp, uint32_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint32Print(FILE *fp, uint32_t data, int indent);

/*
 * Read an int32_t from <fd> into <data).
 */
size_t int32ReadFromFD(int fd, int32_t *data);

/*
 * Write an int32_t to <fd> from <data).
 */
size_t int32WriteToFD(int fd, int32_t data);

/*
 * Read an int32_t from <fp> into <data).
 */
size_t int32ReadFromFP(FILE *fp, int32_t *data);

/*
 * Write an int32_t to <fp> from <data).
 */
size_t int32WriteToFP(FILE *fp, int32_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int32Print(FILE *fp, int32_t data, int indent);

/*
 * Read a uint64_t from <fd> into <data).
 */
size_t uint64ReadFromFD(int fd, uint64_t *data);

/*
 * Write a uint64_t to <fd> from <data).
 */
size_t uint64WriteToFD(int fd, uint64_t data);

/*
 * Read a uint64_t from <fp> into <data).
 */
size_t uint64ReadFromFP(FILE *fp, uint64_t *data);

/*
 * Write a uint64_t to <fp> from <data).
 */
size_t uint64WriteToFP(FILE *fp, uint64_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint64Print(FILE *fp, uint64_t data, int indent);

/*
 * Read an int64_t from <fd> into <data).
 */
size_t int64ReadFromFD(int fd, int64_t *data);

/*
 * Write an int64_t to <fd> from <data).
 */
size_t int64WriteToFD(int fd, int64_t data);

/*
 * Read an int64_t from <fp> into <data).
 */
size_t int64ReadFromFP(FILE *fp, int64_t *data);

/*
 * Write an int64_t to <fp> from <data).
 */
size_t int64WriteToFP(FILE *fp, int64_t data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int64Print(FILE *fp, int64_t data, int indent);

#ifdef __cplusplus
}
#endif

#endif
