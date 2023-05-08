/* lang-python.c: Generate Python code.
 *
 * Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-12-08
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include "switches.h"
#include "parser.h"
#include "utils.h"
#include "lang-python.h"

#include <libjvs/list.h>
#include <libjvs/utils.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

static bool do_pack     = false;
static bool do_unpack   = false;
static bool do_recv     = false;
static bool do_mx_send  = false;
static bool do_mx_bcast = false;

static Switch switches[] = {
    { "--py-pack",     &do_pack,     "Generate pack functions" },
    { "--py-unpack",   &do_unpack,   "Generate unpack functions" },
    { "--py-recv",     &do_recv,     "Generate recv functions" },
    { "--py-mx-send",  &do_mx_send,  "Generate MX send functions" },
    { "--py-mx-bcast", &do_mx_bcast, "Generate MX broadcast functions" },
};

static int num_switches = sizeof(switches) / sizeof(switches[0]);

/*
 * Return the switches that the C language generator accepts.
 */
Switch *python_switches(int *switch_count_ptr)
{
    *switch_count_ptr = num_switches;

    return switches;
}

static const char *interface_type(Definition *def)
{
    switch(def->type) {
    case DT_INT:
    case DT_ENUM:
        return "int";
    case DT_BOOL:
        return "bool";
    case DT_FLOAT:
        return "float";
    case DT_ASTRING:
        return "str";
    case DT_WSTRING:
        return "unicode";
    case DT_ARRAY:
        return "list";
    case DT_ALIAS:
    case DT_STRUCT:
    case DT_UNION:
        return def->name;
    case DT_VOID:
        return "None";
    default:
        return NULL;
    }
}

static void emit_class(FILE *fp, Definition *def)
{
    if (def->type == DT_STRUCT) {
        StructItem *item;

        ifprintf(fp, 0, "class %s(object):\n", def->name);

        if (!listIsEmpty(&def->struct_def.items)) {
            ifprintf(fp, 1, "def __init__(self");

            for (item = listHead(&def->struct_def.items); item; item = listNext(item)) {
                fprintf(fp, ", %s = None", item->name);
            }

            fprintf(fp, "):\n");

            for (item = listHead(&def->struct_def.items); item; item = listNext(item)) {
                ifprintf(fp, 2, "self.%s = %s\n", item->name, item->name);
            }

            fprintf(fp, "\n");
        }

        ifprintf(fp, 1, "def __repr__(self):\n");
        ifprintf(fp, 2, "return '%s(", def->name);

        for (item = listHead(&def->struct_def.items); item; item = listNext(item)) {
            fprintf(fp, "%s = %%r%s", item->name, listNext(item) == NULL ? "" : ", ");
        }

        fprintf(fp, ")'");

        if (!listIsEmpty(&def->struct_def.items)) {
            fprintf(fp, " %% (");

            for (item = listHead(&def->struct_def.items); item; item = listNext(item)) {
                fprintf(fp, "self.%s%s", item->name, listNext(item) == NULL ? ")" : ", ");
            }
        }

        fprintf(fp, "\n\n");
    }
    else if (def->type == DT_ENUM) {
        EnumItem *item;

        ifprintf(fp, 0, "class %s(object):\n", def->name);

        for (item = listHead(&def->enum_def.items); item; item = listNext(item)) {
            ifprintf(fp, 1, "%s = %ld\n", item->name, item->value);
        }

        fprintf(fp, "\n");
    }
    else if (def->type == DT_UNION) {
        UnionItem *item;

        ifprintf(fp, 0, "class %s(object):\n", def->name);

        ifprintf(fp, 1, "def __init__(self, %s = None, u = None):\n",
                def->union_def.discr_name);

        ifprintf(fp, 2, "self.%s = %s\n",
                def->union_def.discr_name, def->union_def.discr_name);
        ifprintf(fp, 2, "self.u = None\n\n");

        ifprintf(fp, 2, "if u is None:\n");
        ifprintf(fp, 3, "return\n");

        for (item = listHead(&def->union_def.items); item; item = listNext(item)) {
            if (is_void_type(item->def)) continue;

            ifprintf(fp, 2, "elif self.%s == %s.%s:\n",
                    def->union_def.discr_name,
                    def->union_def.discr_def->name,
                    item->value);
            ifprintf(fp, 3, "assert isinstance(u, %s)\n",
                    interface_type(item->def));
        }

        fprintf(fp, "\n");

        ifprintf(fp, 2, "self.u = u\n\n");

        ifprintf(fp, 1, "def __repr__(self):\n");
        ifprintf(fp, 2, "return '%s(%s = %%r, u = %%r)' %% (self.%s, self.u)\n\n",
                def->name,
                def->union_def.discr_name,
                def->union_def.discr_name);
    }
}

