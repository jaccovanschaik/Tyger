/* lang-c.c: Generate C code.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-24
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <libjvs/list.h>
#include <libjvs/utils.h>

#include "switches.h"
#include "parser.h"
#include "utils.h"
#include "lang-c.h"

static int do_packsize = 0;
static int do_pack = 0;
static int do_unpack = 0;
static int do_wrap = 0;
static int do_unwrap = 0;
static int do_read_fd = 0;
static int do_write_fd = 0;
static int do_read_fp = 0;
static int do_write_fp = 0;
static int do_print_fp = 0;
static int do_create = 0;
static int do_set = 0;
static int do_copy = 0;
static int do_clear = 0;
static int do_destroy = 0;
static int do_mx_send = 0;
static int do_mx_bcast = 0;

static Switch switches[] = {
    { "--c-packsize", &do_packsize, "Generate packsize functions" },
    { "--c-pack",     &do_pack,     "Generate pack functions" },
    { "--c-unpack",   &do_unpack,   "Generate unpack functions" },
    { "--c-wrap",     &do_wrap,     "Generate wrap functions" },
    { "--c-unwrap",   &do_unwrap,   "Generate unwrap functions" },
    { "--c-read-fd",  &do_read_fd,  "Generate functions to read from an fd" },
    { "--c-write-fd", &do_write_fd, "Generate functions to write to an fd" },
    { "--c-read-fp",  &do_read_fp,  "Generate functions to read from an FP" },
    { "--c-write-fp", &do_write_fp, "Generate functions to write to an FP" },
    { "--c-print-fp", &do_print_fp, "Generate print functions" },
    { "--c-create",   &do_create,   "Generate create functions" },
    { "--c-set",      &do_set,      "Generate set functions" },
    { "--c-copy",     &do_copy,     "Generate copy functions" },
    { "--c-clear",    &do_clear,    "Generate clear functions" },
    { "--c-destroy",  &do_destroy,  "Generate destroy functions" },
    { "--c-mx-send",  &do_mx_send,  "Generate MX send functions" },
    { "--c-mx-bcast", &do_mx_bcast, "Generate MX broadcast functions" },
};

static int num_switches = sizeof(switches) / sizeof(switches[0]);

typedef enum {
    FILE_ID_FD,
    FILE_ID_FP
} FileIdentifier;

typedef struct {
    char *suffix;
    char *vartype;
    char *varname;
} FileAttributes;

static FileAttributes file_attr[] = {
    { "FD", "int ", "fd" },
    { "FP", "FILE *", "fp" },
};

static const char *include_guard_name(const char *filename)
{
    static char return_buffer[PATH_MAX];

    char *p;
    char *duplicate = strdup(filename);
    char *base_name = basename(duplicate);

    for (p = base_name; *p != '\0'; p++) {
        if (*p == '.' || *p == '-')
            *p = '_';
        else if (isalpha(*p))
            *p = toupper(*p);
    }

    strncpy(return_buffer, base_name, sizeof(return_buffer));

    return return_buffer;
}

static const char *associated_header_file(const char *filename)
{
    static char return_buffer[PATH_MAX];

    char *p;
    char *duplicate = strdup(filename);
    char *base_name = basename(duplicate);

    strncpy(return_buffer, base_name, sizeof(return_buffer));

    p = strrchr(return_buffer, '.');

    if (p == NULL) {
        return NULL;
    }

    strcpy(p, ".h");

    return return_buffer;
}

static const char *equivalent_c_type(const Definition *def)
{
    static char name[32];

    if (!def->builtin) {
        snprintf(name, sizeof(name), "%s ", def->name);

        return name;
    }
    else if (def->type == DT_INT) {
        snprintf(name, sizeof(name), "%sint%d_t ",
                def->int_def.is_signed ? "" : "u",
                def->int_def.size * 8);

        return name;
    }
    else if (def->type == DT_FLOAT) {
        if (def->float_def.size == 4) {
            return "float ";
        }
        else {
            return "double ";
        }
    }
    else if (def->type == DT_ASTRING) {
        return "char *";
    }
    else if (def->type == DT_USTRING) {
        return "wchar_t *";
    }
    else {
        return NULL;
    }
}

static Definition *effective_definition(Definition *def)
{
    if (def->type == DT_ALIAS) {
        return effective_definition(def->alias_def.alias);
    }
    else {
        return def;
    }
}

static const char *const_double_pointer_cast(Definition *def)
{
    if (def->type == DT_ASTRING) {
        return "(const char **) ";
    }
    else if (def->type == DT_USTRING) {
        return "(const wchar_t **) ";
    }
    else {
        return "";
    }
}

static void emit_const_declaration(FILE *fp, Definition *def)
{
    fprintf(fp, "\nextern const %s%s;\n",
            equivalent_c_type(def->const_def.const_type),
            def->name);
}

static void emit_const_definition(FILE *fp, Definition *def)
{
    fprintf(fp, "\nconst %s%s = ",
            equivalent_c_type(def->const_def.const_type), def->name);

    switch (def->const_def.const_type->type) {
    case DT_INT:
        fprintf(fp, "%ld;\n\n", def->const_def.value.l);
        break;
    case DT_FLOAT:
        fprintf(fp, "%g;\n\n", def->const_def.value.d);
        break;
    case DT_ASTRING:
        fprintf(fp, "\"%s\";\n\n", def->const_def.value.s);
        break;
    case DT_USTRING:
        fprintf(fp, "L\"%s\";\n\n", def->const_def.value.s);
        break;
    default:
        break;
    }
}

static void emit_typedef(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    EnumItem *enum_item;
    UnionItem *union_item;

    switch(def->type) {
    case DT_ALIAS:
        fprintf(fp, "\ntypedef %s%s;\n",
                equivalent_c_type(def->alias_def.alias),
                def->name);
        break;
    case DT_ARRAY:
        fprintf(fp, "\ntypedef struct {\n");
        ifprintf(fp, 1, "uint32_t count;\n");
        ifprintf(fp, 1, "%s *%s;\n",
                equivalent_c_type(def->array_def.item_type),
                def->array_def.item_name);
        fprintf(fp, "} %s;\n", def->name);
        break;
    case DT_STRUCT:
        fprintf(fp, "\ntypedef struct {\n");

        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "%s%s;\n",
                    equivalent_c_type(struct_item->def),
                    struct_item->name);
        }

        fprintf(fp, "} %s;\n", def->name);

        break;
    case DT_ENUM:
        fprintf(fp, "\ntypedef enum {\n");

        for (enum_item = listHead(&def->enum_def.items);
             enum_item; enum_item = listNext(enum_item)) {
            ifprintf(fp, 1, "%s = %ld,\n",
                    enum_item->name, enum_item->value);
        }

        fprintf(fp, "} %s;\n", def->name);

        break;
    case DT_UNION:
        fprintf(fp, "\ntypedef struct {\n");

        ifprintf(fp, 1, "%s %s;\n",
                def->union_def.discr_def->name,
                def->union_def.discr_name);

        ifprintf(fp, 1, "union {\n");

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            if (is_void_type(union_item->def)) continue;

            ifprintf(fp, 2, "%s %s;\n", union_item->def->name, union_item->name);
        }
        ifprintf(fp, 1, "};\n");

        fprintf(fp, "} %s;\n", def->name);

        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }
}

static void emit_packsize_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Return the number of bytes required to pack <data>.\n");
    fprintf(fp, " */\n");
    fprintf(fp, "size_t %sPackSize(const %s *data)", def->name, def->name);
}

