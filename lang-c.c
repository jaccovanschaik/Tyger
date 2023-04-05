/* lang-c.c: Generate C code.
 *
 * Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-24
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <wchar.h>

#include <libjvs/list.h>
#include <libjvs/utils.h>

#include "switches.h"
#include "parser.h"
#include "utils.h"
#include "utf8.h"
#include "lang-c.h"

static int do_packsize = 0;
static int do_pack = 0;
static int do_unpack = 0;
static int do_clear = 0;
static int do_destroy = 0;
static int do_print = 0;

static Switch switches[] = {
    { "--c-packsize", &do_packsize, "Generate packsize functions" },
    { "--c-pack",     &do_pack,     "Generate pack functions" },
    { "--c-unpack",   &do_unpack,   "Generate unpack functions" },
    { "--c-clear",    &do_clear,    "Generate clear functions" },
    { "--c-destroy",  &do_destroy,  "Generate destroy functions" },
    { "--c-print",    &do_print,    "Generate print functions" },
};

static int num_switches = sizeof(switches) / sizeof(switches[0]);

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
        return "astring ";
    }
    else if (def->type == DT_USTRING) {
        return "ustring ";
    }
    else if (def->type == DT_BOOL) {
        return "bool ";
    }
    else {
        return NULL;
    }
}

static bool is_pass_by_value(Definition *def)
{
    switch(def->type) {
    case DT_INT:
    case DT_BOOL:
    case DT_FLOAT:
    case DT_ENUM:
        return true;
    case DT_ARRAY:
    case DT_STRUCT:
    case DT_UNION:
    case DT_ASTRING:
    case DT_USTRING:
        return false;
    case DT_ALIAS:
        return is_pass_by_value(def->alias_def.alias);
    default:
        return false;
    }
}

/*
 * Does the type definition in <def> have a constant pack size? That is, can
 * we call the ...PackSize function without passing in an instance of the type
 * because we can work out the pack size without looking at its contents?
 */
static bool has_constant_pack_size(Definition *def)
{
    switch(def->type) {
    case DT_INT:
    case DT_BOOL:
    case DT_FLOAT:
    case DT_ENUM:
        return true;
    case DT_STRUCT:
        for (StructItem *struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {
            if (!has_constant_pack_size(struct_item->def)) return false;
        }

        return true;
    case DT_UNION:
        for (UnionItem *union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            if (!has_constant_pack_size(union_item->def)) return false;
        }

        return true;
    case DT_ASTRING:
    case DT_USTRING:
    case DT_ARRAY:
        return false;
    case DT_ALIAS:
        return has_constant_pack_size(def->alias_def.alias);
    default:
        return false;
    }
}

static void emit_const_declaration(FILE *fp, Definition *def)
{
    fprintf(fp, "extern const %s%s;\n",
            equivalent_c_type(def->const_def.const_type),
            def->name);
}

static void emit_const_definition(FILE *fp, Definition *def)
{
    size_t wchar_len;

    switch (def->const_def.const_type->type) {
    case DT_INT:
        fprintf(fp, "const %s%s = ",
                equivalent_c_type(def->const_def.const_type), def->name);
        fprintf(fp, "%ld;\n", def->const_def.value.l);
        break;
    case DT_FLOAT:
        fprintf(fp, "const %s%s = ",
                equivalent_c_type(def->const_def.const_type), def->name);
        fprintf(fp, "%g;\n", def->const_def.value.d);
        break;
    case DT_ASTRING:
        ifprintf(fp, 0, "const %s%s = {\n",
                equivalent_c_type(def->const_def.const_type), def->name);
        ifprintf(fp, 1, ".data = \"%s\",\n", def->const_def.value.s);
        ifprintf(fp, 1, ".len  = %ld,\n", strlen(def->const_def.value.s));
        ifprintf(fp, 1, ".cap  = %ld,\n", strlen(def->const_def.value.s) + 1);
        ifprintf(fp, 0, "};\n");
        break;
    case DT_USTRING:
        utf8_to_wchar((uint8_t *) def->const_def.value.s,
                strlen(def->const_def.value.s), &wchar_len);

        ifprintf(fp, 0, "const %s%s = {\n",
                equivalent_c_type(def->const_def.const_type), def->name);
        ifprintf(fp, 1, ".data = L\"%s\",\n", def->const_def.value.s);
        ifprintf(fp, 1, ".len  = %ld,\n", wchar_len);
        ifprintf(fp, 1, ".cap  = %ld,\n", wchar_len + 1);
        ifprintf(fp, 0, "};\n");
        break;
    case DT_BOOL:
        fprintf(fp, "const %s%s = ",
                equivalent_c_type(def->const_def.const_type), def->name);
        fprintf(fp, "%s;\n", def->const_def.value.l ? "true" : "false");
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
            if (struct_item->optional) {
                ifprintf(fp, 1, "%s*%s;\n",
                        equivalent_c_type(struct_item->def),
                        struct_item->name);
            }
            else {
                ifprintf(fp, 1, "%s%s;\n",
                        equivalent_c_type(struct_item->def),
                        struct_item->name);
            }
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

            ifprintf(fp, 2, "%s%s;\n", equivalent_c_type(union_item->def), union_item->name);
        }
        ifprintf(fp, 1, "};\n");

        fprintf(fp, "} %s;\n", def->name);

        break;
    case DT_INCLUDE:
        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s).\n",
                __func__, def->type, deftype_enum_to_string(def->type));
        abort();
    }
}

