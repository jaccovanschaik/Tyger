/* tokenizer.c: Tokenizer for Tyger files.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-24
 * Version:   $Id: tokenizer.c 148 2018-09-08 18:08:57Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>

#include <libjvs/list.h>
#include <libjvs/buffer.h>
#include <libjvs/utils.h>

#include "tokenizer.h"
#include "tokentype.h"

#define DEBUG 0

/* Tokenizer state. */
typedef enum {
    STATE_SPACE,
    STATE_USTRING,
    STATE_DSTRING,
    STATE_SSTRING,
    STATE_LONG,
    STATE_DOUBLE,
    STATE_LINEMARKER,
    STATE_ESCAPE
} State;

/* Scratch pad to build tokens. */
typedef struct {
    char *file;
    int line;
    int column;
    Buffer buffer;
} Scratch;

/* Type of input. */
typedef enum {
    INPUT_FP,
    INPUT_TEXT
} InputType;

/* Input stream. */
typedef struct {
    InputType type;
    union {
        FILE *fp;
        const char *cp;
    } u;
} Input;

/*
 * Initialize scratch pad for a token on <line> and <column>.
 */
static void init_scratch(Scratch *scratch, const char *file, int line, int column)
{
    bufClear(&scratch->buffer);

    if (scratch->file != NULL) free(scratch->file);

    scratch->file = strdup(file);
    scratch->line = line;
    scratch->column = column;
}

/*
 * Add character <c> to <scratch>.
 */
static void add_to_scratch(Scratch *scratch, char c)
{
    bufAddC(&scratch->buffer, c);
}

/*
 * Start building a token in <scratch> with character <c> on <line> and <column>.
 */
static void start_scratch(Scratch *scratch, const char *file, int line, int column, char c)
{
    init_scratch(scratch, file, line, column);

    add_to_scratch(scratch, c);
}

/*
 * Add a token of type <type> to <tokens>, using data from <scratch>.
 */
static int add_token(Scratch *scratch, tkType type, List *tokens)
{
    tkToken *token = NULL;

    char *endptr;
    const char *text = bufGet(&scratch->buffer);
    int len = bufLen(&scratch->buffer);

    if (type == TT_DOUBLE) {
        double value = strtod(text, &endptr);
        if (endptr == text + len) {
            token = calloc(1, sizeof(*token));
            token->type = TT_DOUBLE;
            token->u.d = value;
        }
    }
    else if (type == TT_LONG) {
        long value = strtol(text, &endptr, 0);
        if (endptr == text + len) {
            token = calloc(1, sizeof(*token));
            token->type = TT_LONG;
            token->u.l = value;
        }
    }
    else if (type == TT_USTRING || type == TT_DSTRING || type == TT_SSTRING) {
        token = calloc(1, sizeof(*token));
        token->type = type;
        token->u.s = strdup(text);
    }
    else {
        token = calloc(1, sizeof(*token));
        token->type = type;
    }

    if (token != NULL) {
        token->file = strdup(scratch->file);
        token->line = scratch->line;
        token->column = scratch->column;

        listAppendTail(tokens, token);

        return 0;
    }
    else {
        return 1;
    }
}

#if DEBUG
/*
 * Dump info about token <token>.
 */
static void dump_token(tkToken *token)
{
    fprintf(stderr, "%s:%d:%d: type = %s",
            token->file, token->line, token->column,
            tokentype_enum_to_string(token->type));

    switch (token->type) {
    case TT_LONG:
        fprintf(stderr, ", value = %ld\n", token->u.l);
        break;
    case TT_DOUBLE:
        fprintf(stderr, ", value = %f\n", token->u.d);
        break;
    case TT_USTRING:
    case TT_DSTRING:
    case TT_SSTRING:
        fprintf(stderr, ", value = \"%s\"\n", token->u.s);
        break;
    default:
        fprintf(stderr, "\n");
        break;
    }
}

/*
 * Dump tokens in token list <tokens>.
 */
static void dump_token_list(List *tokens)
{
    tkToken *token;

    for (token = listHead(tokens); token; token = listNext(token)) {
        dump_token(token);
    }
}
#endif

/*
 * Clear token list <tokens>.
 */