static void emit_packsize_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "return %sPackSize(data);\n", def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n");
        ifprintf(fp, 1, "size_t size = uint32PackSize(&data->count);\n\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "size += %sPackSize(data->%s + i);\n",
                def->array_def.item_type->name,
                def->array_def.item_name);
        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return size;\n");
        break;
    case DT_STRUCT:
        ifprintf(fp, 1, "size_t size = 0;\n\n");

        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "size += %sPackSize(%s&data->%s);%s",
                    struct_item->def->name,
                    const_double_pointer_cast(struct_item->def),
                    def->array_def.item_name,
                    listNext(struct_item) == NULL ? "\n\n" : "\n");
        }

        ifprintf(fp, 1, "return size;\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return sizeof(uint32_t);\n");
        break;
    case DT_UNION:
        ifprintf(fp, 1, "size_t size = %sPackSize(%s&data->%s);\n\n",
                def->union_def.discr_def->name,
                const_double_pointer_cast(def->union_def.discr_def),
                def->union_def.discr_name);
        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {

            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "size += %sPackSize(%s&data->%s);\n",
                        union_item->def->name,
                        const_double_pointer_cast(union_item->def),
                        union_item->name);
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return size;\n");
        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_pack_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp,
            "\n/*\n"
            " * Pack <data> into <buffer>, enlarging it if necessary. <data>\n"
            " * points to the data to write, <buffer> is a pointer to a pointer\n"
            " * to the start of the buffer, <size> points to its current size\n"
            " * and <pos> points to the position in the buffer where the data is\n"
            " * to be written. The contents of <buffer> and <size> are updated\n"
            " * if the buffer is enlarged. <pos> is updated with the new write\n"
            " * position. The number of bytes written is returned.\n"
            " */\n");
    fprintf(fp,
            "size_t %sPack(const %s *data, char **buffer, size_t *size, size_t *pos)",
            def->name, def->name);
}

