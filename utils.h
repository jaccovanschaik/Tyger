#ifndef UTILS_H
#define UTILS_H

/* utils.h: Utility functions.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-06
 * Version:   $Id: utils.h 152 2019-01-11 11:10:13Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
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

#endif