static void emit_packsize_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST || def->type == DT_INCLUDE || def->builtin) return;

    if (has_constant_pack_size(def)) {
        fprintf(fp, "\n/*\n");
        fprintf(fp, " * Return the number of bytes required to pack a %s.\n",
                def->name);
        fprintf(fp, " */\n");
        fprintf(fp, "size_t %sPackSize(void)", def->name);
    }
    else if (is_pass_by_value(def)) {
        fprintf(fp, "\n/*\n");
        fprintf(fp, " * Return the number of bytes required to pack %s "
                    "<data>.\n", def->name);
        fprintf(fp, " */\n");
        fprintf(fp, "size_t %sPackSize(%s data)", def->name, def->name);
    }
    else {
        fprintf(fp, "\n/*\n");
        fprintf(fp, " * Return the number of bytes required to pack %s "
                    "<data>.\n", def->name);
        fprintf(fp, " */\n");
        fprintf(fp, "size_t %sPackSize(const %s *data)", def->name, def->name);
    }
}

static void emit_packsize_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST || def->builtin) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        if (has_constant_pack_size(def)) {
            ifprintf(fp, 1, "return %sPackSize();\n",
                    def->alias_def.alias->name);
        }
        else {
            ifprintf(fp, 1, "return %sPackSize(data);\n",
                    def->alias_def.alias->name);
        }
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "size_t size = uint32PackSize();\n\n");

        if (has_constant_pack_size(def->array_def.item_type)) {
            ifprintf(fp, 1, "size += data->count * %sPackSize();\n\n",
                    def->array_def.item_type->name);
        }
        else {
            ifprintf(fp, 1, "for (int i = 0; i < data->count; i++) {\n");

            if (is_pass_by_value(def->array_def.item_type)) {
                ifprintf(fp, 2, "size += %sPackSize(data->%s[i]);\n",
                        def->array_def.item_type->name,
                        def->array_def.item_name);
            }
            else {
                ifprintf(fp, 2, "size += %sPackSize(data->%s + i);\n",
                        def->array_def.item_type->name,
                        def->array_def.item_name);
            }

            ifprintf(fp, 1, "}\n\n");
        }

        ifprintf(fp, 1, "return size;\n");
        break;
    case DT_STRUCT:
        ifprintf(fp, 1, "size_t size = 0;\n");

        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item))
        {
            fputc('\n', fp);

            if (struct_item->optional) {
                ifprintf(fp, 1, "size += uint8PackSize();\n");
                ifprintf(fp, 1, "if (data->%s) {\n", struct_item->name);

                if (has_constant_pack_size(struct_item->def)) {
                    ifprintf(fp, 2, "size += %sPackSize();\n",
                            struct_item->def->name);
                }
                else if (is_pass_by_value(struct_item->def)) {
                    ifprintf(fp, 2, "size += %sPackSize(*data->%s);\n",
                            struct_item->def->name,
                            struct_item->name);
                }
                else {
                    ifprintf(fp, 2, "size += %sPackSize(data->%s);\n",
                            struct_item->def->name,
                            struct_item->name);
                }
                ifprintf(fp, 1, "}\n");
            }
            else {
                if (has_constant_pack_size(struct_item->def)) {
                    ifprintf(fp, 1, "size += %sPackSize();\n",
                            struct_item->def->name);
                }
                else if (is_pass_by_value(struct_item->def)) {
                    ifprintf(fp, 1, "size += %sPackSize(data->%s);\n",
                            struct_item->def->name,
                            struct_item->name);
                }
                else {
                    ifprintf(fp, 1, "size += %sPackSize(&data->%s);\n",
                            struct_item->def->name,
                            struct_item->name);
                }
            }
        }

        fputc('\n', fp);

        ifprintf(fp, 1, "return size;\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return %ld;\n", def->enum_def.num_bytes);
        break;
    case DT_UNION:
        ifprintf(fp, 1, "size_t size = %sPackSize();\n\n",
                def->union_def.discr_def->name);
        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {

            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                if (has_constant_pack_size(union_item->def)) {
                    ifprintf(fp, 2, "size += %sPackSize();%s",
                            union_item->def->name,
                            listNext(union_item) == NULL ? "\n\n" : "\n");
                }
                else if (is_pass_by_value(union_item->def)) {
                    ifprintf(fp, 2, "size += %sPackSize(data->%s);%s",
                            union_item->def->name,
                            union_item->name,
                            listNext(union_item) == NULL ? "\n\n" : "\n");
                }
                else {
                    ifprintf(fp, 2, "size += %sPackSize(&data->%s);%s",
                            union_item->def->name,
                            union_item->name,
                            listNext(union_item) == NULL ? "\n\n" : "\n");
                }
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return size;\n");
        break;
    default:
        fprintf(stderr, "%s: Unexpected definition type %d (%s) for %s.\n",
                __func__, def->type, deftype_enum_to_string(def->type),
                def->name);
        abort();
    }

    fprintf(fp, "}\n");
}