static void emit_pack_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "return %sPack(data, buffer, size, pos);\n",
                def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n");
        ifprintf(fp, 1,
                "size_t byte_count = uint32Pack(&data->count, buffer, size, pos);\n\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "byte_count += %sPack(%sdata->%s + i, buffer, size, pos);\n",
                def->array_def.item_type->name,
                const_double_pointer_cast(def->array_def.item_type),
                def->array_def.item_name);
        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return byte_count;\n");
        break;
    case DT_STRUCT:
        ifprintf(fp, 1, "size_t byte_count = 0;\n\n");

        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "byte_count += %sPack(%s&data->%s, buffer, size, pos);%s",
                    struct_item->def->name,
                    const_double_pointer_cast(struct_item->def),
                    struct_item->name,
                    listNext(struct_item) == NULL ? "\n\n" : "\n");
        }

        ifprintf(fp, 1, "return byte_count;\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return uint32Pack(data, buffer, size, pos);\n");
        break;
    case DT_UNION:
        ifprintf(fp, 1, "size_t byte_count = %sPack(%s&data->%s, buffer, size, pos);\n\n",
                def->union_def.discr_def->name,
                const_double_pointer_cast(def->union_def.discr_def),
                def->union_def.discr_name);
        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "byte_count += %sPack(%s&data->%s, buffer, size, pos);\n",
                        union_item->def->name,
                        const_double_pointer_cast(union_item->def),
                        union_item->name);
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return byte_count;\n");
        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_unpack_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp,
            "\n/*\n"
            " * Unpack <data> from <buffer>, which is <size> bytes in size.\n"
            " */\n");
    fprintf(fp, "size_t %sUnpack(const char *buffer, size_t size, %s *data)",
            def->name, def->name);
}

static void emit_unpack_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "return %sUnpack(buffer, size, data);\n", def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n");
        ifprintf(fp, 1, "uint32_t old_count = data->count;\n");
        ifprintf(fp, 1, "size_t offset = uint32Unpack(buffer, size, &data->count);\n\n");

        ifprintf(fp, 1, "data->%s = realloc(data->%s, data->count * sizeof(%s));\n\n",
                def->array_def.item_name,
                def->array_def.item_name,
                equivalent_c_type(def->array_def.item_type));

        ifprintf(fp, 1, "if (data->count > old_count) {\n");
        ifprintf(fp, 2, "memset(data->%s + old_count, 0, "
            "sizeof(%s) * (data->count - old_count));\n",
                def->array_def.item_name,
                equivalent_c_type(def->array_def.item_type));
        ifprintf(fp, 1, "}\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "offset += %sUnpack(buffer + offset, "
            "size > offset ? size - offset : 0, data->%s + i);\n",
                def->array_def.item_type->name, def->array_def.item_name);

        ifprintf(fp, 1, "}\n\n");

        ifprintf(fp, 1, "return offset;\n");
        break;
    case DT_STRUCT:
        ifprintf(fp, 1, "size_t offset = 0;\n\n");

        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "offset += %sUnpack(buffer + offset, "
                "size > offset ? size - offset : 0, &data->%s);%s",
                    struct_item->def->name, struct_item->name,
                    listNext(struct_item) == NULL ? "\n\n" : "\n");
        }

        ifprintf(fp, 1, "return offset;\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return uint32Unpack(buffer, size, data);\n");
        break;
    case DT_UNION:
        ifprintf(fp, 1, "size_t offset = %sUnpack(buffer, size, &data->%s);\n\n",
                def->union_def.discr_def->name,
                def->union_def.discr_name);
        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "offset += %sUnpack(buffer + offset, "
                    "size > offset ? size - offset : 0, &data->%s);\n",
                        union_item->def->name, union_item->name);
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return offset;\n");
        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_wrap_signature(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp,
            "\n/*\n"
            " * Pack the given %s fields into <buffer>, enlarging it as\n"
            " * necessary. <size> and <pos> are the current size of\n"
            " * <buffer> and the position upto which it is filled,\n"
            " * and they are updated if necessary.\n"
            " */\n", def->name);
    fprintf(fp,
            "size_t %sWrap(char **buffer, size_t *size, size_t *pos", def->name);

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item))
    {
        Definition *def = effective_definition(struct_item->def);

        int indirect = !(def->type == DT_INT || def->type == DT_FLOAT || def->type == DT_ENUM ||
                         def->type == DT_ASTRING || def->type == DT_USTRING);

        int string = def->type == DT_ASTRING || def->type == DT_USTRING;

        if (listPrev(struct_item) == NULL) {
            fprintf(fp, ",\n");
            findent(fp, 2);
        }
        else {
            fprintf(fp, ", ");
        }

        fprintf(fp, "%s%s%s%s",
                indirect || string ? "const " : "",
                equivalent_c_type(struct_item->def),
                indirect ? "*" : "",
                struct_item->name);
    }

    fprintf(fp, ")");
}

static void emit_wrap_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp, "\n{\n");

    ifprintf(fp, 1, "size_t byte_count = 0;\n\n");

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item)) {
        Definition *def = effective_definition(struct_item->def);

        int direct = def->type == DT_INT || def->type == DT_FLOAT || def->type == DT_ENUM
                  || def->type == DT_ASTRING || def->type == DT_USTRING;

        ifprintf(fp, 1, "byte_count += %sPack(%s%s%s, buffer, size, pos);\n",
                struct_item->def->name,
                const_double_pointer_cast(struct_item->def),
                direct ? "&" : "",
                struct_item->name);
    }

    fprintf(fp, "\n");

    ifprintf(fp, 1, "return byte_count;\n");

    fprintf(fp, "}\n");
}