static void emit_packer(FILE *fp, Definition *def)
{
    if (def->builtin || def->level > 0) return;

    if (def->type == DT_ALIAS) {
        ifprintf(fp, 0, "class %sPacker(%sPacker):\n", def->name, def->alias_def.alias->name);
        ifprintf(fp, 1, "pass\n\n");
    }
    else if (def->type == DT_ARRAY) {
        ifprintf(fp, 0, "class %sPacker(object):\n", def->name);

        if (do_pack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def pack(value):\n");
            ifprintf(fp, 2, "count = len(value)\n\n");
            ifprintf(fp, 2, "buf = uint32Packer.pack(count)\n\n");
            ifprintf(fp, 2, "for i in range(count):\n");
            ifprintf(fp, 3, "buf += %sPacker.pack(value[i])\n\n",
                    def->array_def.item_type->name);
            ifprintf(fp, 2, "return buf\n\n");
        }

        if (do_unpack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def unpack(buf, offset = 0):\n");
            ifprintf(fp, 2, "count, offset = uint32Packer.unpack(buf, offset)\n\n");
            ifprintf(fp, 2, "value = count * [ None ]\n\n");
            ifprintf(fp, 2, "for i in range(count):\n");
            ifprintf(fp, 3, "value[i], offset = %sPacker.unpack(buf, offset)\n\n",
                    def->array_def.item_type->name);
            ifprintf(fp, 2, "return value, offset\n\n");
        }

        if (do_recv) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def recv(sock):\n");
            ifprintf(fp, 2, "count = uint32Packer.recv(sock)\n\n");
            ifprintf(fp, 2, "value = count * [ None ]\n\n");
            ifprintf(fp, 2, "for i in range(count):\n");
            ifprintf(fp, 3, "value[i] = %sPacker.recv(sock)\n\n",
                    def->array_def.item_type->name);
            ifprintf(fp, 2, "return value\n\n");
        }
    }
    else if (def->type == DT_STRUCT) {
        StructItem *item;

        ifprintf(fp, 0, "class %sPacker(object):\n", def->name);

        if (do_pack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def pack(value):\n");

            ifprintf(fp, 2, "buf = b''\n\n");

            for (item = listHead(&def->struct_def.items); item; item = listNext(item)) {
                ifprintf(fp, 2, "buf += %sPacker.pack(value.%s)%s",
                        item->def->name,
                        item->name,
                        listNext(item) == NULL ? "\n\n" : "\n");
            }

            ifprintf(fp, 2, "return buf\n\n");
        }

        if (do_unpack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def unpack(buf, offset = 0):\n");

            ifprintf(fp, 2, "value = %s()\n\n", def->name);

            for (item = listHead(&def->struct_def.items); item; item = listNext(item)) {
                ifprintf(fp, 2, "value.%s, offset = %sPacker.unpack(buf, offset)%s",
                        item->name, item->def->name,
                        listNext(item) == NULL ? "\n\n" : "\n");
            }

            ifprintf(fp, 2, "return value, offset\n\n");
        }

        if (do_recv) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def recv(sock):\n");

            ifprintf(fp, 2, "value = %s()\n\n", def->name);

            for (item = listHead(&def->struct_def.items); item; item = listNext(item)) {
                ifprintf(fp, 2, "value.%s = %sPacker.recv(sock)%s",
                        item->name, item->def->name,
                        listNext(item) == NULL ? "\n\n" : "\n");
            }

            ifprintf(fp, 2, "return value\n\n");
        }
    }
    else if (def->type == DT_ENUM) {
        ifprintf(fp, 0, "class %sPacker(object):\n", def->name);

        if (do_pack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def pack(value):\n");
            ifprintf(fp, 2, "return uintPacker.pack(%d, value)\n\n", def->enum_def.num_bytes);
        }

        if (do_unpack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def unpack(buf, offset = 0):\n");
            ifprintf(fp, 2, "return uintPacker.unpack(%d, buf, offset)\n\n", def->enum_def.num_bytes);
        }

        if (do_recv) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def recv(sock):\n");
            ifprintf(fp, 2, "return uintPacker.recv(%d, sock)\n\n", def->enum_def.num_bytes);
        }
    }
    else if (def->type == DT_UNION) {
        UnionItem *item;

        ifprintf(fp, 0, "class %sPacker(object):\n", def->name);

        if (do_pack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def pack(value):\n");

            ifprintf(fp, 2, "buf = %sPacker.pack(value.%s)\n\n",
                    def->union_def.discr_def->name, def->union_def.discr_name);

            for (item = listHead(&def->union_def.items); item; item = listNext(item)) {
                ifprintf(fp, 2, "%s value.%s == %s.%s:\n",
                        item == listHead(&def->union_def.items) ? "if" : "elif",
                        def->union_def.discr_name,
                        def->union_def.discr_def->name,
                        item->value);

                if (is_void_type(item->def)) {
                    ifprintf(fp, 3, "pass\n");
                }
                else {
                    ifprintf(fp, 3, "buf += %sPacker.pack(value.u)\n", item->def->name);
                }
            }

            ifprintf(fp, 0, "\n");

            ifprintf(fp, 2, "return buf\n\n");
        }

        if (do_unpack) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def unpack(buf, offset = 0):\n");

            ifprintf(fp, 2, "value = %s()\n\n", def->name);
            ifprintf(fp, 2, "value.%s, offset = %sPacker.unpack(buf, offset)\n\n",
                    def->union_def.discr_name, def->union_def.discr_def->name);

            for (item = listHead(&def->union_def.items); item; item = listNext(item)) {
                ifprintf(fp, 2, "%s value.%s == %s.%s:\n",
                        item == listHead(&def->union_def.items) ? "if" : "elif",
                        def->union_def.discr_name,
                        def->union_def.discr_def->name,
                        item->value);

                if (is_void_type(item->def)) {
                    ifprintf(fp, 3, "pass\n");
                }
                else {
                    ifprintf(fp, 3, "value.u, offset = %sPacker.unpack(buf, offset)\n",
                            item->def->name);
                }
            }

            ifprintf(fp, 0, "\n");

            ifprintf(fp, 2, "return value, offset\n\n");
        }

        if (do_recv) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def recv(sock):\n");

            ifprintf(fp, 2, "value = %s()\n\n", def->name);
            ifprintf(fp, 2, "value.%s = uint32Packer.recv(sock)\n\n",
                    def->union_def.discr_name);

            for (item = listHead(&def->union_def.items); item; item = listNext(item)) {
                ifprintf(fp, 2, "%s value.%s == %s.%s:\n",
                        item == listHead(&def->union_def.items) ? "if" : "elif",
                        def->union_def.discr_name,
                        def->union_def.discr_def->name,
                        item->value);

                if (is_void_type(item->def)) {
                    ifprintf(fp, 3, "pass\n");
                }
                else {
                    ifprintf(fp, 3, "value.u = %sPacker.recv(sock)\n",
                            item->def->name);
                }
            }

            ifprintf(fp, 0, "\n");

            ifprintf(fp, 2, "return value\n\n");
        }
    }
    else if (def->type == DT_INT || def->type == DT_BOOL || def->type == DT_FLOAT ||
             def->type == DT_ASTRING || def->type == DT_WSTRING) {
        if (do_mx_send) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def sendMX(mx, fd, msg_type, msg_ver, value):\n");

            ifprintf(fp, 2, "assert isinstance(value, %s)\n\n",
                    interface_type(def));
            ifprintf(fp, 2, "payload = %sPacker.pack(value)\n\n", def->name);
            ifprintf(fp, 2, "mx.send(fd, msg_type, msg_ver, payload)\n\n");
        }

        if (do_mx_bcast) {
            ifprintf(fp, 1, "@staticmethod\n");
            ifprintf(fp, 1, "def broadcastMX(mx, msg_type, msg_ver, value):\n");

            ifprintf(fp, 2, "assert isinstance(value, %s)\n\n",
                    interface_type(def));
            ifprintf(fp, 2, "payload = %sPacker.pack(value)\n\n", def->name);
            ifprintf(fp, 2, "mx.broadcast(msg_type, msg_ver, payload)\n\n");
        }
    }
}

