#ifndef LANG_C_H
#define LANG_C_H

/* lang-c.h: Generate C code.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-06
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include <libjvs/list.h>

/*
 * Return the switches that the C language generator accepts.
 */
Switch *c_switches(int *switch_count_ptr);

/*
 * Emit C header file.
 */
int emit_c_hdr(const char *out_file, const char *in_file, const char *prog_name, List *definitions);

/*
 * Emit C source file.
 */
int emit_c_src(const char *out_file, const char *in_file, const char *prog_name, List *definitions);

#endif