static void emit_unwrap_signature(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp,
            "\n/*\n"
            " * Unpack the given %s fields from <buffer>, which has size\n"
            " * <size>.\n"
            " */\n", def->name);
    fprintf(fp,
            "size_t %sUnwrap(char *buffer, size_t size", def->name);

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item))
    {
        if (listPrev(struct_item) == NULL) {
            fprintf(fp, ",\n");
            findent(fp, 2);
        }
        else {
            fprintf(fp, ", ");
        }

        fprintf(fp, "%s*%s",
                equivalent_c_type(struct_item->def),
                struct_item->name);
    }

    fprintf(fp, ")");
}

static void emit_unwrap_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp, "\n{\n");

    ifprintf(fp, 1, "size_t offset = 0;\n\n");

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item)) {
        ifprintf(fp, 1, "offset += %sUnpack(buffer + offset, size > offset ? size - offset : 0, %s);\n",
                struct_item->def->name,
                struct_item->name);
    }

    fprintf(fp, "\n");

    ifprintf(fp, 1, "return offset;\n");

    fprintf(fp, "}\n");
}

static void emit_read_signature(FILE *fp, Definition *def, FileAttributes *attr)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Read a binary representation of <data> from <%s>.\n", attr->varname);
    fprintf(fp, " */\n");
    fprintf(fp, "size_t %sReadFrom%s(%s%s, %s *data)",
            def->name, attr->suffix, attr->vartype, attr->varname, def->name);
}

static void emit_read_body(FILE *fp, Definition *def, FileAttributes *attr)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "return %sReadFrom%s(%s, data);\n",
                def->alias_def.alias->name, attr->suffix, attr->varname);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "size_t byte_count = 0;\n");
        ifprintf(fp, 1, "uint32_t i, old_count = data->count;\n\n");
        ifprintf(fp, 1, "byte_count += uint32ReadFrom%s(%s, &data->count);\n\n",
                attr->suffix, attr->varname);

        ifprintf(fp, 1, "data->%s = realloc(data->%s, data->count * sizeof(%s));\n\n",
                def->array_def.item_name,
                def->array_def.item_name,
                equivalent_c_type(def->array_def.item_type));

        ifprintf(fp, 1, "if (data->count > old_count) {\n");
        ifprintf(fp, 2, "memset(data->%s + old_count, 0, "
            "sizeof(%s) * (data->count - old_count));\n",
                def->array_def.item_name,
                equivalent_c_type(def->array_def.item_type));
        ifprintf(fp, 1, "}\n\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "byte_count += %sReadFrom%s(%s, data->%s + i);\n",
                def->array_def.item_type->name,
                attr->suffix, attr->varname,
                def->array_def.item_name);
        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return(byte_count);\n");
        break;
    case DT_STRUCT:
        ifprintf(fp, 1, "size_t byte_count = 0;\n\n");

        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "byte_count += %sReadFrom%s(%s, &data->%s);\n",
                    struct_item->def->name, attr->suffix, attr->varname,
                    struct_item->name);
        }

        fputc('\n', fp);

        ifprintf(fp, 1, "return byte_count;\n");

        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return uint32ReadFrom%s(%s, data);\n",
                attr->suffix, attr->varname);

        break;
    case DT_UNION:
        ifprintf(fp, 1, "size_t byte_count = %sReadFrom%s(%s, &data->%s);\n\n",
                def->union_def.discr_def->name,
                attr->suffix, attr->varname,
                def->union_def.discr_name);

        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "byte_count += %sReadFrom%s(%s, &data->%s);\n",
                        union_item->def->name,
                        attr->suffix, attr->varname,
                        union_item->name);
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return byte_count;\n");

        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_write_signature(FILE *fp, Definition *def, FileAttributes *attr)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Write a binary representation of <data> to <%s>.\n", attr->varname);
    fprintf(fp, " */\n");
    fprintf(fp, "size_t %sWriteTo%s(%s%s, const %s *data)",
            def->name, attr->suffix, attr->vartype, attr->varname, def->name);
}

