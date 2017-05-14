#ifndef SWITCHES_H
#define SWITCHES_H

/* switches.h: Descriptions for command lines switches.
 *
 * Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-12-15
 * Version:   $Id: switches.h 127 2017-05-14 17:20:46Z jacco $
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

typedef struct {
    char *option;
    int *enabled;
    char *description;
} Switch;

#endif