void tokClear(List *tokens)
{
    tkToken *token;

    while ((token = listRemoveHead(tokens)) != NULL) {
        if (token->type == TT_USTRING ||
            token->type == TT_DSTRING ||
            token->type == TT_SSTRING) {
            free(token->u.s);
        }

        free(token);
    }
}

/*
 * Get and return a character from <in>.
 */
static int tok_get(Input *in)
{
    int c;

    switch(in->type) {
    case INPUT_FP:
        c = fgetc(in->u.fp);
        break;
    case INPUT_TEXT:
        if ((c = *in->u.cp) == '\0') {
            c = EOF;
        }
        else {
            in->u.cp++;
        }
        break;
    }

    return c;
}

/*
 * Push back character <c> into <in>.
 */
static int tok_unget(Input *in, int c)
{
    switch(in->type) {
    case INPUT_FP:
        return ungetc(c, in->u.fp);
    case INPUT_TEXT:
        in->u.cp--;
        return c;
    default:
        return EOF;
    }
}

/*
 * Create tokens from <in> and put them into <tokens>. <filename> specifies the
 * stream we're reading from. Returns an error message if something went wrong,
 * otherwise returns NULL.
 */
static char *tokenize(Input *in, const char *filename, List *tokens)
{
    int line = 1, column = 0, c;
    char file[PATH_MAX];

    Buffer error = { 0 };
    Scratch scratch = { 0 };

    State old_state, state = STATE_SPACE;

    strncpy(file, filename, sizeof(file));

    while (1) {
        if (!bufIsEmpty(&error)) {
            break;
        }

        c = tok_get(in);

        if (c == '\n') {
            line++;
            column = 0;
        }
        else {
            column++;
        }

        switch(state) {
        case STATE_SPACE:
            if (isdigit(c)) {
                start_scratch(&scratch, file, line, column, c);
                state = STATE_LONG;
            }
            else if (c == '.') {
                start_scratch(&scratch, file, line, column, c);
                state = STATE_DOUBLE;
            }
            else if (isalpha(c)) {
                start_scratch(&scratch, file, line, column, c);
                state = STATE_USTRING;
            }
            else if (c == '"') {
                init_scratch(&scratch, file, line, column);
                state = STATE_DSTRING;
            }
            else if (c == '\'') {
                init_scratch(&scratch, file, line, column);
                state = STATE_SSTRING;
            }
            else if (c == '#') {
                start_scratch(&scratch, file, line, column, c);
                state = STATE_LINEMARKER;
            }
            else if (c == '(') {
                scratch.line = line;
                scratch.column = column;
                add_token(&scratch, TT_OPAREN, tokens);
            }
            else if (c == ')') {
                scratch.line = line;
                scratch.column = column;
                add_token(&scratch, TT_CPAREN, tokens);
            }
            else if (c == '{') {
                scratch.line = line;
                scratch.column = column;
                add_token(&scratch, TT_OBRACE, tokens);
            }
            else if (c == '}') {
                scratch.line = line;
                scratch.column = column;
                add_token(&scratch, TT_CBRACE, tokens);
            }
            else if (c == '=') {
                scratch.line = line;
                scratch.column = column;
                add_token(&scratch, TT_EQUALS, tokens);
            }
            else if (c == ':') {
                scratch.line = line;
                scratch.column = column;
                add_token(&scratch, TT_COLON, tokens);
            }
            else if (c != EOF && !isspace(c)) {
                bufSetF(&error, "%d:%d: unexpected character '%c' (ascii %d).",
                        line, column, c, c);
            }
            break;
        case STATE_LONG:
            if (c == '.' || tolower(c) == 'e') {
                add_to_scratch(&scratch, c);
                state = STATE_DOUBLE;
            }
            else if (isxdigit(c) || tolower(c) == 'x') {
                add_to_scratch(&scratch, c);
            }
            else if (isalpha(c)) {
                bufSetF(&error, "%d:%d: badly formatted number.",
                        scratch.line, scratch.column);
            }
            else if (add_token(&scratch, TT_LONG, tokens) != 0) {
                bufSetF(&error, "%d:%d: badly formatted number.",
                        scratch.line, scratch.column);
            }
            else {
                if (!isspace(c)) {
                    tok_unget(in, c);
                    column--;
                }

                state = STATE_SPACE;
            }
            break;
        case STATE_DOUBLE:
            if (isdigit(c) || tolower(c) == 'e') {
                add_to_scratch(&scratch, c);
            }
            else if (isalpha(c) || c == '.') {
                bufSetF(&error, "%d:%d: badly formatted number.",
                        scratch.line, scratch.column);
            }
            else if (add_token(&scratch, TT_DOUBLE, tokens) != 0) {
                bufSetF(&error, "%d:%d: badly formatted number.",
                        scratch.line, scratch.column);
            }
            else {
                if (!isspace(c)) {
                    tok_unget(in, c);
                    column--;
                }

                state = STATE_SPACE;
            }
            break;
        case STATE_USTRING:
            if (isalnum(c) || c == '_') {
                add_to_scratch(&scratch, c);
            }
            else {
                add_token(&scratch, TT_USTRING, tokens);

                if (!isspace(c)) {
                    tok_unget(in, c);
                    column--;
                }

                state = STATE_SPACE;
            }
            break;
        case STATE_DSTRING:
        case STATE_SSTRING:
            if ((state == STATE_DSTRING && c == '"') ||
                (state == STATE_SSTRING && c == '\'')) {
                add_token(&scratch,
                        state == STATE_DSTRING ? TT_DSTRING : TT_SSTRING, tokens);

                state = STATE_SPACE;
            }
            else if (c == '\\') {
                old_state = state;
                state = STATE_ESCAPE;
            }
            else if (c == EOF) {
                bufSetF(&error, "%d:%d: unterminated string.",
                        scratch.line, scratch.column);
            }
            else {
                add_to_scratch(&scratch, c);
            }
            break;
        case STATE_LINEMARKER:
            if (c == '\n' || c == EOF) {
                if (sscanf(bufGet(&scratch.buffer), "# %d \"%[^\"]\"",
                        &line, file) != 2) {
                    fprintf(stderr, "Failed parsing string \"%s\"\n",
                            bufGet(&scratch.buffer));
                }

                column = 0;

#if DEBUG
                fprintf(stderr, "Line %d in \"%s\"\n", scratch.line, file);
#endif

                state = STATE_SPACE;
            }
            else {
                add_to_scratch(&scratch, c);
            }
            break;
        case STATE_ESCAPE:
            if (c == '\\') {
                add_to_scratch(&scratch, '\\');
            }
            else if (c == 'n') {
                add_to_scratch(&scratch, '\n');
            }
            else if (c == 'r') {
                add_to_scratch(&scratch, '\r');
            }
            else if (c == 't') {
                add_to_scratch(&scratch, '\t');
            }
            else {
                add_to_scratch(&scratch, c);
            }

            state = old_state;

            break;
        }

        if (c == EOF) {
            break;
        }
    }

    free(scratch.file);
    scratch.file = NULL;

    bufReset(&scratch.buffer);

    scratch.file = strdup(filename);
    scratch.line = line;
    scratch.column = column;
    add_token(&scratch, TT_EOF, tokens);

#if DEBUG
    dump_token_list(tokens);
#endif

    if (!bufIsEmpty(&error)) {
#if DEBUG
        fprintf(stderr, "%s\n", bufGet(&error));
#endif
        tokClear(tokens);
        return bufDetach(&error);
    }
    else {
        return NULL;
    }
}