static void emit_write_body(FILE *fp, Definition *def, FileAttributes *attr)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "return %sWriteTo%s(%s, data);\n",
                def->alias_def.alias->name, attr->suffix, attr->varname);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n");
        ifprintf(fp, 1,
                "size_t byte_count = uint32WriteTo%s(%s, &data->count);\n\n",
                attr->suffix, attr->varname);

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "byte_count += %sWriteTo%s(%s, data->%s + i);\n",
                def->array_def.item_type->name,
                attr->suffix, attr->varname,
                def->array_def.item_name);
        ifprintf(fp, 1, "}\n\n");

        ifprintf(fp, 1, "return byte_count;\n");
        break;
    case DT_STRUCT:
        ifprintf(fp, 1, "size_t byte_count = 0;\n\n");

        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "byte_count += %sWriteTo%s(%s, &data->%s);\n",
                    struct_item->def->name, attr->suffix, attr->varname,
                    struct_item->name);
        }

        fputc('\n', fp);

        ifprintf(fp, 1, "return byte_count;\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return uint32WriteTo%s(%s, data);\n",
                attr->suffix, attr->varname);

        break;
    case DT_UNION:
        ifprintf(fp, 1, "size_t byte_count = %sWriteTo%s(%s, &data->%s);\n\n",
                def->union_def.discr_def->name,
                attr->suffix, attr->varname,
                def->union_def.discr_name);

        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);
            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "byte_count += %sWriteTo%s(%s, &data->%s);\n",
                        union_item->def->name,
                        attr->suffix, attr->varname,
                        union_item->name);
            }
            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return byte_count;\n");

        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_print_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Print an ASCII representation of <data> to <fp>.\n");
    fprintf(fp, " */\n");
    fprintf(fp, "void %sPrint(FILE *fp, const %s *data, int level)", def->name, def->name);
}

static void emit_print_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;
    EnumItem *enum_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "%sPrint(fp, data, level);\n", def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n\n");
        ifprintf(fp, 1, "fprintf(fp, \"{\\n\");\n\n");
        ifprintf(fp, 1, "level++;\n\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "fprintf(fp, \"%%s%s: \", indent(level));\n",
                def->array_def.item_name);
        ifprintf(fp, 2, "%sPrint(fp, %s(data->%s + i), level);\n",
                def->array_def.item_type->name,
                const_double_pointer_cast(def->array_def.item_type),
                def->array_def.item_name);
        ifprintf(fp, 2, "fprintf(fp, \"\\n\");\n");

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "level--;\n\n");
        ifprintf(fp, 1, "fprintf(fp, \"%%s}\", indent(level));\n");
        break;
    case DT_STRUCT:
        ifprintf(fp, 1, "fprintf(fp, \"{\\n\");\n\n");

        if (!listIsEmpty(&def->struct_def.items)) {
            ifprintf(fp, 1, "level++;\n\n");

            for (struct_item = listHead(&def->struct_def.items);
                struct_item; struct_item = listNext(struct_item)) {
                ifprintf(fp, 1, "fprintf(fp, \"%%s%s: \", indent(level));\n", struct_item->name);
                ifprintf(fp, 1, "%sPrint(fp, %s&data->%s, level);\n",
                        struct_item->def->name,
                        const_double_pointer_cast(struct_item->def),
                        struct_item->name);
                ifprintf(fp, 1, "fprintf(fp, \"\\n\");\n\n");
            }

            ifprintf(fp, 1, "level--;\n\n");
        }

        ifprintf(fp, 1, "fprintf(fp, \"%%s}\", indent(level));\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "switch(*data) {\n");

        for (enum_item = listHead(&def->enum_def.items);
             enum_item; enum_item = listNext(enum_item)) {
            ifprintf(fp, 1, "case %s:\n", enum_item->name);
            ifprintf(fp, 2, "fprintf(fp, \"%s\");\n", enum_item->name);
            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n");
        break;
    case DT_UNION:
        ifprintf(fp, 1, "%sPrint(fp, &data->%s, level);\n\n",
                def->union_def.discr_def->name, def->union_def.discr_name);

        ifprintf(fp, 1, "fprintf(fp, \" \");\n\n");

        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "%sPrint(fp, &data->%s, level);\n",
                        union_item->def->name,
                        union_item->name);
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");

        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_create_signature(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp,
            "\n/*\n"
            " * Create a new %s using the given values.\n"
            " */\n", def->name);
    fprintf(fp,
            "%s *%sCreate(", def->name, def->name);

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item))
    {
        Definition *def = effective_definition(struct_item->def);

        int indirect = !(def->type == DT_INT || def->type == DT_FLOAT || def->type == DT_ENUM ||
                         def->type == DT_ASTRING || def->type == DT_USTRING);

        int string = def->type == DT_ASTRING || def->type == DT_USTRING;

        if (listPrev(struct_item) != NULL) {
            fprintf(fp, ", ");
        }

        fprintf(fp, "%s%s%s%s",
                indirect || string ? "const " : "",
                equivalent_c_type(struct_item->def),
                indirect ? "*" : "",
                struct_item->name);
    }

    fprintf(fp, ")");
}

static void emit_create_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp, "\n{\n");

    ifprintf(fp, 1, "%s *data = calloc(1, sizeof(%s));\n\n", def->name, def->name);

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item)) {
        Definition *def = effective_definition(struct_item->def);

        int direct = def->type == DT_INT || def->type == DT_FLOAT || def->type == DT_ENUM
                  || def->type == DT_ASTRING || def->type == DT_USTRING;

        ifprintf(fp, 1, "%sCopy(&data->%s, %s%s);\n",
                struct_item->def->name,
                struct_item->name,
                direct ? "&" : "",
                struct_item->name);
    }

    fprintf(fp, "\n");

    ifprintf(fp, 1, "return data;\n");

    fprintf(fp, "}\n");
}

