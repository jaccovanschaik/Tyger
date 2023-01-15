/*
 * utf8.c: Handle conversion between UTF-8 and wchar_t.
 *
 * Copyright: (c) 2022-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2022-08-25
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: softtabstop=4 shiftwidth=4 expandtab textwidth=100 columns=100
 */

#include "utf8.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <stdio.h>
#include <errno.h>

/*
 * Convert the <count> wide characters pointed to by <in> to UTF-8 characters.
 * Returns a pointer to a statically allocated buf containing the UTF-8 text,
 * and returns the number of bytes contained therein through <size>.
 */
const uint8_t *wchar_to_utf8(const wchar_t *in, size_t count, uint32_t *size)
{
    static iconv_t conv = NULL;

    static char *out = NULL;
    static size_t out_size = 16;

    size_t inbytesleft;
    size_t outbytesleft;

    if (conv == NULL) {
        if ((conv = iconv_open("UTF-8//TRANSLIT", "wchar_t")) == (iconv_t) -1) {
            perror("wchar_to_utf8: iconv_open");
            conv = NULL;
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

        size_t r = iconv(conv,
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

    return (uint8_t *) out;
}

/*
 * Convert the <count> UTF-8 bytes pointed to by <in> to wide characters.
 * Returns a pointer to a statically allocated buf that contains the wide
 * characters, and returns the number of characters contained therein through
 * <size>.
 */
const wchar_t *utf8_to_wchar(const uint8_t *in, size_t count, size_t *size)
{
    static iconv_t conv = NULL;

    static wchar_t *out = NULL;
    static size_t out_size = 16;

    size_t inbytesleft;
    size_t outbytesleft;

    if (conv == NULL) {
        if ((conv = iconv_open("wchar_t//TRANSLIT", "UTF-8")) == (iconv_t) -1) {
            perror("utf8_to_wchar: iconv_open");
            conv = NULL;
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
        char *in_ptr  = (char *) in;
        char *out_ptr = (char *) out;

        size_t r = iconv(conv,
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
