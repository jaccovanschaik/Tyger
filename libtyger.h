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

/*
 * Set the indentation string.
 */
void setIndent(const char *str);

/*
 * Return an indentation string for level <level>.
 */
const char *indent(int level);

/*
 * Pack the least-significant <num_bytes> of <data> into <buffer>, updating
 * <size> and <pos>.
 */
size_t uintPack(unsigned int data, size_t num_bytes,
                char **buffer, size_t *size, size_t *pos);

/*
 * Unpack <num_bytes> from buffer (which has size <size>) fill <data> with
 * them.
 */
size_t uintUnpack(size_t num_bytes, const char *buffer, size_t size,
        unsigned int *data);

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
 * the <num_bytes> least-significant bytes from <data> to <fp>
 */
size_t uintWriteToFP(FILE *fp, size_t num_bytes, unsigned int data);

/*
 * Clear the contents of <data>.
 */
void astringClear(char **data);

/*
 * Destroy <data>.
 */
void astringDestroy(char **data);

/*
 * Return the number of bytes required to pack the astring pointed to by <data>.
 */
size_t astringPackSize(const char *const *data);

/*
 * Unpack an astring from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t astringUnpack(const char *buffer, size_t size, char **data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t astringPack(const char *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read an astring from <fd> into <data).
 */
size_t astringReadFromFD(int fd, char **data);

/*
 * Write an astring to <fd> from <data).
 */
size_t astringWriteToFD(int fd, const char *data);

/*
 * Read an astring from <fp> into <data).
 */
size_t astringReadFromFP(FILE *fp, char **data);

/*
 * Write an astring to <fp> from <data).
 */