static void emit_set_signature(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp,
            "\n/*\n"
            " * Set the contents of %s <dst> using the given fields.\n"
            " */\n", def->name);
    fprintf(fp,
            "void %sSet(%s *dst", def->name, def->name);

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item))
    {
        Definition *def = effective_definition(struct_item->def);

        int indirect = !(def->type == DT_INT || def->type == DT_FLOAT || def->type == DT_ENUM ||
                         def->type == DT_ASTRING || def->type == DT_USTRING);

        int string = def->type == DT_ASTRING || def->type == DT_USTRING;

        fprintf(fp, ", ");

        fprintf(fp, "%s%s%s%s",
                indirect || string ? "const " : "",
                equivalent_c_type(struct_item->def),
                indirect ? "*" : "",
                struct_item->name);
    }

    fprintf(fp, ")");
}

static void emit_set_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;

    assert(def->type == DT_STRUCT);

    fprintf(fp, "\n{\n");

    ifprintf(fp, 1, "assert(dst != NULL);\n\n");

    for (struct_item = listHead(&def->struct_def.items);
         struct_item; struct_item = listNext(struct_item)) {
        Definition *def = effective_definition(struct_item->def);

        int direct = def->type == DT_INT || def->type == DT_FLOAT || def->type == DT_ENUM
                  || def->type == DT_ASTRING || def->type == DT_USTRING;

        ifprintf(fp, 1, "%sCopy(&dst->%s, %s%s);\n",
                struct_item->def->name,
                struct_item->name,
                direct ? "&" : "",
                struct_item->name);
    }

    fprintf(fp, "}\n");
}

static void emit_copy_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Copy the %s <src> to <dst>.\n", def->name);
    fprintf(fp, " */\n");
    fprintf(fp, "void %sCopy(%s *dst, const %s *src)", def->name, def->name, def->name);
}

static void emit_copy_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    ifprintf(fp, 1, "assert(dst != NULL);\n");
    ifprintf(fp, 1, "assert(src != NULL);\n\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "%sCopy(dst, src);\n", def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n\n");
        ifprintf(fp, 1, "if (dst->%s != NULL) {\n",
                def->array_def.item_name);

        ifprintf(fp, 2, "for (i = 0; i < dst->count; i++) {\n");
        ifprintf(fp, 3, "%sClear(%s(dst->%s + i));\n",
                def->array_def.item_type->name,
                const_double_pointer_cast(def->array_def.item_type),
                def->array_def.item_name);
        ifprintf(fp, 2, "}\n\n");

        ifprintf(fp, 2, "free(dst->%s);\n", def->array_def.item_name);
        ifprintf(fp, 1, "}\n\n");

        ifprintf(fp, 1, "dst->count = src->count;\n\n");

        ifprintf(fp, 1, "dst->%s = calloc(dst->count, sizeof(%s));\n\n",
                def->array_def.item_name, def->array_def.item_type->name);

        ifprintf(fp, 1, "for (i = 0; i < dst->count; i++) {\n");
        ifprintf(fp, 2, "%sCopy(dst->%s + i, src->%s + i);\n",
                def->array_def.item_type->name,
                def->array_def.item_name, def->array_def.item_name);
        ifprintf(fp, 1, "}\n");
        break;
    case DT_STRUCT:
        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "%sCopy(&dst->%s, %s&src->%s);\n",
                    struct_item->def->name,
                    struct_item->name,
                    const_double_pointer_cast(struct_item->def),
                    struct_item->name);
        }
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "*dst = *src;\n");
        break;
    case DT_UNION:
        ifprintf(fp, 1, "dst->%s = src->%s;\n\n",
                def->union_def.discr_name, def->union_def.discr_name);

        ifprintf(fp, 1, "switch(src->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);
            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "%sCopy(&dst->%s, %s&src->%s);\n",
                        union_item->def->name,
                        union_item->name,
                        const_double_pointer_cast(union_item->def),
                        union_item->name);
            }
            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n");
        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_clear_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Clear an already used %s.\n", def->name);
    fprintf(fp, " */\n");
    fprintf(fp, "void %sClear(%s *data)", def->name, def->name);
}

static void emit_clear_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "%sClear(data);\n", def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "%sClear(%s(data->%s + i));\n",
                def->array_def.item_type->name,
                const_double_pointer_cast(def->array_def.item_type),
                def->array_def.item_name);
        ifprintf(fp, 1, "}\n\n");

        ifprintf(fp, 1, "free(data->%s);\n\n", def->array_def.item_name);
        ifprintf(fp, 1, "data->count = 0;\n");
        ifprintf(fp, 1, "data->%s = NULL;\n", def->array_def.item_name);
        break;
    case DT_STRUCT:
        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            ifprintf(fp, 1, "%sClear(&data->%s);\n", struct_item->def->name, struct_item->name);
        }
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "*data = 0;\n");
        break;
    case DT_UNION:
        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);
            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "%sClear(&data->%s);\n",
                        union_item->def->name,
                        union_item->name);
            }
            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n");
        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_destroy_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Destroy an already used %s.\n", def->name);
    fprintf(fp, " */\n");
    fprintf(fp, "void %sDestroy(%s *data)", def->name, def->name);
}