/*
 * Create tokens from the stream given by <fp> and put them in <tokens>.
 * Returns an error message if something went wrong, otherwise returns NULL.
 */
char *tokStream(FILE *fp, List *tokens)
{
    char *r;
    Input *in = calloc(1, sizeof(*in));

    in->type = INPUT_FP;
    in->u.fp = fp;

    r = tokenize(in, "<stream>", tokens);

    free(in);

    return r;
}

/*
 * Create tokens from string <text>. Returns an error message if something went
 * wrong, otherwise returns NULL.
 */
char *tokString(char *text, List *tokens)
{
    char *r;
    Input *in = calloc(1, sizeof(*in));

    in->type = INPUT_TEXT;
    in->u.cp = text;

    r = tokenize(in, "<string>", tokens);

    free(in);

    return r;
}

/*
 * Create tokens from the file <filename>, which is piped through cpp (the C
 * preprocessor) before processing. Returns an error message if something went
 * wrong, otherwise returns NULL.
 */
char *tokFile(const char *filename, List *output)
{
    char *r, *msg;
    FILE *fp;

    Buffer *cmdline = bufCreate();

    bufSetF(cmdline, "cpp -x c++ -traditional-cpp %s", filename);

    if ((fp = popen(bufGet(cmdline), "r")) == NULL) {
        r = strdup(strerror(errno));
    }
    else if ((msg = tokStream(fp, output)) != NULL) {
        r = msg;
    }
    else if (pclose(fp) != 0) {
        r = strdup(strerror(errno));
    }
    else {
        r = NULL;
    }

    bufDestroy(cmdline);

    return r;
}

