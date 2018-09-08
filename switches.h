#ifndef SWITCHES_H
#define SWITCHES_H

/* switches.h: Descriptions for command lines switches.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-12-15
 * Version:   $Id: switches.h 148 2018-09-08 18:08:57Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

typedef struct {
    const char *option;
    int *enabled;
    const char *description;
} Switch;

#endif