static void emit_destroy_body(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");
    ifprintf(fp, 1, "%sClear(data);\n\n", def->name);
    ifprintf(fp, 1, "free(data);\n");
    fprintf(fp, "}\n");
}

static void emit_mx_send_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Send the %s in <data> out over <mx> file descriptor <fd>,\n", def->name);
    fprintf(fp, " * using message type <type> and message version <version>.\n");
    fprintf(fp, " */\n");
    fprintf(fp, "void %sSendMX(MX *mx, int fd, uint32_t type, uint32_t version, %s *data)",
            def->name, def->name);
}

static void emit_mx_send_body(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");
    ifprintf(fp, 1, "char *buf = NULL;\n");
    ifprintf(fp, 1, "size_t size = 0, pos = 0;\n\n");
    ifprintf(fp, 1, "%sPack(data, &buf, &size, &pos);\n\n", def->name);
    ifprintf(fp, 1, "mxSend(mx, fd, type, version, buf, pos);\n\n");
    ifprintf(fp, 1, "free(buf);\n");
    fprintf(fp, "}\n");
}

static void emit_mx_bcast_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Broadcast the %s in <data> to all subscribers to message\n",
            def->name);
    fprintf(fp, " * type <type>, using <version> as the message version.\n");
    fprintf(fp, " */\n");
    fprintf(fp, "void %sBroadcastMX(MX *mx, uint32_t type, uint32_t version, %s *data)",
            def->name, def->name);
}

static void emit_mx_bcast_body(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST) return;

    fprintf(fp, "\n{\n");
    ifprintf(fp, 1, "char *buf = NULL;\n");
    ifprintf(fp, 1, "size_t size = 0, pos = 0;\n\n");
    ifprintf(fp, 1, "%sPack(data, &buf, &size, &pos);\n\n", def->name);
    ifprintf(fp, 1, "mxBroadcast(mx, type, version, buf, pos);\n\n");
    ifprintf(fp, 1, "free(buf);\n");
    fprintf(fp, "}\n");
}

static void set_dependencies(void)
{
    if (do_wrap)    do_pack = TRUE;
    if (do_unwrap)  do_unpack = TRUE;
    if (do_create)  do_copy = TRUE;
    if (do_set)     do_copy = TRUE;
    if (do_copy)    do_clear = TRUE;
    if (do_destroy) do_clear = TRUE;

    if (do_mx_send)  do_pack = TRUE;
    if (do_mx_bcast) do_pack = TRUE;
}

/*
 * Return the switches that the C language generator accepts.
 */
Switch *c_switches(int *switch_count_ptr)
{
    *switch_count_ptr = num_switches;

    return switches;
}

/*
 * Emit C header file.
 */
int emit_c_hdr(const char *out_file, const char *in_file, const char *prog_name, List *definitions)
{
    FILE *fp;

    Definition *def;

    const char *time_str = time_as_string();
    const char *include_guard = include_guard_name(out_file);

    if ((fp = fopen(out_file, "w")) == NULL) {
        fprintf(stderr, "Could not open file \"%s\": %s\n",
            out_file, strerror(errno));
        return 1;
    }

    set_dependencies();

    fprintf(fp, "#ifndef %s\n#define %s\n\n",
            include_guard, include_guard);

    fprintf(fp, "/*\n");
    fprintf(fp, " * GENERATED CODE. DO NOT EDIT.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * Generated by %s from \"%s\" on %s", prog_name, in_file, time_str);
    fprintf(fp, " */\n\n");

    fprintf(fp, "#include <stdlib.h>\t/* size_t */\n");
    fprintf(fp, "#include <stdint.h>\t/* int types */\n");
    fprintf(fp, "#include <wchar.h>\t/* wchar_t */\n\n");

    if (do_mx_send || do_mx_bcast) {
        fprintf(fp, "#include <libmx.h>\t/* MX functions. */\n");
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->type == DT_CONST) {
            emit_const_declaration(fp, def);
        }
        else if (!def->builtin) {
            emit_typedef(fp, def);
        }
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->type == DT_CONST) {
            continue;
        }
        else if (!def->builtin) {
            if (do_packsize) {
                emit_packsize_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_pack) {
                emit_pack_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_unpack) {
                emit_unpack_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_wrap && def->type == DT_STRUCT) {
                emit_wrap_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_unwrap && def->type == DT_STRUCT) {
                emit_unwrap_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_read_fd) {
                emit_read_signature(fp, def, &file_attr[FILE_ID_FD]);
                fprintf(fp, ";\n");
            }
            if (do_write_fd) {
                emit_write_signature(fp, def, &file_attr[FILE_ID_FD]);
                fprintf(fp, ";\n");
            }
            if (do_read_fp) {
                emit_read_signature(fp, def, &file_attr[FILE_ID_FP]);
                fprintf(fp, ";\n");
            }
            if (do_write_fp) {
                emit_write_signature(fp, def, &file_attr[FILE_ID_FP]);
                fprintf(fp, ";\n");
            }
            if (do_print_fp) {
                emit_print_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_create && def->type == DT_STRUCT) {
                emit_create_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_set && def->type == DT_STRUCT) {
                emit_set_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_copy) {
                emit_copy_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_clear) {
                emit_clear_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_destroy) {
                emit_destroy_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_mx_send) {
                emit_mx_send_signature(fp, def);
                fprintf(fp, ";\n");
            }
            if (do_mx_bcast) {
                emit_mx_bcast_signature(fp, def);
                fprintf(fp, ";\n");
            }
        }
    }

    fprintf(fp, "\n#endif\n");

    fclose(fp);

    return 0;
}

