/* test_objects.c: Tests for generated C code.
 *
 * Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
 * Created:   2016-09-14
 *
 * This software is distributed under the terms of the MIT license. See
 * http://www.opensource.org/licenses/mit-license.php for details.
 *
 * vim: textwidth=100 columns=100
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include <libjvs/utils.h>

#include "Objects.h"

#define DEBUG 1

static void fill_objects(Objects *objects)
{
    objects->count = 4;
    objects->object = calloc(objects->count, sizeof(Object));

    objects->object[0].name = astringMake("A line");
    objects->object[0].creator = ustringCreate(L"Øve");
    objects->object[0].visible = true;
    objects->object[0].shape.shape_type = ST_LINE;
    objects->object[0].shape.line.sv.x = 1;
    objects->object[0].shape.line.sv.y = 2;
    objects->object[0].shape.line.sv.z = 3;
    objects->object[0].shape.line.dv.x = 4;
    objects->object[0].shape.line.dv.y = 5;
    objects->object[0].shape.line.dv.z = 6;

    objects->object[1].name = astringMake("A polygon");
    objects->object[1].creator = ustringCreate(L"Björk");
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

    objects->object[2].name = astringMake("A plane");
    objects->object[2].creator = ustringCreate(L"José");
    objects->object[2].visible = false;
    objects->object[2].shape.shape_type = ST_PLANE;
    objects->object[2].shape.plane.sv.x =  1;
    objects->object[2].shape.plane.sv.y =  2;
    objects->object[2].shape.plane.sv.z =  3;
    objects->object[2].shape.plane.nv.x = -1;
    objects->object[2].shape.plane.nv.y = -2;
    objects->object[2].shape.plane.nv.z = -3;

    objects->object[3].name = astringMake("A sphere");
    objects->object[3].creator = NULL;
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

    fprintf(stderr, "%s\n", __func__);

    make_sure_that(objects->count == 4);

    make_sure_that(strcmp(astringGet(&objects->object[0].name), "A line") == 0);
    make_sure_that(wcscmp(ustringGet(objects->object[0].creator), L"Øve") == 0);
    make_sure_that(objects->object[0].visible == true);
    make_sure_that(objects->object[0].shape.shape_type == ST_LINE);
    make_sure_that(objects->object[0].shape.line.sv.x == 1);
    make_sure_that(objects->object[0].shape.line.sv.y == 2);
    make_sure_that(objects->object[0].shape.line.sv.z == 3);
    make_sure_that(objects->object[0].shape.line.dv.x == 4);
    make_sure_that(objects->object[0].shape.line.dv.y == 5);
    make_sure_that(objects->object[0].shape.line.dv.z == 6);

    make_sure_that(strcmp(astringGet(&objects->object[1].name), "A polygon") == 0);
    make_sure_that(wcscmp(ustringGet(objects->object[1].creator), L"Björk") == 0);
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

    make_sure_that(strcmp(astringGet(&objects->object[2].name), "A plane") == 0);
    make_sure_that(wcscmp(ustringGet(objects->object[2].creator), L"José") == 0);
    make_sure_that(objects->object[2].visible == false);
    make_sure_that(objects->object[2].shape.shape_type == ST_PLANE);
    make_sure_that(objects->object[2].shape.plane.sv.x ==  1);
    make_sure_that(objects->object[2].shape.plane.sv.y ==  2);
    make_sure_that(objects->object[2].shape.plane.sv.z ==  3);
    make_sure_that(objects->object[2].shape.plane.nv.x == -1);
    make_sure_that(objects->object[2].shape.plane.nv.y == -2);
    make_sure_that(objects->object[2].shape.plane.nv.z == -3);

    make_sure_that(strcmp(astringGet(&objects->object[3].name), "A sphere") == 0);
    make_sure_that(objects->object[3].creator == NULL);
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
    size_t pos;
    buffer buf = { };

    Objects original = { 0 };
    Objects unpacked = { 0 };
    // Objects copy     = { 0 };

    // Vector center;
    // Coordinate radius;
    // Sphere *sphere;

    setlocale(LC_CTYPE, "");

    /* Check that the constant was created. */

    make_sure_that(Dimensions == 3);

    fill_objects(&original);

#if DEBUG
    ObjectsPrint(stdout, &original, 0);
#endif

    errors += check_objects(&original);

    /* Check Objects packing. */

    ObjectsPack(&original, &buf);

    hexdump(stderr, (const char *) buf.data, buf.len);

    make_sure_that(buf.len == 197);

#if DEBUG
    fprintf(stdout, "size = %lu\n", buf.len);
    hexdump(stdout, (char *) buf.data, buf.len);