#ifdef TEST
/*
 * Test the token list in <tokens>. See below for examples.
 */
static int test_tokens(List *tokens, int count, ...)
{
    va_list ap;
    int i, errors = 0;
    int line, column;
    char *file;

    tkToken *token;
    tkType type;

    make_sure_that(listLength(tokens) == count + 1);

    va_start(ap, count);

    for (token = listHead(tokens), i = 0;
         i < count && token != NULL;
         token = listNext(token), i++)
    {
        long l_exp, l_act;
        double d_exp, d_act;
        char *s_exp, *s_act;

        type = va_arg(ap, tkType);
#if DEBUG
        fprintf(stderr, "Expected type: %s, got: %s\n",
                tokentype_enum_to_string(type),
                tokentype_enum_to_string(token->type));
#endif
        make_sure_that(token->type == type);

        file = va_arg(ap, char *);
#if DEBUG
        fprintf(stderr, "Expected file: %s, got: %s\n", file, token->file);
#endif
        make_sure_that(strcmp(token->file, file) == 0);

        line = va_arg(ap, int);
#if DEBUG
        fprintf(stderr, "Expected line: %d, got: %d\n", line, token->line);
#endif
        make_sure_that(token->line == line);

        column = va_arg(ap, int);
#if DEBUG
        fprintf(stderr, "Expected column: %d, got: %d\n", column, token->column);
#endif
        make_sure_that(token->column == column);

        switch (type) {
        case TT_LONG:
            l_exp = va_arg(ap, long);
            l_act = token->u.l;

#if DEBUG
            fprintf(stderr, "Expected: %ld, got: %ld\n", l_exp, l_act);
#endif

            make_sure_that(l_exp == l_act);
            break;
        case TT_DOUBLE:
            d_exp = va_arg(ap, double);
            d_act = token->u.d;

#if DEBUG
            fprintf(stderr, "Expected: %f, got: %f\n", d_exp, d_act);
#endif

            make_sure_that(d_exp == d_act);
            break;
        case TT_USTRING:
        case TT_DSTRING:
        case TT_SSTRING:
            s_exp = va_arg(ap, char *);
            s_act = token->u.s;

#if DEBUG
            fprintf(stderr, "Expected: \"%s\", got: \"%s\"\n", s_exp, s_act);
#endif

            make_sure_that(strcmp(s_exp, s_act) == 0);
            break;
        default:
            break;
        }
    }

    return errors;
}

