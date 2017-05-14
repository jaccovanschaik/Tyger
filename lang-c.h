#ifndef LANG_C_H
#define LANG_C_H

/* lang-c.h: Generate C code.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-06
 * Version:   $Id: lang-c.h 81 2016-12-15 19:55:54Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
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
