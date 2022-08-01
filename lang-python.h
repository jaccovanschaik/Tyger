#ifndef LANG_PYTHON_H
#define LANG_PYTHON_H

/* lang-python.h: Generate Python code.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-12-08
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

/*
 * Return the switches that the C language generator accepts.
 */
Switch *python_switches(int *switch_count_ptr);

/*
 * Emit python code.
 */
int emit_python_src(const char *out_file,
                    const char *in_file,
                    const char *prog_name,
                    List *definitions);
#endif
