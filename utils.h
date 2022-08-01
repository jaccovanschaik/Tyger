#ifndef UTILS_H
#define UTILS_H

/* utils.h: Utility functions.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-06
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

/*
 * Return a string representation of the current time.
 */
const char *time_as_string(void);

/*
 * Find out whether <def> defines an integer type.
 */
int is_integer_type(Definition *def);

/*
 * Find out whether <def> defines a void type.
 */
int is_void_type(Definition *def);

/*
 * Find out whether <def> defines a string type.
 */
int is_string_type(Definition *def);

#endif
