/* parser.c: Parser for Tyger files.
 *
 * Copyright: (c) 2016-2025 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-25
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <libjvs/list.h>
#include <libjvs/buffer.h>
#include <libjvs/hash.h>
#include <libjvs/defs.h>

#include "tokenizer.h"
#include "parser.h"
#include "utils.h"

typedef enum {
    ST_INITIAL,
    ST_NAME,
    ST_EQUALS,
    ST_CONST,
    ST_ARRAY,
    ST_STRUCT,
    ST_ENUM,
    ST_UNION,
} State;

static void expected(tkType type, tkToken *token, Buffer *error)
{
    bufSetF(error, "%s:%d:%d: expected %s, got %s.\n",
            token->file, token->line, token->column,
            tokentype_enum_to_string(type),
            tokentype_enum_to_string(token->type));
}

static Definition *create_def(const char *name, const char *file, int line, int level)
{
    Definition *def = calloc(1, sizeof(*def));

    def->name  = strdup(name);
    def->file  = strdup(file);
    def->line  = line;
    def->level = level;

    return def;
}

static Definition *find_def(List *defs, const char *name)
{
    Definition *def;

    for (def = listHead(defs); def; def = listNext(def)) {
        if (strcmp(def->name, name) == 0) break;
    }

    return def;
}

static int expect_token(tkToken **token, tkType type, Buffer *error)
{
    if ((*token)->type != type) {
        expected(type, *token, error);
        return 1;
    }
    else {
        *token = listNext(*token);
        return 0;
    }
}

static int expect_long(tkToken **token, long *contents, Buffer *error)
{
    if ((*token)->type != TT_LONG) {
        expected(TT_LONG, *token, error);
        return 1;
    }
    else {
        *contents = (*token)->l;
        *token = listNext(*token);
        return 0;
    }
}

static int expect_bool(tkToken **token, bool *contents, Buffer *error)
{
    if ((*token)->type != TT_BOOL) {
        expected(TT_BOOL, *token, error);
        return 1;
    }
    else {
        *contents = (*token)->b;
        *token = listNext(*token);
        return 0;
    }
}

static int expect_float(tkToken **token, double *contents, Buffer *error)
{
    if ((*token)->type == TT_LONG) {
        *contents = (*token)->l;
        *token = listNext(*token);
        return 0;
    }
    else if ((*token)->type == TT_DOUBLE) {
        *contents = (*token)->d;
        *token = listNext(*token);
        return 0;
    }
    else {
        expected(TT_DOUBLE, *token, error);
        return 1;
    }
}

static int expect_string(tkToken **token, tkType type, char **contents, Buffer *error)
{
    if ((*token)->type != type) {
        expected(type, *token, error);
        return 1;
    }
    else {
        *contents = (*token)->s;
        *token = listNext(*token);
        return 0;
    }
}

static int process_const(Definition *def, List *defs, tkToken **token, Buffer *error)
{
    char *item_type;

    Definition *type_def;

    def->type = DT_CONST;

    if (expect_string(token, TT_USTRING, &item_type, error) != 0) {
        fprintf(stderr, "Got const type \"%s\"\n", item_type);
        return 1;
    }
    else if ((type_def = find_def(defs, item_type)) == NULL) {
        bufSetF(error, "%s:%d:%d: unknown type: \"%s\".\n",
                (*token)->file, (*token)->line, (*token)->column, item_type);
        return 1;
    }
    else if (type_def->type == DT_INT) {
        if (expect_long(token, &def->const_def.value.l, error) != 0) return 1;
    }
    else if (type_def->type == DT_BOOL) {
        if (expect_bool(token, &def->const_def.value.b, error) != 0) return 1;
    }
    else if (type_def->type == DT_FLOAT) {
        if (expect_float(token, &def->const_def.value.d, error) != 0) return 1;
    }
    else if (type_def->type == DT_ASTRING || type_def->type == DT_WSTRING) {
        if (expect_string(token, TT_DSTRING, &def->const_def.value.s, error) != 0) return 1;
    }
    else {
        bufSetF(error, "%s:%d:%d: invalid const type %s.\n",
            (*token)->file, (*token)->line, (*token)->column,
            deftype_enum_to_string(type_def->type));
        return 1;
    }

    def->const_def.const_type = type_def;

    listAppendTail(defs, def);

    return 0;
}

static int process_array(Definition *def, List *defs, tkToken **token, Buffer *error)
{
    char *item_type, *item_name;

    Definition *item_def;

    def->type = DT_ARRAY;

    if (expect_token(token, TT_OPAREN, error) != 0) {
        return 1;
    }
    else if (expect_string(token, TT_USTRING, &item_type, error) != 0) {
        return 1;
    }
    else if ((item_def = find_def(defs, item_type)) == NULL) {
        bufSetF(error, "%s:%d:%d: unknown type: \"%s\".\n",
                (*token)->file, (*token)->line, (*token)->column, item_type);
        return 1;
    }
    else if (item_def->type == DT_VOID) {
        bufSetF(error, "%s:%d:%d: can not have an array of void.\n",
                (*token)->file, (*token)->line, (*token)->column);
        return 1;
    }
    else if (expect_string(token, TT_USTRING, &item_name, error) != 0) {
        return 1;
    }
    else if (expect_token(token, TT_CPAREN, error) != 0) {
        return 1;
    }

    def->array_def.item_type = item_def;
    def->array_def.item_name = strdup(item_name);

    listAppendTail(defs, def);

    return 0;
}

static int process_struct(Definition *def, List *defs, tkToken **token, Buffer *error)
{
    if (expect_token(token, TT_OBRACE, error) != 0) {
        return 1;
    }

    bool optional = false;

    def->type = DT_STRUCT;

    while ((*token)->type == TT_USTRING) {
        char *elem_name;
        char *elem_type = (*token)->s;

        if (strcmp(elem_type, "opt") == 0) {
            if (optional) {
                bufSetF(error, "%s:%d:%d: multiple \"opt\" keywords.\n",
                        (*token)->file, (*token)->line, (*token)->column);
                return 1;
            }
            else {
                optional = true;

                *token = listNext(*token);

                continue;
            }
        }

        Definition *elem_def = find_def(defs, elem_type);

        if (elem_def == NULL) {
            bufSetF(error, "%s:%d:%d: unknown type: \"%s\".\n",
                    (*token)->file, (*token)->line, (*token)->column, elem_type);
            return 1;
        }
        else if (elem_def->type == DT_VOID) {
            bufSetF(error, "%s:%d:%d: can not have void as structure element.\n",
                (*token)->file, (*token)->line, (*token)->column);
            return 1;
        }

        *token = listNext(*token);

        if (expect_string(token, TT_USTRING, &elem_name, error) != 0) {
            return 1;
        }

        StructItem *item = calloc(1, sizeof(*item));

        item->name = strdup(elem_name);
        item->def = elem_def;
        item->optional = optional;

        listAppendTail(&def->struct_def.items, item);

        optional = false;
    }

    if (expect_token(token, TT_CBRACE, error) != 0) {
        return 1;
    }

    listAppendTail(defs, def);

    return 0;
}

static int process_enum(Definition *def, List *defs, tkToken **token, Buffer *error)
{
    long next_value = 0, max_value = 0;

    if (expect_token(token, TT_OBRACE, error) != 0) {
        return 1;
    }

    def->type = DT_ENUM;

    while ((*token)->type == TT_USTRING) {
        char *item_name = (*token)->s;

        EnumItem *item = calloc(1, sizeof(*item));

        item->name = strdup(item_name);

        *token = listNext(*token);

        if ((*token)->type == TT_USTRING || (*token)->type == TT_CBRACE) {
            item->value = next_value++;
        }
        else if (expect_token(token, TT_EQUALS, error) != 0) {
            return 1;
        }
        else if (expect_long(token, &item->value, error) != 0) {
            return 1;
        }
        else {
            next_value = item->value + 1;
        }

        max_value = MAX(max_value, item->value);

        listAppendTail(&def->enum_def.items, item);
    }

    if (expect_token(token, TT_CBRACE, error) != 0) {
        return 1;
    }

    if (max_value >= (1 << 24))
        def->enum_def.num_bytes = 4;
    else if (max_value >= (1 << 16))
        def->enum_def.num_bytes = 3;
    else if (max_value >= (1 << 8))
        def->enum_def.num_bytes = 2;
    else
        def->enum_def.num_bytes = 1;

    listAppendTail(defs, def);

    return 0;
}

static int process_union(Definition *def, List *defs, tkToken **token, Buffer *error)
{
    char *discr_type, *discr_name;
    Definition *discr_def;

    def->type = DT_UNION;

    if (expect_token(token, TT_OPAREN, error) != 0) {
        return 1;
    }
    else if (expect_string(token, TT_USTRING, &discr_type, error) != 0) {
        return 1;
    }
    else if ((discr_def = find_def(defs, discr_type)) == NULL) {
        bufSetF(error, "%s:%d:%d: unknown type: \"%s\".\n",
                (*token)->file, (*token)->line, (*token)->column, discr_type);
        return 1;
    }
    else if (!is_integer_type(discr_def)) {
        bufSetF(error, "%s:%d:%d: can't use %s as discriminator type.\n",
                (*token)->file, (*token)->line, (*token)->column, discr_type);
        return 1;
    }
    else if (expect_string(token, TT_USTRING, &discr_name, error) != 0) {
        return 1;
    }
    else if (expect_token(token, TT_CPAREN, error) != 0) {
        return 1;
    }
    else if (expect_token(token, TT_OBRACE, error) != 0) {
        return 1;
    }

    def->union_def.discr_def = discr_def;
    def->union_def.discr_name = strdup(discr_name);

    while ((*token)->type == TT_USTRING) {
        char *item_type, *item_name;
        char *discr_value = (*token)->s;
        Definition *item_def;

        UnionItem *item = calloc(1, sizeof(*item));

        item->value = strdup(discr_value);

        *token = listNext(*token);

        if (expect_token(token, TT_COLON, error) != 0) {
            return 1;
        }
        else if (expect_string(token, TT_USTRING, &item_type, error) != 0) {
            return 1;
        }
        else if ((item_def = find_def(defs, item_type)) == NULL) {
            bufSetF(error, "%s:%d:%d: unknown type: \"%s\".\n",
                    (*token)->file, (*token)->line, (*token)->column, item_type);
            return 1;
        }
        else if (item_def->type == DT_VOID) {
            item->def = item_def;
            item->name = NULL;

            listAppendTail(&def->union_def.items, item);

            continue;
        }
        else if (expect_string(token, TT_USTRING, &item_name, error) != 0) {
            return 1;
        }

        item->def = item_def;
        item->name = strdup(item_name);

        listAppendTail(&def->union_def.items, item);
    }

    if (expect_token(token, TT_CBRACE, error) != 0) {
        return 1;
    }

    listAppendTail(defs, def);

    return 0;
}

char *parse(const char *filename, List *definitions)
{
    List tokens = { };
    Buffer error = { };

    tkToken *token;
    Definition *cur_def, *alias_def;

    char *err = tokFile(filename, &tokens);

    if (err != NULL) {
        return err;
    }

    State state = ST_INITIAL;
    int inc_level = 0;

    token = listHead(&tokens);

    while (token->type != TT_EOF) {
        if (!bufIsEmpty(&error)) {
            return bufDetach(&error);
        }

        switch(state) {
        case ST_INITIAL:
            if (token->type == TT_INC_ENTRY) {
                inc_level++;

                if (inc_level == 1 && find_def(definitions, token->s) == NULL) {
                    Definition *inc_def = create_def(token->s, token->file, token->line, inc_level);
                    inc_def->type = DT_INCLUDE;
                    listAppendTail(definitions, inc_def);
                }
            }
            else if (token->type == TT_INC_EXIT) {
                inc_level--;
            }
            else if (token->type == TT_USTRING) {
                cur_def = create_def(token->s, token->file, token->line, inc_level);
                state = ST_NAME;
            }
            else {
                expected(TT_USTRING, token, &error);
            }

            token = listNext(token);

            break;
        case ST_NAME:
            if (token->type == TT_EQUALS) {
                token = listNext(token);
                state = ST_EQUALS;
            }
            else {
                expected(TT_EQUALS, token, &error);
            }
            break;
        case ST_EQUALS:
            if (token->type != TT_USTRING) {
                expected(TT_USTRING, token, &error);
            }
            else if (strcmp(token->s, "const") == 0) {
                token = listNext(token);
                state = ST_CONST;
            }
            else if (strcmp(token->s, "array") == 0) {
                token = listNext(token);
                state = ST_ARRAY;
            }
            else if (strcmp(token->s, "struct") == 0) {
                token = listNext(token);
                state = ST_STRUCT;
            }
            else if (strcmp(token->s, "enum") == 0) {
                token = listNext(token);
                state = ST_ENUM;
            }
            else if (strcmp(token->s, "union") == 0) {
                token = listNext(token);
                state = ST_UNION;
            }
            else if ((alias_def = find_def(definitions, token->s)) != NULL) {
                cur_def->type = DT_ALIAS;
                cur_def->alias_def.alias = alias_def;
                listAppendTail(definitions, cur_def);
                token = listNext(token);
                state = ST_INITIAL;
            }
            else {
                bufSetF(&error, "%s:%d:%d: unknown base type \"%s\".\n",
                        token->file, token->line, token->column, token->s);
            }
            break;
        case ST_CONST:
            if (process_const(cur_def, definitions, &token, &error) == 0) {
                state = ST_INITIAL;
            }
            break;
        case ST_ARRAY:
            if (process_array(cur_def, definitions, &token, &error) == 0) {
                state = ST_INITIAL;
            }
            break;
        case ST_STRUCT:
            if (process_struct(cur_def, definitions, &token, &error) == 0) {
                state = ST_INITIAL;
            }
            break;
        case ST_ENUM:
            if (process_enum(cur_def, definitions, &token, &error) == 0) {
                state = ST_INITIAL;
            }
            break;
        case ST_UNION:
            if (process_union(cur_def, definitions, &token, &error) == 0) {
                state = ST_INITIAL;
            }
            break;
        default:
            fprintf(stderr, "Unexpected state %d\n", state);
            abort();
        }
    }

    return NULL;
}