/*
 * Emit python code.
 */
int emit_python_src(const char *out_file,
                    const char *in_file,
                    const char *prog_name,
                    List *definitions)
{
    FILE *fp;

    Definition *def;

    const char *time_str = time_as_string();

    if ((fp = fopen(out_file, "w")) == NULL) {
        fprintf(stderr, "Could not open file \"%s\": %s\n",
            out_file, strerror(errno));
        return 1;
    }

    fprintf(fp, "#!/usr/bin/env python\n");
    fprintf(fp, "# -*- coding: utf-8 -*-\n\n");

    fprintf(fp, "'''\n");
    fprintf(fp, "  GENERATED CODE. DO NOT EDIT.\n");
    fprintf(fp, "\n");
    fprintf(fp, "  Generated by %s from \"%s\" on %s", prog_name, in_file, time_str);
    fprintf(fp, "'''\n\n");
    fprintf(fp, "from tyger import *\n\n");

    for (def = listHead(definitions); def; def = listNext(def)) {
        char *base = basename(def->name);
        char *period = strchr(base, '.');

        if (def->type == DT_INCLUDE && def->level == 1) {
            fprintf(fp, "from %.*s import *\n", (int) (period - base), base);
        }
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->type != DT_CONST) continue;

        fprintf(fp, "%s = ", def->name);

        switch (def->const_def.const_type->type) {
        case DT_INT:
            fprintf(fp, "%ld\n\n", def->const_def.value.l);
            break;
        case DT_BOOL:
            fprintf(fp, "%s\n\n", def->const_def.value.l ? "True" : "False");
            break;
        case DT_FLOAT:
            fprintf(fp, "%g\n\n", def->const_def.value.d);
            break;
        case DT_ASTRING:
            fprintf(fp, "\"%s\"\n\n", def->const_def.value.s);
            break;
        case DT_WSTRING:
            fprintf(fp, "u\"%s\"\n\n", def->const_def.value.s);
            break;
        default:
            break;
        }
    }

    for (def = listHead(definitions); def; def = listNext(def)) {
        if (def->type == DT_CONST || def->level > 0) continue;

        emit_class(fp, def);
        emit_packer(fp, def);
    }

    return 0;
}
