#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
  tyger.py: Python runtime module for Tyger.

  Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
  Created:   2016-11-25

  This software is distributed under the terms of the MIT license. See
  http://www.opensource.org/licenses/mit-license.php for details.
'''

import struct

def recv_all(sock, size):
  received = ""

  while len(received) < size:
    data = sock.recv(size - len(received))

    if data == "":
      raise Exception("Lost connection")
    else:
      received += data

  return received

class basePacker(object):
  @classmethod
  def pack(cls, value):
    return struct.pack(cls._format, value)

  @classmethod
  def unpack(cls, buf, offset = 0):
    size = struct.calcsize(cls._format)

    value, = struct.unpack(cls._format, buf[offset:offset + size])

    return value, offset + size

  @classmethod
  def recv(cls, sock):
    size = struct.calcsize(cls._format)

    data = recv_all(sock, size)

    value, = struct.unpack(cls._format, data)

    return value

class uintPacker(object):
  @classmethod
  def pack(cls, num_bytes, value):
    data = bytes()

    for i in range(num_bytes - 1, -1, -1):
      data += uint8Packer.pack((value >> (8 * i)) & 0xFF)

    return data

  @classmethod
  def unpack(cls, num_bytes, buf, offset = 0):
    value = 0

    for i in range(num_bytes - 1, -1, -1):
      b, offset = uint8Packer.unpack(buf, offset)
      value |= (b << (8 * i))

    return value, offset

  @classmethod
  def recv(cls, num_bytes, sock):
    data = recv_all(sock, num_bytes)

    return uintPacker.unpack(num_bytes, data, 0)

class boolPacker(basePacker):
  _format = '>?'

class int8Packer(basePacker):
  _format = '>b'

class int16Packer(basePacker):
  _format = '>h'

class int32Packer(basePacker):
  _format = '>i'

class int64Packer(basePacker):
  _format = '>q'

class uint8Packer(basePacker):
  _format = '>B'

class uint16Packer(basePacker):
  _format = '>H'

class uint32Packer(basePacker):
  _format = '>I'

class uint64Packer(basePacker):
  _format = '>Q'

class float32Packer(basePacker):
  _format = '>f'

class float64Packer(basePacker):
  _format = '>d'

class astringPacker(object):
  @staticmethod
  def pack(value):
    asc = value.encode('ascii')

    return struct.pack(">I", len(asc)) + asc

  @staticmethod
  def unpack(buf, offset = 0):
    length, offset = uint32Packer.unpack(buf, offset)

    asc = buf[offset:offset + length]

    return asc.decode('ascii'), offset + length

  @staticmethod
  def recv(sock):
    length = uint32Packer.recv(sock)

    data = recv_all(sock, length)

    return str(data)

class wstringPacker(object):
  @staticmethod
  def pack(value):
    utf8 = value.encode('utf-8')

    return struct.pack(">I", len(utf8)) + utf8

  @staticmethod
  def unpack(buf, offset = 0):
    length, offset = uint32Packer.unpack(buf, offset)

    utf8 = buf[offset:offset + length]

    return utf8.decode('utf-8'), offset + length

  @staticmethod
  def recv(sock):
    length = uint32Packer.recv(sock)

    data = recv_all(sock, length)

    return data.decode('utf-8')

if __name__ == '__main__':
  s = "abc"
  print("s = %s" % s)

  buf = astringPacker.pack(s)
  print("packed:\n", repr(buf))

  s, offset = astringPacker.unpack(buf)
  print("unpacked: s =", s, ", offset =", offset)

  s = U"αß¢"
  print("s = %s" % s)

  buf = wstringPacker.pack(s)
  print("packed:\n", repr(buf))

  s, offset = wstringPacker.unpack(buf)
  print("unpacked: s =", s, ", offset =", offset)
