/* test_objects.c: Tests for generated C code.
 *
 * Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-09-14
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include <libjvs/utils.h>

#include "Objects.h"

#define DEBUG 0

static void fill_objects(Objects *objects)
{
    objects->count = 4;
    objects->object = calloc(objects->count, sizeof(Object));

    objects->object[0].name = strdup("A line");
    objects->object[0].creator = wcsdup(L"Øve");
    objects->object[0].visible = true;
    objects->object[0].shape.shape_type = ST_LINE;
    objects->object[0].shape.line.sv.x = 1;
    objects->object[0].shape.line.sv.y = 2;
    objects->object[0].shape.line.sv.z = 3;
    objects->object[0].shape.line.dv.x = 4;
    objects->object[0].shape.line.dv.y = 5;
    objects->object[0].shape.line.dv.z = 6;

    objects->object[1].name = strdup("A polygon");
    objects->object[1].creator = wcsdup(L"Björk");
    objects->object[1].visible = true;
    objects->object[1].shape.shape_type = ST_POLYGON;
    objects->object[1].shape.polygon.count = 3;
    objects->object[1].shape.polygon.vector =
        calloc(objects->object[1].shape.polygon.count, sizeof(Vector));

    objects->object[1].shape.polygon.vector[0].x =  1;
    objects->object[1].shape.polygon.vector[0].y =  1;
    objects->object[1].shape.polygon.vector[0].z =  0;

    objects->object[1].shape.polygon.vector[1].x = -1;
    objects->object[1].shape.polygon.vector[1].y =  1;
    objects->object[1].shape.polygon.vector[1].z =  0;

    objects->object[1].shape.polygon.vector[2].x =  0;
    objects->object[1].shape.polygon.vector[2].y =  0;
    objects->object[1].shape.polygon.vector[2].z =  2;

    objects->object[2].name = strdup("A plane");
    objects->object[2].creator = wcsdup(L"Björn");
    objects->object[2].visible = false;
    objects->object[2].shape.shape_type = ST_PLANE;
    objects->object[2].shape.plane.sv.x =  1;
    objects->object[2].shape.plane.sv.y =  2;
    objects->object[2].shape.plane.sv.z =  3;
    objects->object[2].shape.plane.nv.x = -1;
    objects->object[2].shape.plane.nv.y = -2;
    objects->object[2].shape.plane.nv.z = -3;

    objects->object[3].name = strdup("A sphere");
    objects->object[3].creator = wcsdup(L"Jürgen");
    objects->object[3].visible = false;
    objects->object[3].shape.shape_type = ST_SPHERE;
    objects->object[3].shape.sphere.c.x = 1;
    objects->object[3].shape.sphere.c.y = 2;
    objects->object[3].shape.sphere.c.z = 3;
    objects->object[3].shape.sphere.r   = 10;
}

static int check_objects(const Objects *objects)
{
    int errors = 0;

    make_sure_that(objects->count == 4);

    make_sure_that(strcmp(objects->object[0].name, "A line") == 0);
    make_sure_that(wcscmp(objects->object[0].creator, L"Øve") == 0);
    make_sure_that(objects->object[0].visible == true);
    make_sure_that(objects->object[0].shape.shape_type == ST_LINE);
    make_sure_that(objects->object[0].shape.line.sv.x == 1);
    make_sure_that(objects->object[0].shape.line.sv.y == 2);
    make_sure_that(objects->object[0].shape.line.sv.z == 3);
    make_sure_that(objects->object[0].shape.line.dv.x == 4);
    make_sure_that(objects->object[0].shape.line.dv.y == 5);
    make_sure_that(objects->object[0].shape.line.dv.z == 6);

    make_sure_that(strcmp(objects->object[1].name, "A polygon") == 0);
    make_sure_that(wcscmp(objects->object[1].creator, L"Björk") == 0);
    make_sure_that(objects->object[1].visible == true);
    make_sure_that(objects->object[1].shape.shape_type == ST_POLYGON);
    make_sure_that(objects->object[1].shape.polygon.count == 3);
    make_sure_that(objects->object[1].shape.polygon.vector[0].x ==  1);
    make_sure_that(objects->object[1].shape.polygon.vector[0].y ==  1);
    make_sure_that(objects->object[1].shape.polygon.vector[0].z ==  0);
    make_sure_that(objects->object[1].shape.polygon.vector[1].x == -1);
    make_sure_that(objects->object[1].shape.polygon.vector[1].y ==  1);
    make_sure_that(objects->object[1].shape.polygon.vector[1].z ==  0);
    make_sure_that(objects->object[1].shape.polygon.vector[2].x ==  0);
    make_sure_that(objects->object[1].shape.polygon.vector[2].y ==  0);
    make_sure_that(objects->object[1].shape.polygon.vector[2].z ==  2);

    make_sure_that(strcmp(objects->object[2].name, "A plane") == 0);
    make_sure_that(wcscmp(objects->object[2].creator, L"Björn") == 0);
    make_sure_that(objects->object[2].visible == false);
    make_sure_that(objects->object[2].shape.shape_type == ST_PLANE);
    make_sure_that(objects->object[2].shape.plane.sv.x ==  1);
    make_sure_that(objects->object[2].shape.plane.sv.y ==  2);
    make_sure_that(objects->object[2].shape.plane.sv.z ==  3);
    make_sure_that(objects->object[2].shape.plane.nv.x == -1);
    make_sure_that(objects->object[2].shape.plane.nv.y == -2);
    make_sure_that(objects->object[2].shape.plane.nv.z == -3);

    make_sure_that(strcmp(objects->object[3].name, "A sphere") == 0);
    make_sure_that(wcscmp(objects->object[3].creator, L"Jürgen") == 0);
    make_sure_that(objects->object[3].visible == false);
    make_sure_that(objects->object[3].shape.shape_type == ST_SPHERE);
    make_sure_that(objects->object[3].shape.sphere.c.x == 1);
    make_sure_that(objects->object[3].shape.sphere.c.y == 2);
    make_sure_that(objects->object[3].shape.sphere.c.z == 3);
    make_sure_that(objects->object[3].shape.sphere.r   == 10);

    return errors;
}

int main(int argc, char *argv[])
{
    int errors = 0;
    size_t size = 0, pos = 0;
    char *buffer = NULL;

    Objects original = { 0 };
    Objects unpacked = { 0 };
    Objects copy     = { 0 };

    Vector center;
    Coordinate radius;
    Sphere *sphere;

    setlocale(LC_CTYPE, "");

    /* Check that the constant was created. */

    make_sure_that(Dimensions == 3);

    fill_objects(&original);

