/* utils.c: Utility functions.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-06
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include <time.h>

#include "parser.h"
#include "utils.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

/*
 * Return a string representation of the current time.
 */
const char *time_as_string(void)
{
    time_t sec = time(NULL);

    return ctime(&sec);
}

/*
 * Find out whether <def> defines an integer type.
 */
int is_integer_type(Definition *def)
{
    if (def->type == DT_INT) {
        return TRUE;
    }
    else if (def->type == DT_ENUM) {
        return TRUE;
    }
    else if (def->type == DT_ALIAS) {
        return is_integer_type(def->alias_def.alias);
    }
    else {
        return FALSE;
    }
}

/*
 * Find out whether <def> defines a void type.
 */
int is_void_type(Definition *def)
{
    if (def->type == DT_VOID) {
        return TRUE;
    }
    else if (def->type == DT_ALIAS) {
        return is_void_type(def->alias_def.alias);
    }
    else {
        return FALSE;
    }
}