static void emit_pack_signature(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST || def->type == DT_INCLUDE || def->builtin) return;

    fprintf(fp,
            "\n/*\n"
            " * Pack <data> into <buf>, enlarging it if necessary. <data>\n"
            " * points to the data to write, <buf> is a pointer to a pointer\n"
            " * to the start of the buffer, <size> points to its current size\n"
            " * and <pos> points to the position in the buffer where the data is\n"
            " * to be written. The contents of <buf> and <size> are updated\n"
            " * if the buffer is enlarged. <pos> is updated with the new write\n"
            " * position. The number of bytes written is returned.\n"
            " */\n");

    if (is_pass_by_value(def)) {
        fprintf(fp,
                "buffer *%sPack(%s data, buffer *buf)",
                def->name, def->name);
    }
    else {
        fprintf(fp,
                "buffer *%sPack(const %s *data, buffer *buf)",
                def->name, def->name);
    }
}

static void emit_pack_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST || def->builtin) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "return %sPack(data, buf);\n",
                def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n");
        ifprintf(fp, 1,
                "uint32Pack(data->count, buf);\n\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");

        if (is_pass_by_value(def->array_def.item_type)) {
            ifprintf(fp, 2, "%sPack(data->%s[i], buf);\n",
                    def->array_def.item_type->name,
                    def->array_def.item_name);
        }
        else {
            ifprintf(fp, 2, "%sPack(data->%s + i, buf);\n",
                    def->array_def.item_type->name,
                    def->array_def.item_name);
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return buf;\n");
        break;
    case DT_STRUCT:
        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item)) {

            if (struct_item->optional) {
                fputc('\n', fp);
                ifprintf(fp, 1, "uint8Pack(data->%s ? 1 : 0, buf);\n", struct_item->name);
                ifprintf(fp, 1, "if (data->%s) {\n", struct_item->name);

                if (is_pass_by_value(struct_item->def)) {
                    ifprintf(fp, 2, "%sPack(*data->%s, buf);\n",
                            struct_item->def->name,
                            struct_item->name);
                }
                else {
                    ifprintf(fp, 2, "%sPack(data->%s, buf);\n",
                            struct_item->def->name,
                            struct_item->name);
                }

                ifprintf(fp, 1, "}\n");
            }
            else {
                if (is_pass_by_value(struct_item->def)) {
                    ifprintf(fp, 1, "%sPack(data->%s, buf);\n",
                            struct_item->def->name,
                            struct_item->name);
                }
                else {
                    ifprintf(fp, 1, "%sPack(&data->%s, buf);\n",
                            struct_item->def->name,
                            struct_item->name);
                }
            }
        }

        fputc('\n', fp);

        ifprintf(fp, 1, "return buf;\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return uintPack(data, %ld, buf);\n", def->enum_def.num_bytes);
        break;
    case DT_UNION:
        ifprintf(fp, 1, "%sPack(data->%s, buf);\n\n",
                def->union_def.discr_def->name,
                def->union_def.discr_name);
        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                if (is_pass_by_value(union_item->def)) {
                    ifprintf(fp, 2, "%sPack(data->%s, buf);\n",
                            union_item->def->name,
                            union_item->name);
                }
                else {
                    ifprintf(fp, 2, "%sPack(&data->%s, buf);\n",
                            union_item->def->name,
                            union_item->name);
                }
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return buf;\n");
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
    if (def->type == DT_CONST || def->type == DT_INCLUDE || def->builtin) return;

    fprintf(fp,
            "\n/*\n"
            " * Unpack <data> from <buf>, which is <size> bytes in size.\n"
            " */\n");
    fprintf(fp, "size_t %sUnpack(const buffer *buf, size_t pos, %s *data)",
            def->name, def->name);
}