/*
 * Emit C source file.
 */
int emit_c_src(const char *out_file, const char *in_file, const char *prog_name, List *definitions)
{
    FILE *fp;

    Definition *def;

    const char *time_str = time_as_string();

    if ((fp = fopen(out_file, "w")) == NULL) {
        fprintf(stderr, "Could not open file \"%s\": %s\n",
            out_file, strerror(errno));
        return 1;
    }

    set_dependencies();

    fprintf(fp, "/*\n");
    fprintf(fp, " * GENERATED CODE. DO NOT EDIT.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * Generated by %s from \"%s\" on %s",
            prog_name, in_file, time_str);
    fprintf(fp, " */\n\n");

    if (do_copy) {
        fprintf(fp, "#include <assert.h>\t/* assert */\n");
    }

    if (do_read_fp || do_write_fp || do_print_fp) {
        fprintf(fp, "#include <stdio.h>\t/* FILE, fread, fwrite etc. */\n");
    }

    if (do_unpack || do_read_fd || do_read_fp || do_copy) {
        fprintf(fp, "#include <string.h>\t/* memset */\n");
    }

    fprintf(fp, "#include <stdlib.h>\t/* size_t */\n");

    if (do_mx_send || do_mx_bcast) {
        fprintf(fp, "\n#include <libmx.h>\t/* MX functions. */");
    }

    fprintf(fp, "\n#include <libtyger.h>\t/* Tyger functions. */\n\n");

    fprintf(fp, "#include \"%s\"\n", associated_header_file(out_file));

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->type == DT_CONST) {
            emit_const_definition(fp, def);
        }
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (!def->builtin) {
            if (do_packsize) {
                emit_packsize_signature(fp, def);
                emit_packsize_body(fp, def);
            }
            if (do_pack) {
                emit_pack_signature(fp, def);
                emit_pack_body(fp, def);
            }
            if (do_unpack) {
                emit_unpack_signature(fp, def);
                emit_unpack_body(fp, def);
            }
            if (do_wrap && def->type == DT_STRUCT) {
                emit_wrap_signature(fp, def);
                emit_wrap_body(fp, def);
            }
            if (do_unwrap && def->type == DT_STRUCT) {
                emit_unwrap_signature(fp, def);
                emit_unwrap_body(fp, def);
            }
            if (do_read_fd) {
                emit_read_signature(fp, def, &file_attr[FILE_ID_FD]);
                emit_read_body(fp, def, &file_attr[FILE_ID_FD]);
            }
            if (do_write_fd) {
                emit_write_signature(fp, def, &file_attr[FILE_ID_FD]);
                emit_write_body(fp, def, &file_attr[FILE_ID_FD]);
            }
            if (do_read_fp) {
                emit_read_signature(fp, def, &file_attr[FILE_ID_FP]);
                emit_read_body(fp, def, &file_attr[FILE_ID_FP]);
            }
            if (do_write_fp) {
                emit_write_signature(fp, def, &file_attr[FILE_ID_FP]);
                emit_write_body(fp, def, &file_attr[FILE_ID_FP]);
            }
            if (do_print_fp) {
                emit_print_signature(fp, def);
                emit_print_body(fp, def);
            }
            if (do_create && def->type == DT_STRUCT) {
                emit_create_signature(fp, def);
                emit_create_body(fp, def);
            }
            if (do_set && def->type == DT_STRUCT) {
                emit_set_signature(fp, def);
                emit_set_body(fp, def);
            }
            if (do_copy) {
                emit_copy_signature(fp, def);
                emit_copy_body(fp, def);
            }
            if (do_clear) {
                emit_clear_signature(fp, def);
                emit_clear_body(fp, def);
            }
            if (do_destroy) {
                emit_destroy_signature(fp, def);
                emit_destroy_body(fp, def);
            }
            if (do_mx_send) {
                emit_mx_send_signature(fp, def);
                emit_mx_send_body(fp, def);
            }
            if (do_mx_bcast) {
                emit_mx_bcast_signature(fp, def);
                emit_mx_bcast_body(fp, def);
            }
        }
    }

    fclose(fp);

    return 0;
}