size_t astringWriteToFP(FILE *fp, const char *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void astringPrint(FILE *fp, const char *data, int indent);

/*
 * Copy string <src> to <dst>.
 */
void astringCopy(char **dst, const char *const *src);

/*
 * Clear the contents of <data>.
 */
void ustringClear(wchar_t **data);

/*
 * Destroy <data>.
 */
void ustringDestroy(wchar_t **data);

/*
 * Return the number of bytes required to pack the ustring pointed to by <data>.
 */
size_t ustringPackSize(const wchar_t *const *data);

/*
 * Unpack a UTF-8 encoded ustring from <buffer> (which has size <size>), write
 * it to a newly allocated wide-character string whose starting address is
 * written to <data>, and return the number of bytes consumed from <buffer>.
 * If <buffer> doesn't contain enough bytes to successfully extract a string
 * from it return the mininum number of bytes we would need.
 */
size_t ustringUnpack(const char *buffer, size_t size, wchar_t **data);

/*
 * Add <data> to position <pos> in <buffer>, which currently has size <size>,
 * enlarging it if necessary. Return the number of bytes added to <buffer>.
 */
size_t ustringPack(const wchar_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a ustring from <fd> into <data).
 */
size_t ustringReadFromFD(int fd, wchar_t **data);

/*
 * Write a ustring to <fd> from <data).
 */
size_t ustringWriteToFD(int fd, const wchar_t *data);

/*
 * Read a ustring from <fp> into <data).
 */
size_t ustringReadFromFP(FILE *fp, wchar_t **data);

/*
 * Write a ustring to <fp> from <data).
 */
size_t ustringWriteToFP(FILE *fp, const wchar_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void ustringPrint(FILE *fp, const wchar_t *data, int indent);

/*
 * Copy string <src> to <dst>.
 */
void ustringCopy(wchar_t **dst, const wchar_t *const *src);

/*
 * Clear the contents of <data>.
 */
void float32Clear(float *data);

/*
 * Destroy <data>.
 */
void float32Destroy(float *data);

/*
 * Return the number of bytes required to pack a float32.
 */
size_t float32PackSize(void);

/*
 * Unpack a float from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t float32Unpack(const char *buffer, size_t size, float *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t float32Pack(const float data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void float32Copy(float *dst, const float *src);

/*
 * Clear the contents of <data>.
 */
void float64Clear(double *data);

/*
 * Destroy <data>.
 */
void float64Destroy(double *data);

/*
 * Return the number of bytes required to pack a float64.
 */
size_t float64PackSize(void);

/*
 * Unpack a double from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t float64Unpack(const char *buffer, size_t size, double *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t float64Pack(const double data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void float64Copy(double *dst, const double *src);

/*
 * Clear the contents of <data>.
 */
void boolClear(bool *data);

/*
 * Destroy <data>.
 */
void boolDestroy(bool *data);

/*
 * Return the number of bytes required to pack a bool.
 */
size_t boolPackSize(void);

/*
 * Unpack a bool from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t boolUnpack(const char *buffer, size_t size, bool *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t boolPack(const bool data, char **buffer, size_t *size, size_t *pos);

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
void boolCopy(bool *dst, const bool *src);

/*
 * Clear the contents of <data>.
 */
void uint8Clear(uint8_t *data);

/*
 * Destroy <data>.
 */
void uint8Destroy(uint8_t *data);

/*
 * Return the number of bytes required to pack a uint8_t.
 */
size_t uint8PackSize(void);

/*
 * Unpack a uint8_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint8Unpack(const char *buffer, size_t size, uint8_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint8Pack(const uint8_t data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void uint8Copy(uint8_t *dst, const uint8_t *src);

/*
 * Clear the contents of <data>.
 */
void int8Clear(int8_t *data);

/*
 * Destroy <data>.
 */
void int8Destroy(int8_t *data);

/*
 * Return the number of bytes required to pack an int8_t.
 */
size_t int8PackSize(void);

/*
 * Unpack an int8_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int8Unpack(const char *buffer, size_t size, int8_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int8Pack(const int8_t data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void int8Copy(int8_t *dst, const int8_t *src);

/*
 * Clear the contents of <data>.
 */
void uint16Clear(uint16_t *data);

/*
 * Destroy <data>.
 */
void uint16Destroy(uint16_t *data);

/*
 * Return the number of bytes required to pack a uint16_t.
 */
size_t uint16PackSize(void);

/*
 * Unpack a uint16_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint16Unpack(const char *buffer, size_t size, uint16_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint16Pack(const uint16_t data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void uint16Copy(uint16_t *dst, const uint16_t *src);

/*
 * Clear the contents of <data>.
 */
void int16Clear(int16_t *data);

/*
 * Destroy <data>.
 */
void int16Destroy(int16_t *data);

/*
 * Return the number of bytes required to pack an int16_t.
 */
size_t int16PackSize(void);

/*
 * Unpack an int16_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int16Unpack(const char *buffer, size_t size, int16_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int16Pack(int16_t data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void int16Copy(int16_t *dst, const int16_t *src);

/*
 * Clear the contents of <data>.
 */
void uint32Clear(uint32_t *data);

/*
 * Destroy <data>.
 */
void uint32Destroy(uint32_t *data);

/*
 * Return the number of bytes required to pack a uint32_t.
 */
size_t uint32PackSize(void);

/*
 * Unpack a uint32_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint32Unpack(const char *buffer, size_t size, uint32_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint32Pack(uint32_t data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void uint32Copy(uint32_t *dst, const uint32_t *src);

/*
 * Clear the contents of <data>.
 */
void int32Clear(int32_t *data);

/*
 * Destroy <data>.
 */
void int32Destroy(int32_t *data);

/*
 * Return the number of bytes required to pack an int32_t.
 */
size_t int32PackSize(void);

/*
 * Unpack an int32_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int32Unpack(const char *buffer, size_t size, int32_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int32Pack(int32_t data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void int32Copy(int32_t *dst, const int32_t *src);

/*
 * Clear the contents of <data>.
 */
void uint64Clear(uint64_t *data);

/*
 * Destroy <data>.
 */
void uint64Destroy(uint64_t *data);

/*
 * Return the number of bytes required to pack a uint64_t.
 */
size_t uint64PackSize(void);

/*
 * Unpack a uint64_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint64Unpack(const char *buffer, size_t size, uint64_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint64Pack(uint64_t data, char **buffer, size_t *size, size_t *pos);

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
 * Copy <src> to <dst>.
 */
void uint64Copy(uint64_t *dst, const uint64_t *src);

/*
 * Clear the contents of <data>.
 */
void int64Clear(int64_t *data);

/*
 * Destroy <data>.
 */
void int64Destroy(int64_t *data);

/*
 * Return the number of bytes required to pack an int64_t.
 */
size_t int64PackSize(void);

/*
 * Unpack an int64_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int64Unpack(const char *buffer, size_t size, int64_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int64Pack(int64_t data, char **buffer, size_t *size, size_t *pos);

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

/*
 * Copy <src> to <dst>.
 */
void int64Copy(int64_t *dst, const int64_t *src);

#ifdef __cplusplus
}
#endif

#endif