#endif

    make_sure_that(memcmp(buf.data,
                "\x00\x00\x00\x04"      /* Number of objects */
                                        /* Object 1: */
                "\x00\x00\x00\x06"      /* Object name length */
                "A line"                /* Object name */
                "\x01"                  /* Object creator present? */
                "\x00\x00\x00\x04"      /* Object creator length */
                "\xC3\x98ve"            /* Object creator */
                "\x01"                  /* Object is visible */
                "\x00\x01"              /* Object type (ST_LINE) */
                "\x00\x00\x00\x01"      /* X coordinate of support vector */
                "\x00\x00\x00\x02"      /* Y coordinate of support vector */
                "\x00\x00\x00\x03"      /* Z coordinate of support vector */
                "\x00\x00\x00\x04"      /* X coordinate of direction vector */
                "\x00\x00\x00\x05"      /* Y coordinate of direction vector */
                "\x00\x00\x00\x06"      /* Z coordinate of direction vector */
                                        /* Object 2: */
                "\x00\x00\x00\x09"      /* Object name length */
                "A polygon"             /* Object name */
                "\x01"                  /* Object creator present? */
                "\x00\x00\x00\x06"      /* Object creator length */
                "Bj\xC3\xB6rk"          /* Object creator */
                "\x01"                  /* Object is visible */
                "\x00\x02"              /* Object type (ST_POLYGON) */
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
                "\x01"                  /* Object creator present? */
                "\x00\x00\x00\x05"      /* Object creator length */
                "Jos\xC3\xA9"           /* Object creator */
                "\x00"                  /* Object is invisible */
                "\x12\x34"              /* Object type (ST_PLANE) */
                "\x00\x00\x00\x01"      /* X coordinate of support vector */
                "\x00\x00\x00\x02"      /* Y coordinate of support vector */
                "\x00\x00\x00\x03"      /* Z coordinate of support vector */
                "\xFF\xFF\xFF\xFF"      /* X coordinate of normal vector */
                "\xFF\xFF\xFF\xFE"      /* Y coordinate of normal vector */
                "\xFF\xFF\xFF\xFD"      /* Z coordinate of normal vector */
                                        /* Object 4: */
                "\x00\x00\x00\x08"      /* Object name length */
                "A sphere"              /* Object name */
                "\x00"                  /* Object creator present? */
                "\x00"                  /* Object is invisible */
                "\x43\x21"              /* Object type (ST_SPHERE) */
                "\x00\x00\x00\x01"      /* X coordinate of centre */
                "\x00\x00\x00\x02"      /* Y coordinate of centre */
                "\x00\x00\x00\x03"      /* Z coordinate of centre */
                "\x00\x00\x00\x0A"      /* radius */
                , buf.len) == 0);

    /* Unpack into a new Objects struct and check it's the same. */

    pos = ObjectsUnpack(&buf, 0, &unpacked);

    make_sure_that(pos == 197);

#if DEBUG
    ObjectsPrint(stdout, &unpacked, 0);
#endif

    errors += check_objects(&unpacked);

    // ObjectsClear(&original);
    ObjectsClear(&unpacked);
    bufferClear(&buf);

    /* Copy to a third Objects struct and check that's the same too. */

#if 0
    ObjectsCopy(&copy, &unpacked);

    errors += check_objects(&copy);

    ObjectsClear(&original);
    ObjectsClear(&unpacked);
    ObjectsClear(&copy);

    free(buf);

    buf = NULL;
    size = 0;
    pos = 0;

    VectorWrap(&buf, &size, &pos, 1, 2, 3);

    make_sure_that(buf != NULL);
    make_sure_that(size >= 12);
    make_sure_that(pos  == 12);

    make_sure_that(memcmp(buf,
                "\x00\x00\x00\x01"
                "\x00\x00\x00\x02"
                "\x00\x00\x00\x03",
                pos) == 0);

    VectorUnwrap(buf, pos, &center.x, &center.y, &center.z);

    make_sure_that(center.x == 1);
    make_sure_that(center.y == 2);
    make_sure_that(center.z == 3);

    free(buf);

    buf = NULL;
    size = 0;
    pos = 0;

    VectorSet(&center, -1, -2, -3);

    make_sure_that(center.x == -1);
    make_sure_that(center.y == -2);
    make_sure_that(center.z == -3);

    SphereWrap(&buf, &size, &pos, &center, 10);

    make_sure_that(buf != NULL);
    make_sure_that(size >= 16);
    make_sure_that(pos  == 16);

    make_sure_that(memcmp(buf,
                "\xff\xff\xff\xff"
                "\xff\xff\xff\xfe"
                "\xff\xff\xff\xfd"
                "\x00\x00\x00\x0a",
                pos) == 0);

    memset(&center, 0, sizeof(center));

    SphereUnwrap(buf, pos, &center, &radius);

    free(buf);

    buf = NULL;
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
#endif

    return errors;
}
