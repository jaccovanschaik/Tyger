#ifndef UTF8_H
#define UTF8_H

/*
 * utf8.h: Handle conversion between UTF-8 and wchar_t.
 *
 * Copyright: (c) 2022-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2022-08-25
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: softtabstop=4 shiftwidth=4 expandtab textwidth=100 columns=100
 */

#include <stdint.h>
#include <stddef.h>

/*
 * Convert the <count> wide characters pointed to by <in> to UTF-8 characters.
 * Returns a pointer to a statically allocated buf containing the UTF-8 text,
 * and returns the number of bytes contained therein through <size>.
 */
const uint8_t *wchar_to_utf8(const wchar_t *in, size_t count, uint32_t *size);

/*
 * Convert the <count> UTF-8 bytes pointed to by <in> to wide characters.
 * Returns a pointer to a statically allocated buf that contains the wide
 * characters, and returns the number of characters contained therein through
 * <size>.
 */
const wchar_t *utf8_to_wchar(const uint8_t *in, size_t count, size_t *size);

#endif
