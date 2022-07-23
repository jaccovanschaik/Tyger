#ifndef LIBTYGER_H
#define LIBTYGER_H

/* libtyger.h: Run-time portion for the Tyger type generator.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-31
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Set the indentation string.
 */
void setIndent(const char *str);

/*
 * Return an indentation string for level <level>.
 */
const char *indent(int level);

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
size_t astringPack(const char *const *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read an astring from <fd> into <data).
 */
size_t astringReadFromFD(int fd, char **data);

/*
 * Write an astring to <fd> from <data).
 */
size_t astringWriteToFD(int fd, const char *const *data);

/*
 * Read an astring from <fp> into <data).
 */
size_t astringReadFromFP(FILE *fp, char **data);

/*
 * Write an astring to <fp> from <data).
 */
size_t astringWriteToFP(FILE *fp, const char *const *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void astringPrint(FILE *fp, const char *const *data, int indent);

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
 * Unpack a ustring from <buffer> (which has size <size>) and put it at
 * the address pointed to by <data>.
 */
size_t ustringUnpack(const char *buffer, size_t size, wchar_t **data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t ustringPack(const wchar_t *const *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a ustring from <fd> into <data).
 */
size_t ustringReadFromFD(int fd, wchar_t **data);

/*
 * Write a ustring to <fd> from <data).
 */
size_t ustringWriteToFD(int fd, const wchar_t *const *data);

/*
 * Read a ustring from <fp> into <data).
 */
size_t ustringReadFromFP(FILE *fp, wchar_t **data);

/*
 * Write a ustring to <fp> from <data).
 */
size_t ustringWriteToFP(FILE *fp, const wchar_t **data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void ustringPrint(FILE *fp, const wchar_t *const *data, int indent);

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
 * Return the number of bytes required to pack the float pointed to by <data>.
 */
size_t float32PackSize(const float *data);

/*
 * Unpack a float from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t float32Unpack(const char *buffer, size_t size, float *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t float32Pack(const float *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a float from <fd> into <data).
 */
size_t float32ReadFromFD(int fd, float *data);

/*
 * Write a float to <fd> from <data).
 */
size_t float32WriteToFD(int fd, const float *data);

/*
 * Read a float from <fp> into <data).
 */
size_t float32ReadFromFP(FILE *fp, float *data);

/*
 * Write a float to <fp> from <data).
 */
size_t float32WriteToFP(FILE *fp, const float *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float32Print(FILE *fp, const float *data, int indent);

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
 * Return the number of bytes required to pack the double pointed to by <data>.
 */
size_t float64PackSize(const double *data);

/*
 * Unpack a double from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t float64Unpack(const char *buffer, size_t size, double *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t float64Pack(const double *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a double from <fd> into <data).
 */
size_t float64ReadFromFD(int fd, double *data);

/*
 * Write a double to <fd> from <data).
 */
size_t float64WriteToFD(int fd, const double *data);

/*
 * Read a double from <fp> into <data).
 */
size_t float64ReadFromFP(FILE *fp, double *data);

/*
 * Write a double to <fp> from <data).
 */
size_t float64WriteToFP(FILE *fp, const double *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void float64Print(FILE *fp, const double *data, int indent);

/*
 * Copy <src> to <dst>.
 */
void float64Copy(double *dst, const double *src);

/*
 * Clear the contents of <data>.
 */
void uint8Clear(uint8_t *data);

/*
 * Destroy <data>.
 */
void uint8Destroy(uint8_t *data);

/*
 * Return the number of bytes required to pack the uint8_t pointed to by <data>.
 */
size_t uint8PackSize(const uint8_t *data);

/*
 * Unpack a uint8_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint8Unpack(const char *buffer, size_t size, uint8_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint8Pack(const uint8_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a uint8_t from <fd> into <data).
 */
size_t uint8ReadFromFD(int fd, uint8_t *data);

/*
 * Write a uint8_t to <fd> from <data).
 */
size_t uint8WriteToFD(int fd, const uint8_t *data);

/*
 * Read a uint8_t from <fp> into <data).
 */
size_t uint8ReadFromFP(FILE *fp, uint8_t *data);

/*
 * Write a uint8_t to <fp> from <data).
 */
size_t uint8WriteToFP(FILE *fp, const uint8_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint8Print(FILE *fp, const uint8_t *data, int indent);

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
 * Return the number of bytes required to pack the int8_t pointed to by <data>.
 */
size_t int8PackSize(const int8_t *data);

/*
 * Unpack an int8_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int8Unpack(const char *buffer, size_t size, int8_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int8Pack(const int8_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read an int8_t from <fd> into <data).
 */
size_t int8ReadFromFD(int fd, int8_t *data);

/*
 * Write an int8_t to <fd> from <data).
 */
size_t int8WriteToFD(int fd, const int8_t *data);

/*
 * Read an int8_t from <fp> into <data).
 */
size_t int8ReadFromFP(FILE *fp, int8_t *data);

/*
 * Write an int8_t to <fp> from <data).
 */
size_t int8WriteToFP(FILE *fp, const int8_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int8Print(FILE *fp, const int8_t *data, int indent);

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
 * Return the number of bytes required to pack the uint16_t pointed to by <data>.
 */
size_t uint16PackSize(const uint16_t *data);

/*
 * Unpack a uint16_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint16Unpack(const char *buffer, size_t size, uint16_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint16Pack(const uint16_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a uint16_t from <fd> into <data).
 */
size_t uint16ReadFromFD(int fd, uint16_t *data);

/*
 * Write a uint16_t to <fd> from <data).
 */
size_t uint16WriteToFD(int fd, const uint16_t *data);

/*
 * Read a uint16_t from <fp> into <data).
 */
size_t uint16ReadFromFP(FILE *fp, uint16_t *data);

/*
 * Write a uint16_t to <fp> from <data).
 */
size_t uint16WriteToFP(FILE *fp, const uint16_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint16Print(FILE *fp, const uint16_t *data, int indent);

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
 * Return the number of bytes required to pack the int16_t pointed to by <data>.
 */
size_t int16PackSize(const int16_t *data);

/*
 * Unpack an int16_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int16Unpack(const char *buffer, size_t size, int16_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int16Pack(const int16_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a int16_t from <fd> into <data).
 */
size_t int16ReadFromFD(int fd, int16_t *data);

/*
 * Write a int16_t to <fd> from <data).
 */
size_t int16WriteToFD(int fd, const int16_t *data);

/*
 * Read a int16_t from <fp> into <data).
 */
size_t int16ReadFromFP(FILE *fp, int16_t *data);

/*
 * Write a int16_t to <fp> from <data).
 */
size_t int16WriteToFP(FILE *fp, const int16_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int16Print(FILE *fp, const int16_t *data, int indent);

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
 * Return the number of bytes required to pack the uint32_t pointed to by <data>.
 */
size_t uint32PackSize(const uint32_t *data);

/*
 * Unpack a uint32_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint32Unpack(const char *buffer, size_t size, uint32_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint32Pack(const uint32_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a uint32_t from <fd> into <data).
 */
size_t uint32ReadFromFD(int fd, uint32_t *data);

/*
 * Write a uint32_t to <fd> from <data).
 */
size_t uint32WriteToFD(int fd, const uint32_t *data);

/*
 * Read a uint32_t from <fp> into <data).
 */
size_t uint32ReadFromFP(FILE *fp, uint32_t *data);

/*
 * Write a uint32_t to <fp> from <data).
 */
size_t uint32WriteToFP(FILE *fp, const uint32_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint32Print(FILE *fp, const uint32_t *data, int indent);

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
 * Return the number of bytes required to pack the int32_t pointed to by <data>.
 */
size_t int32PackSize(const int32_t *data);

/*
 * Unpack an int32_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int32Unpack(const char *buffer, size_t size, int32_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int32Pack(const int32_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read an int32_t from <fd> into <data).
 */
size_t int32ReadFromFD(int fd, int32_t *data);

/*
 * Write an int32_t to <fd> from <data).
 */
size_t int32WriteToFD(int fd, const int32_t *data);

/*
 * Read an int32_t from <fp> into <data).
 */
size_t int32ReadFromFP(FILE *fp, int32_t *data);

/*
 * Write an int32_t to <fp> from <data).
 */
size_t int32WriteToFP(FILE *fp, const int32_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int32Print(FILE *fp, const int32_t *data, int indent);

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
 * Return the number of bytes required to pack the uint64_t pointed to by <data>.
 */
size_t uint64PackSize(const uint64_t *data);

/*
 * Unpack a uint64_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t uint64Unpack(const char *buffer, size_t size, uint64_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t uint64Pack(const uint64_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read a uint64_t from <fd> into <data).
 */
size_t uint64ReadFromFD(int fd, uint64_t *data);

/*
 * Write a uint64_t to <fd> from <data).
 */
size_t uint64WriteToFD(int fd, const uint64_t *data);

/*
 * Read a uint64_t from <fp> into <data).
 */
size_t uint64ReadFromFP(FILE *fp, uint64_t *data);

/*
 * Write a uint64_t to <fp> from <data).
 */
size_t uint64WriteToFP(FILE *fp, const uint64_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void uint64Print(FILE *fp, const uint64_t *data, int indent);

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
 * Return the number of bytes required to pack the int64_t pointed to by <data>.
 */
size_t int64PackSize(const int64_t *data);

/*
 * Unpack an int64_t from <buffer> (which has size <size>) and put it at the
 * address pointed to by <data>.
 */
size_t int64Unpack(const char *buffer, size_t size, int64_t *data);

/*
 * Add <data> to position <pos> in <buffer>, which has size <size>, enlarging it
 * if necessary.
 */
size_t int64Pack(const int64_t *data, char **buffer, size_t *size, size_t *pos);

/*
 * Read an int64_t from <fd> into <data).
 */
size_t int64ReadFromFD(int fd, int64_t *data);

/*
 * Write an int64_t to <fd> from <data).
 */
size_t int64WriteToFD(int fd, const int64_t *data);

/*
 * Read an int64_t from <fp> into <data).
 */
size_t int64ReadFromFP(FILE *fp, int64_t *data);

/*
 * Write an int64_t to <fp> from <data).
 */
size_t int64WriteToFP(FILE *fp, const int64_t *data);

/*
 * Print an ASCII representation of <data> to <fp>.
 */
void int64Print(FILE *fp, const int64_t *data, int indent);

/*
 * Copy <src> to <dst>.
 */
void int64Copy(int64_t *dst, const int64_t *src);

#ifdef __cplusplus
}
#endif

#endif