static void emit_unpack_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST || def->type == DT_INCLUDE || def->builtin) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "return %sUnpack(buf, pos, data);\n", def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n");
        ifprintf(fp, 1, "uint32_t old_count = data->count;\n");
        ifprintf(fp, 1, "pos = uint32Unpack(buf, pos, &data->count);\n\n");

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
        ifprintf(fp, 2, "pos = %sUnpack(buf, pos, data->%s + i);\n",
                def->array_def.item_type->name, def->array_def.item_name);

        ifprintf(fp, 1, "}\n\n");

        ifprintf(fp, 1, "return pos;\n");
        break;
    case DT_STRUCT:
        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item))
        {
            if (struct_item->optional) {
                ifprintf(fp, 1, "uint8_t %s_follows;\n", struct_item->name);
                ifprintf(fp, 1, "pos = uint8Unpack(buf, pos, &%s_follows);\n\n",
                        struct_item->name);
                ifprintf(fp, 1, "if (data->%s != NULL && %s_follows) {\n",
                        struct_item->name, struct_item->name);
                ifprintf(fp, 2, "pos = %sUnpack(buf, pos, data->%s);\n",
                        struct_item->def->name, struct_item->name);
                ifprintf(fp, 1, "}\n");
                ifprintf(fp, 1, "else if (data->%s != NULL && !%s_follows) {\n",
                        struct_item->name, struct_item->name);
                ifprintf(fp, 2, "%sDestroy(data->%s);\n",
                        struct_item->def->name, struct_item->name);
                ifprintf(fp, 2, "data->%s = NULL;\n", struct_item->name);
                ifprintf(fp, 1, "}\n");
                ifprintf(fp, 1, "else if (data->%s == NULL && %s_follows) {\n",
                        struct_item->name, struct_item->name);
                ifprintf(fp, 2, "data->%s = calloc(1, sizeof(%s));\n",
                        struct_item->name, equivalent_c_type(struct_item->def));
                ifprintf(fp, 2, "pos = %sUnpack(buf, pos, data->%s);\n",
                        struct_item->def->name, struct_item->name);
                ifprintf(fp, 1, "}\n\n");
            }
            else {
                ifprintf(fp, 1, "pos = %sUnpack(buf, pos, &data->%s);\n\n",
                        struct_item->def->name, struct_item->name);
            }
        }

        ifprintf(fp, 1, "return pos;\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "return uintUnpack(%ld, buf, pos, data);\n", def->enum_def.num_bytes);
        break;
    case DT_UNION:
        ifprintf(fp, 1, "pos = %sUnpack(buf, pos, &data->%s);\n\n",
                def->union_def.discr_def->name,
                def->union_def.discr_name);
        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                ifprintf(fp, 2, "pos = %sUnpack(buf, pos, &data->%s);\n",
                        union_item->def->name, union_item->name);
            }

            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n\n");
        ifprintf(fp, 1, "return pos;\n");
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
    if (def->type == DT_CONST || def->type == DT_INCLUDE || def->builtin) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Print an ASCII representation of <data> to <fp>.\n");
    fprintf(fp, " */\n");

    if (is_pass_by_value(def)) {
        fprintf(fp, "void %sPrint(FILE *fp, %s data, int level)", def->name, def->name);
    }
    else {
        fprintf(fp, "void %sPrint(FILE *fp, const %s *data, int level)", def->name, def->name);
    }
}

