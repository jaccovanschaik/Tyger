/* utils.c: Utility functions.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-10-06
 * Version:   $Id: utils.c 49 2016-11-25 09:43:12Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#include <time.h>

#include "utils.h"

/*
 * Return a string representation of the current time.
 */
const char *time_as_string(void)
{
    time_t sec = time(NULL);

    return ctime(&sec);
}
