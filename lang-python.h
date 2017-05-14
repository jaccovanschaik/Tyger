#ifndef LANG_PYTHON_H
#define LANG_PYTHON_H

/* lang-python.h: Generate Python code.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-12-08
 * Version:   $Id: lang-python.h 99 2016-12-23 10:34:39Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
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