static void emit_print_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;
    EnumItem *enum_item;

    if (def->type == DT_CONST || def->builtin) return;

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

        if (is_pass_by_value(def->array_def.item_type)) {
            ifprintf(fp, 2, "%sPrint(fp, data->%s[i], level);\n",
                    def->array_def.item_type->name,
                    def->array_def.item_name);
        }
        else {
            ifprintf(fp, 2, "%sPrint(fp, data->%s + i, level);\n",
                    def->array_def.item_type->name,
                    def->array_def.item_name);
        }

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
                struct_item; struct_item = listNext(struct_item))
            {
                ifprintf(fp, 1, "fprintf(fp, \"%%s%s: \", indent(level));\n", struct_item->name);

                if (struct_item->optional) {
                    ifprintf(fp, 1, "if (data->%s) {\n", struct_item->name);
                    if (is_pass_by_value(struct_item->def)) {
                        ifprintf(fp, 2, "%sPrint(fp, *data->%s, level);\n",
                                struct_item->def->name,
                                struct_item->name);
                    }
                    else {
                        ifprintf(fp, 2, "%sPrint(fp, data->%s, level);\n",
                                struct_item->def->name,
                                struct_item->name);
                    }
                    ifprintf(fp, 1, "}\n");
                    ifprintf(fp, 1, "else {\n");
                    ifprintf(fp, 2, "fprintf(fp, \"<none>\");\n");
                    ifprintf(fp, 1, "}\n");
                }
                else {
                    if (is_pass_by_value(struct_item->def)) {
                        ifprintf(fp, 1, "%sPrint(fp, data->%s, level);\n",
                                struct_item->def->name,
                                struct_item->name);
                    }
                    else {
                        ifprintf(fp, 1, "%sPrint(fp, &data->%s, level);\n",
                                struct_item->def->name,
                                struct_item->name);
                    }
                }
                ifprintf(fp, 1, "fputc('\\n', fp);\n\n");
            }

            ifprintf(fp, 1, "level--;\n\n");
        }

        ifprintf(fp, 1, "fprintf(fp, \"%%s}\", indent(level));\n");
        break;
    case DT_ENUM:
        ifprintf(fp, 1, "switch(data) {\n");

        for (enum_item = listHead(&def->enum_def.items);
             enum_item; enum_item = listNext(enum_item)) {
            ifprintf(fp, 1, "case %s:\n", enum_item->name);
            ifprintf(fp, 2, "fprintf(fp, \"%s\");\n", enum_item->name);
            ifprintf(fp, 2, "break;\n");
        }

        ifprintf(fp, 1, "}\n");
        break;
    case DT_UNION:
        ifprintf(fp, 1, "%sPrint(fp, data->%s, level);\n\n",
                def->union_def.discr_def->name, def->union_def.discr_name);

        ifprintf(fp, 1, "fprintf(fp, \" \");\n\n");

        ifprintf(fp, 1, "switch(data->%s) {\n",
                def->union_def.discr_name);

        for (union_item = listHead(&def->union_def.items);
             union_item; union_item = listNext(union_item)) {
            ifprintf(fp, 1, "case %s:\n", union_item->value);

            if (!is_void_type(union_item->def)) {
                if (is_pass_by_value(union_item->def)) {
                    ifprintf(fp, 2, "%sPrint(fp, data->%s, level);\n",
                            union_item->def->name,
                            union_item->name);
                }
                else {
                    ifprintf(fp, 2, "%sPrint(fp, &data->%s, level);\n",
                            union_item->def->name,
                            union_item->name);
                }
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
    if (def->type == DT_CONST || def->type == DT_INCLUDE || def->builtin) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Clear an already used %s.\n", def->name);
    fprintf(fp, " */\n");
    fprintf(fp, "void %sClear(%s *data)", def->name, def->name);
}

static void emit_clear_body(FILE *fp, Definition *def)
{
    StructItem *struct_item;
    UnionItem *union_item;

    if (def->type == DT_CONST || def->builtin) return;

    fprintf(fp, "\n{\n");

    switch(def->type) {
    case DT_ALIAS:
        ifprintf(fp, 1, "%sClear(data);\n", def->alias_def.alias->name);
        break;
    case DT_ARRAY:
        ifprintf(fp, 1, "int i;\n\n");

        ifprintf(fp, 1, "for (i = 0; i < data->count; i++) {\n");
        ifprintf(fp, 2, "%sClear(data->%s + i);\n",
                def->array_def.item_type->name,
                def->array_def.item_name);
        ifprintf(fp, 1, "}\n\n");

        ifprintf(fp, 1, "free(data->%s);\n\n", def->array_def.item_name);
        ifprintf(fp, 1, "data->count = 0;\n");
        ifprintf(fp, 1, "data->%s = NULL;\n", def->array_def.item_name);
        break;
    case DT_STRUCT:
        for (struct_item = listHead(&def->struct_def.items);
             struct_item; struct_item = listNext(struct_item))
        {
            if (struct_item->optional) {
                ifprintf(fp, 1, "if (data->%s != NULL) {\n", struct_item->name);
                ifprintf(fp, 2, "%sDestroy(data->%s);\n",
                        struct_item->def->name, struct_item->name);
                ifprintf(fp, 2, "data->%s = NULL;\n", struct_item->name);
                ifprintf(fp, 1, "}\n");
            }
            else {
                ifprintf(fp, 1, "%sClear(&data->%s);\n", struct_item->def->name, struct_item->name);
            }
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
    if (def->type == DT_CONST || def->type == DT_INCLUDE || def->builtin) return;

    fprintf(fp, "\n/*\n");
    fprintf(fp, " * Destroy an already used %s.\n", def->name);
    fprintf(fp, " */\n");
    fprintf(fp, "void %sDestroy(%s *data)", def->name, def->name);
}

static void emit_destroy_body(FILE *fp, Definition *def)
{
    if (def->type == DT_CONST || def->builtin) return;

    fprintf(fp, "\n{\n");
    ifprintf(fp, 1, "%sClear(data);\n\n", def->name);
    ifprintf(fp, 1, "free(data);\n");
    fprintf(fp, "}\n");
}

static void set_dependencies(void)
{
    if (do_destroy) do_clear = TRUE;
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
int emit_c_hdr(const char *out_file, const char *in_file,
               const char *prog_name, List *definitions)
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
    fprintf(fp, " * Generated by %s from \"%s\" on %s",
            prog_name, in_file, time_str);
    fprintf(fp, " */\n\n");

    fprintf(fp, "#include <stdlib.h>\t/* size_t */\n");
    fprintf(fp, "#include <stdint.h>\t/* int types */\n");
    fprintf(fp, "#include <stdbool.h>\t/* bool */\n");
    fprintf(fp, "#include <wchar.h>\t/* wchar_t */\n");

    fprintf(fp, "\n#include \"libtyger.h\"\n\n");

    for (def = listHead(definitions); def; def = listNext(def)) {
        char *base = basename(def->name);
        char *period = strchr(base, '.');

        if (def->type == DT_INCLUDE && def->level == 1) {
            fprintf(fp, "#include \"%.*s.h\"\n", (int) (period - base), base);
        }
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->builtin || def->level > 0) {
            continue;
        }
        else if (def->type == DT_CONST) {
            emit_const_declaration(fp, def);
        }
        else {
            emit_typedef(fp, def);
        }
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->type == DT_CONST || def->builtin || def->level > 0) {
            continue;
        }

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
        if (do_clear) {
            emit_clear_signature(fp, def);
            fprintf(fp, ";\n");
        }
        if (do_destroy) {
            emit_destroy_signature(fp, def);
            fprintf(fp, ";\n");
        }
        if (do_print) {
            emit_print_signature(fp, def);
            fprintf(fp, ";\n");
        }
    }

    fprintf(fp, "\n#endif\n");

    fclose(fp);

    return 0;
}

/*
 * Emit C source file.
 */
int emit_c_src(const char *out_file, const char *in_file,
               const char *prog_name, List *definitions)
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
    fprintf(fp, " * Generated by %s from \"%s\" on %s", prog_name, in_file, time_str);
    fprintf(fp, " */\n\n");

    fprintf(fp, "#include <string.h>\t/* memset */\n");
    fprintf(fp, "#include <stdlib.h>\t/* size_t */\n");

    fprintf(fp, "\n#include \"libtyger.h\"\t/* Tyger functions. */\n");

    fprintf(fp, "\n#include \"%s\"\n\n", associated_header_file(out_file));

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->level != 0) {
            continue;
        }
        else if (def->type == DT_CONST) {
            emit_const_definition(fp, def);
        }
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->level != 0) continue;

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
            if (do_clear) {
                emit_clear_signature(fp, def);
                emit_clear_body(fp, def);
            }
            if (do_destroy) {
                emit_destroy_signature(fp, def);
                emit_destroy_body(fp, def);
            }
            if (do_print) {
                emit_print_signature(fp, def);
                emit_print_body(fp, def);
            }
        }
    }

    fclose(fp);

    return 0;
}
