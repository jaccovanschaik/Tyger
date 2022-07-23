#ifndef PARSER_H
#define PARSER_H

/* parser.h: Parser for Tyger files.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-25
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#include "deftype.h"

#include <libjvs/list.h>

#include <stdbool.h>

typedef struct Definition Definition;

typedef struct {
    Definition *const_type;
    union {
        long   l;
        bool   b;
        double d;
        char  *s;
    } value;
} ConstDefinition;

typedef struct {
    int size;
    int is_signed;
} IntDefinition;

typedef struct {
    int size;
} FloatDefinition;

typedef struct {
} StringDefinition;

typedef struct {
    Definition *alias;
} AliasDefinition;

typedef struct {
    Definition *item_type;
    char *item_name;
} ArrayDefinition;

typedef struct {
    ListNode _node;
    char *name;
    Definition *def;
} StructItem;

typedef struct {
    List items;
} StructDefinition;

typedef struct {
    ListNode _node;
    char *name;
    long value;
} EnumItem;

typedef struct {
    List items;
} EnumDefinition;

typedef struct {
    ListNode _node;
    char *value;
    Definition *def;
    char *name;
} UnionItem;

typedef struct {
    char *discr_name;
    Definition *discr_def;
    List items;
} UnionDefinition;

struct Definition {
    ListNode _node;
    char *name;
    int builtin;
    DefinitionType type;
    union {
        ConstDefinition const_def;
        IntDefinition int_def;
        FloatDefinition float_def;
        StringDefinition string_def;
        AliasDefinition alias_def;
        ArrayDefinition array_def;
        StructDefinition struct_def;
        EnumDefinition enum_def;
        UnionDefinition union_def;
    };
};

char *parse(const char *filename, List *definitions);

#endif
