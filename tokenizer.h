#ifndef TOKENIZER_H
#define TOKENIZER_H

/* tokenizer.h: Tokenizer for Tyger files.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-24
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#include <libjvs/list.h>

#include "tokentype.h"

typedef struct {
    ListNode _node;
    char *file;
    int line;
    int column;
    tkType type;
    union {
        long l;
        double d;
        char *s;
    };
} tkToken;

/*
 * Clear token list <tokens>.
 */
void tokClear(List *tokens);

/*
 * Create tokens from the stream given by <fp> and put them in <tokens>.
 * Returns an error message if something went wrong, otherwise returns NULL.
 */
char *tokStream(FILE *fp, List *tokens);

/*
 * Create tokens from string <text>. Returns an error message if something went
 * wrong, otherwise returns NULL.
 */
char *tokString(char *text, List *tokens);

/*
 * Create tokens from the file <filename>, which is piped through cpp (the C
 * preprocessor) before processing. Returns an error message if something went
 * wrong, otherwise returns NULL.
 */
char *tokFile(const char *filename, List *output);

#endif