#if DEBUG
    ObjectsPrint(stdout, &original, 0);
#endif

    /* Check Objects packing. */

    size = ObjectsPack(&original, &buffer, &size, &pos);

#if DEBUG
    fprintf(stdout, "size = %lu\n", size);
    hexdump(stdout, buffer, size);
#endif

    make_sure_that(size == 213);

    make_sure_that(memcmp(buffer,
                "\x00\x00\x00\x04"      /* Number of objects */
                                        /* Object 1: */
                "\x00\x00\x00\x06"      /* Object name length */
                "A line"                /* Object name */
                "\x00\x00\x00\x04"      /* Object creator length */
                "\xC3\x98ve"            /* Object creator */
                "\x01"                  /* Object is visible */
                "\x00\x00\x00\x01"      /* Object type (ST_LINE) */
                "\x00\x00\x00\x01"      /* X coordinate of support vector */
                "\x00\x00\x00\x02"      /* Y coordinate of support vector */
                "\x00\x00\x00\x03"      /* Z coordinate of support vector */
                "\x00\x00\x00\x04"      /* X coordinate of direction vector */
                "\x00\x00\x00\x05"      /* Y coordinate of direction vector */
                "\x00\x00\x00\x06"      /* Z coordinate of direction vector */
                                        /* Object 2: */
                "\x00\x00\x00\x09"      /* Object name length */
                "A polygon"             /* Object name */
                "\x00\x00\x00\x06"      /* Object creator length */
                "Bj\xC3\xB6rk"          /* Object creator */
                "\x01"                  /* Object is visible */
                "\x00\x00\x00\x02"      /* Object type (ST_POLYGON) */
                "\x00\x00\x00\x03"      /* Number of vectors */
                "\x00\x00\x00\x01"      /* X coordinate of first vector */
                "\x00\x00\x00\x01"      /* Y coordinate of first vector */
                "\x00\x00\x00\x00"      /* Z coordinate of first vector */
                "\xFF\xFF\xFF\xFF"      /* X coordinate of second vector */
                "\x00\x00\x00\x01"      /* Y coordinate of second vector */
                "\x00\x00\x00\x00"      /* Z coordinate of second vector */
                "\x00\x00\x00\x00"      /* X coordinate of third vector */
                "\x00\x00\x00\x00"      /* Y coordinate of third vector */
                "\x00\x00\x00\x02"      /* Z coordinate of third vector */
                                        /* Object 3: */
                "\x00\x00\x00\x07"      /* Object name length */
                "A plane"               /* Object name */
                "\x00\x00\x00\x06"      /* Object creator length */
                "Bj\xC3\xB6rn"          /* Object creator */
                "\x00"                  /* Object is invisible */
                "\x00\x00\x00\x03"      /* Object type (ST_PLANE) */
                "\x00\x00\x00\x01"      /* X coordinate of support vector */
                "\x00\x00\x00\x02"      /* Y coordinate of support vector */
                "\x00\x00\x00\x03"      /* Z coordinate of support vector */
                "\xFF\xFF\xFF\xFF"      /* X coordinate of normal vector */
                "\xFF\xFF\xFF\xFE"      /* Y coordinate of normal vector */
                "\xFF\xFF\xFF\xFD"      /* Z coordinate of normal vector */
                                        /* Object 4: */
                "\x00\x00\x00\x08"      /* Object name length */
                "A sphere"              /* Object name */
                "\x00\x00\x00\x07"      /* Object creator length */
                "J\xC3\xBCrgen"         /* Object creator */
                "\x00"                  /* Object is invisible */
                "\x00\x00\x00\x04"      /* Object type (ST_SPHERE) */
                "\x00\x00\x00\x01"      /* X coordinate of centre */
                "\x00\x00\x00\x02"      /* Y coordinate of centre */
                "\x00\x00\x00\x03"      /* Z coordinate of centre */
                "\x00\x00\x00\x0A"      /* radius */
                , size) == 0);

    /* Unpack into a new Objects struct and check it's the same. */

    size = ObjectsUnpack(buffer, size, &unpacked);

    make_sure_that(size == 213);

    errors += check_objects(&unpacked);

    /* Copy to a third Objects struct and check that's the same too. */

    ObjectsCopy(&copy, &unpacked);

    errors += check_objects(&copy);

    ObjectsClear(&original);
    ObjectsClear(&unpacked);
    ObjectsClear(&copy);

    free(buffer);

    buffer = NULL;
    size = 0;
    pos = 0;

    VectorWrap(&buffer, &size, &pos, 1, 2, 3);

    make_sure_that(buffer != NULL);
    make_sure_that(size >= 12);
    make_sure_that(pos  == 12);

    make_sure_that(memcmp(buffer,
                "\x00\x00\x00\x01"
                "\x00\x00\x00\x02"
                "\x00\x00\x00\x03",
                pos) == 0);

    VectorUnwrap(buffer, pos, &center.x, &center.y, &center.z);

    make_sure_that(center.x == 1);
    make_sure_that(center.y == 2);
    make_sure_that(center.z == 3);

    free(buffer);

    buffer = NULL;
    size = 0;
    pos = 0;

    VectorSet(&center, -1, -2, -3);

    make_sure_that(center.x == -1);
    make_sure_that(center.y == -2);
    make_sure_that(center.z == -3);

    SphereWrap(&buffer, &size, &pos, &center, 10);

    make_sure_that(buffer != NULL);
    make_sure_that(size >= 16);
    make_sure_that(pos  == 16);

    make_sure_that(memcmp(buffer,
                "\xff\xff\xff\xff"
                "\xff\xff\xff\xfe"
                "\xff\xff\xff\xfd"
                "\x00\x00\x00\x0a",
                pos) == 0);

    memset(&center, 0, sizeof(center));

    SphereUnwrap(buffer, pos, &center, &radius);

    free(buffer);

    buffer = NULL;
    size = 0;
    pos = 0;

    make_sure_that(center.x == -1);
    make_sure_that(center.y == -2);
    make_sure_that(center.z == -3);
    make_sure_that(radius == 10);

    sphere = SphereCreate(&center, radius);

    make_sure_that(sphere->c.x == -1);
    make_sure_that(sphere->c.y == -2);
    make_sure_that(sphere->c.z == -3);
    make_sure_that(sphere->r == 10);

    SphereDestroy(sphere);

    return errors;
}
