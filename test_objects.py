#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
  test_objects.py: Tests for generated Python code.

  Copyright: (c) 2016-2022 Jacco van Schaik (jacco@jaccovanschaik.net)
  Created:   2016-11-25

  This software is distributed under the terms of the MIT license. See
  http://www.opensource.org/licenses/mit-license.php for details.
'''

from Objects import *

s = Vector(1, 2, 3)

assert s.x == 1
assert s.y == 2
assert s.z == 3

n = Vector(4, 5, -6)

assert n.x == 4
assert n.y == 5
assert n.z == -6

s.x = 10

assert s.x == 10
assert s.y == 2
assert s.z == 3

buf = VectorPacker.pack(s)

assert buf == b'\x00\x00\x00\x0a\x00\x00\x00\x02\x00\x00\x00\x03'

p = Plane(s, n)

assert p.sv.x == 10.0
assert p.sv.y == 2
assert p.sv.z == 3

assert p.nv.x == 4
assert p.nv.y == 5
assert p.nv.z == -6

buf = PlanePacker.pack(p)

assert buf == b'\x00\x00\x00\x0a\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x04' \
            + b'\x00\x00\x00\x05\xff\xff\xff\xfa'

p, offset = PlanePacker.unpack(buf)

assert p.sv.x == 10.0
assert p.sv.y == 2
assert p.sv.z == 3

assert p.nv.x == 4
assert p.nv.y == 5
assert p.nv.z == -6

assert offset == 24

s = Shape(ShapeType.ST_PLANE, p)

assert s.shape_type == ShapeType.ST_PLANE
assert s.u.nv.x == 4
assert s.u.nv.y == 5
assert s.u.nv.z == -6

o = Object('A plane', u'Björn', s)

assert o.name == 'A plane'
assert o.creator == U'Björn'
assert o.shape.u.nv.x == 4
assert o.shape.u.nv.y == 5
assert o.shape.u.nv.z == -6

buf = ObjectPacker.pack(o)

expected = \
  b"\x00\x00\x00\x07A plane" \
+ b"\x00\x00\x00\x06Bj\xc3\xb6rn" \
+ b"\x00\x00\x00\x03" \
+ b"\x00\x00\x00\x0a" \
+ b"\x00\x00\x00\x02" \
+ b"\x00\x00\x00\x03" \
+ b"\x00\x00\x00\x04" \
+ b"\x00\x00\x00\x05" \
+ b"\xff\xff\xff\xfa"

assert buf == expected

o, offset = ObjectPacker.unpack(buf)

assert o.name == 'A plane'
assert o.creator == U'Björn'
assert o.shape.u.nv.x == 4
assert o.shape.u.nv.y == 5
assert o.shape.u.nv.z == -6

assert offset == 49

l = 3 * [ o ]

buf = ObjectsPacker.pack(l)

assert buf == b"\x00\x00\x00\x03" + expected + expected + expected

l, offset = ObjectsPacker.unpack(buf)

assert len(l) == 3

for o in l:
  assert o.name == 'A plane'
  assert o.creator == U'Björn'
  assert o.shape.u.nv.x == 4
  assert o.shape.u.nv.y == 5
  assert o.shape.u.nv.z == -6