int main(int argc, char *argv[])
{
    int errors = 0;

    List tokens = { 0 };

    char *text, *msg;

    /* Simple string without line terminator. */

    text = "Test1a";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);
    make_sure_that(listLength(&tokens) == 2);

    errors += test_tokens(&tokens, 1,
            TT_USTRING, "<string>", 1, 1, "Test1a");

    tokClear(&tokens);

    /* Simple string with line terminator. */

    text = "Test2a\n";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);

    errors += test_tokens(&tokens, 1,
            TT_USTRING, "<string>", 1, 1, "Test2a");

    tokClear(&tokens);

    /* A little more complex unquoted string. */

    text = "ABC_123\n";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);

    errors += test_tokens(&tokens, 1,
            TT_USTRING, "<string>", 1, 1, "ABC_123");

    tokClear(&tokens);

    /* Multiple strings, longs and doubles. */

    text = "Test3a \"Test3b\" 'Goodbye' 123 0.5 1e2 0x10 0777";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);

    errors += test_tokens(&tokens, 8,
            TT_USTRING, "<string>", 1, 1, "Test3a",
            TT_DSTRING, "<string>", 1, 8, "Test3b",
            TT_SSTRING, "<string>", 1, 17, "Goodbye",
            TT_LONG,    "<string>", 1, 27, 123L,
            TT_DOUBLE,  "<string>", 1, 31, 0.5,
            TT_DOUBLE,  "<string>", 1, 35, 100.0,
            TT_LONG,    "<string>", 1, 39, 16,
            TT_LONG,    "<string>", 1, 44, 511);

    tokClear(&tokens);

    /* Strings with parentheses and braces. */

    text = "Test4a(Test4b{})";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);

    errors += test_tokens(&tokens, 6,
            TT_USTRING, "<string>", 1, 1, "Test4a",
            TT_OPAREN,  "<string>", 1, 7,
            TT_USTRING, "<string>", 1, 8, "Test4b",
            TT_OBRACE,  "<string>", 1, 14,
            TT_CBRACE,  "<string>", 1, 15,
            TT_CPAREN,  "<string>", 1, 16);

    tokClear(&tokens);

    /* Colons and equals signs. */

    text = "Take note: Pi = 3.14";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);

    errors += test_tokens(&tokens, 6,
            TT_USTRING, "<string>", 1, 1, "Take",
            TT_USTRING, "<string>", 1, 6, "note",
            TT_COLON,   "<string>", 1, 10,
            TT_USTRING, "<string>", 1, 12, "Pi",
            TT_EQUALS,  "<string>", 1, 15,
            TT_DOUBLE,  "<string>", 1, 17,  3.14);

    tokClear(&tokens);

    /* Bunched-up strings. */

    text = "'A'B\"C\"";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);

    errors += test_tokens(&tokens, 3,
            TT_SSTRING, "<string>", 1, 1, "A",
            TT_USTRING, "<string>", 1, 4, "B",
            TT_DSTRING, "<string>", 1, 5, "C");

    tokClear(&tokens);

    /* Escape characters in a single-quoted string. */

    text = "'\\t\\r\\n\\\\'";

    msg = tokString(text, &tokens);

    make_sure_that(msg == NULL);

    errors += test_tokens(&tokens, 1,
            TT_SSTRING, "<string>", 1, 1, "\t\r\n\\");

    tokClear(&tokens);

    /* Badly formatted decimal. */

    text = "123XYZ";

    msg = tokString(text, &tokens);

    make_sure_that(msg != NULL);
    make_sure_that(strcmp(msg, "1:1: badly formatted number.") == 0);
    make_sure_that(listIsEmpty(&tokens));

    /* Badly formatted octal. */

    text = "0123456789";

    msg = tokString(text, &tokens);

    make_sure_that(msg != NULL);
    make_sure_that(strcmp(msg, "1:1: badly formatted number.") == 0);
    make_sure_that(listIsEmpty(&tokens));

    /* Badly formatted hexadecimal. */

    text = "0x123456789ABCDEFG";

    msg = tokString(text, &tokens);

    make_sure_that(msg != NULL);
    make_sure_that(strcmp(msg, "1:1: badly formatted number.") == 0);
    make_sure_that(listIsEmpty(&tokens));

    /* Escape code outside quotes. */

    text = "\\t";

    msg = tokString(text, &tokens);

    make_sure_that(msg != NULL);
    make_sure_that(strcmp(msg, "1:1: unexpected character '\\' (ascii 92).") == 0);
    make_sure_that(listIsEmpty(&tokens));

    /* Unterminated single-quoted string. */

    text = "xyz'abc";

    msg = tokString(text, &tokens);

    make_sure_that(msg != NULL);
    make_sure_that(strcmp(msg, "1:4: unterminated string.") == 0);
    make_sure_that(listIsEmpty(&tokens));

    /* Unterminated double-quoted string. */

    text = "xyz\"abc";

    msg = tokString(text, &tokens);

    make_sure_that(msg != NULL);
    make_sure_that(strcmp(msg, "1:4: unterminated string.") == 0);
    make_sure_that(listIsEmpty(&tokens));

#if 0
    tokClear(&tokens);

    msg = tokFile("test/Shape.tgr", &tokens);

    make_sure_that(msg == NULL);

    dump_token_list(&tokens);
#endif

    return errors;
}
#endif
