/* tyger.c: Tyger type generator.
 *
 * Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-08-25
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

#include <libjvs/buffer.h>
#include <libjvs/options.h>
#include <libjvs/utils.h>

#include "parser.h"
#include "version.h"
#include "switches.h"

#include "lang-c.h"
#include "lang-python.h"

static void make_astring_type(List *definitions)
{
    Definition *def;

    def = calloc(1, sizeof(*def));

    def->type = DT_ASTRING;
    def->name = strdup("astring");
    def->builtin = true;

    listAppendTail(definitions, def);
}

static void make_ustring_type(List *definitions)
{
    Definition *def;

    def = calloc(1, sizeof(*def));

    def->type = DT_USTRING;
    def->name = strdup("ustring");
    def->builtin = true;

    listAppendTail(definitions, def);
}

static void make_int_types(List *definitions)
{
    Definition *def;
    int is_signed;
    int size;

    Buffer name_buf = { };

    for (is_signed = 0; is_signed <= 1; is_signed++) {
        for (size = 1; size <= 8; size <<= 1) {
            def = calloc(1, sizeof(*def));

            bufSetF(&name_buf, "%sint%d", is_signed ? "" : "u", 8 * size);

            def->type = DT_INT;
            def->name = bufDetach(&name_buf);
            def->builtin = true;
            def->int_def.size = size;
            def->int_def.is_signed = is_signed;

            listAppendTail(definitions, def);
        }
    }
}

static void make_bool_type(List *definitions)
{
    Definition *def;

    def = calloc(1, sizeof(*def));

    def->type = DT_BOOL;
    def->name = strdup("bool");
    def->builtin = true;

    listAppendTail(definitions, def);
}

static void make_float_types(List *definitions)
{
    Definition *def;
    int size;

    for (size = 4; size <= 8; size <<= 1) {
        Buffer *name_buf = bufCreate();

        bufSetF(name_buf, "float%d", 8 * size);

        def = calloc(1, sizeof(*def));

        def->type = DT_FLOAT;
        def->name = bufFinish(name_buf);
        def->builtin = true;
        def->float_def.size = size;

        listAppendTail(definitions, def);
    }
}

static void make_void_type(List *definitions)
{
    Definition *def;

    def = calloc(1, sizeof(*def));

    def->type = DT_VOID;
    def->name = strdup("void");
    def->builtin = true;

    listAppendTail(definitions, def);
}

static void dump_definitions(FILE *fp, List *definitions)
{
    Definition *def;

    for (def = listHead(definitions); def; def = listNext(def)) {
        StructItem *struct_item;
        EnumItem *enum_item;
        UnionItem *union_item;

        fprintf(stderr, "Definition:\n");
        fprintf(stderr, "\ttype: %s\n", deftype_enum_to_string(def->type));
        fprintf(stderr, "\tname: %s\n", def->name);
        fprintf(stderr, "\tbuilt-in: %s\n", def->builtin ? "yes" : "no ");

        switch(def->type) {
        case DT_INT:
            fprintf(stderr, "\tsize: %d\n", def->int_def.size);
            fprintf(stderr, "\tis_signed: %s\n",
                    def->int_def.is_signed ? "yes" : "no");
            break;
        case DT_FLOAT:
            fprintf(stderr, "\tsize: %d\n", def->float_def.size);
            break;
        case DT_ALIAS:
            fprintf(stderr, "\taka:  %s\n", def->alias_def.alias->name);
            break;
        case DT_ARRAY:
            fprintf(stderr, "\tcontains: %s\n",
		    def->array_def.item_type->name);
            break;
        case DT_STRUCT:
            fprintf(stderr, "\tcontents:\n");
            for (struct_item = listHead(&def->struct_def.items);
                 struct_item; struct_item = listNext(struct_item)) {
                fprintf(stderr, "\t\t%s%s %s\n",
                        struct_item->optional ? "optional " : "",
                        struct_item->def->name, struct_item->name);
            }
            break;
        case DT_ENUM:
            fprintf(stderr, "\tcontents:\n");
            for (enum_item = listHead(&def->enum_def.items);
                 enum_item; enum_item = listNext(enum_item)) {
                fprintf(stderr, "\t\t%s = %ld\n",
		        enum_item->name, enum_item->value);
            }
            break;
        case DT_UNION:
            fprintf(stderr, "\tcontents:\n");
            for (union_item = listHead(&def->union_def.items);
                 union_item; union_item = listNext(union_item)) {
                fprintf(stderr, "\t\t%s: %s %s\n",
                        union_item->value,
			union_item->def->name,
			union_item->name);
            }
            break;
        default:
            break;
        }
    }
}

static void usage(char *argv0, const char *msg, int exit_code)
{
    int i, num_switches;
    Switch *switches;

    if (msg) {
        fprintf(stderr, "%s\n", msg);
    }

    if (exit_code != 0) {
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "Usage: %s <options> <input-file>\n\n", basename(argv0));

    fprintf(stderr,
	"Options:\n"
	"\t-V, --version\t\t\tPrint version and exit.\n"
	"\t-c, --c-src <C-source-output>\tOutput C source file here.\n"
	"\t-h, --c-hdr <C-header-output>\tOutput C header file here.\n"
	"\t-p, --python <python-output>\tOutput python code here.\n"
	"\t-i, --indent <indent-string>\tUse this string as indent.\n");

    fprintf(stderr, "\n\tSwitches accepted by the C code generator\n");

    switches = c_switches(&num_switches);

    for (i = 0; i < num_switches; i++) {
        fprintf(stderr, "\t  %s\t%s\n",
	        switches[i].option, switches[i].description);
    }

    fprintf(stderr, "\n\tSwitches accepted by the Python code generator\n");

    switches = python_switches(&num_switches);

    for (i = 0; i < num_switches; i++) {
        fprintf(stderr, "\t  %s\t%s\n",
	        switches[i].option, switches[i].description);
    }

    exit(exit_code);
}

int main(int argc, char *argv[])
{
    int i, file_arg, debug = 0;
    char *msg;
    List definitions = { };

    int num_switches;
    Switch *switches;

    const char *prog_name = basename(argv[0]);

    if (argc == 1) {
        usage(argv[0], NULL, 0);
    }

    Options *options = optCreate();

    optAdd(options, "c-src", 'c', ARG_REQUIRED);
    optAdd(options, "c-hdr", 'h', ARG_REQUIRED);
    optAdd(options, "python", 'p', ARG_REQUIRED);
    optAdd(options, "indent", 'i', ARG_REQUIRED);
    optAdd(options, "version", 'V', ARG_NONE);

    switches = c_switches(&num_switches);

    for (i = 0; i < num_switches; i++) {
        optAdd(options, switches[i].option + 2, 0, ARG_NONE);
    }

    switches = python_switches(&num_switches);

    for (i = 0; i < num_switches; i++) {
        optAdd(options, switches[i].option + 2, 0, ARG_NONE);
    }

    file_arg = optParse(options, argc, argv);

    if (file_arg == -1) {
        usage(argv[0], NULL, 1);
    }
    else if (optIsSet(options, "version")) {
        fprintf(stdout, "%s\n", VERSION);
        exit(0);
    }
    else if (file_arg >= argc) {
        usage(argv[0], "Missing input file.", 1);
    }

    switches = c_switches(&num_switches);

    for (i = 0; i < num_switches; i++) {
        *switches[i].enabled = optIsSet(options, switches[i].option + 2);
    }

    switches = python_switches(&num_switches);

    for (i = 0; i < num_switches; i++) {
        *switches[i].enabled = optIsSet(options, switches[i].option + 2);
    }

    make_astring_type(&definitions);
    make_ustring_type(&definitions);
    make_int_types(&definitions);
    make_bool_type(&definitions);
    make_float_types(&definitions);
    make_void_type(&definitions);

    msg = parse(argv[file_arg], &definitions);

    if (msg != NULL) {
        fprintf(stderr, "%s\n", msg);
        return 1;
    }

    if (debug) {
        dump_definitions(stderr, &definitions);
    }

    set_indent_string(optArg(options, "indent", "    "));

    if (optIsSet(options, "c-hdr")) {
        const char *out_file = optArg(options, "c-hdr", NULL);

        if (emit_c_hdr(out_file, argv[file_arg], prog_name, &definitions) != 0)
            return 1;
    }

    if (optIsSet(options, "c-src")) {
        const char *out_file = optArg(options, "c-src", NULL);

	if (emit_c_src(out_file, argv[file_arg], prog_name, &definitions) != 0)
            return 1;
    }

    if (optIsSet(options, "python")) {
        const char *out_file = optArg(options, "python", NULL);

        if (emit_python_src(out_file, argv[file_arg],
	                    prog_name, &definitions) != 0) return 1;
    }

    return 0;
}
