#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
  tyger.py: Python runtime module for Tyger.
 
  Copyright: (c) 2016 Jacco van Schaik (jacco@jaccovanschaik.net)
  Created:   2016-11-25
  Version:   $Id: tyger.py 127 2017-05-14 17:20:46Z jacco $
 
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
    buf = uint32Packer.pack(len(value))

    return buf + value

  @staticmethod
  def unpack(buf, offset = 0):
    length, offset = uint32Packer.unpack(buf, offset)

    return buf[offset:offset + length], offset + length

  @staticmethod
  def recv(sock):
    length = uint32Packer.recv(sock)

    data = recv_all(sock, length)

    return str(data)

class ustringPacker(object):
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
  from hexdump import hexdump

  s = "abc"
  print "s = %s" % s

  buf = astringPacker.pack(s)
  print "packed:\n", hexdump(buf)

  s, offset = astringPacker.unpack(buf)
  print "unpacked: s =", s, ", offset =", offset

  s = U"αß¢"
  print "s = %s" % s

  buf = ustringPacker.pack(s)
  print "packed:\n", hexdump(buf)

  s, offset = ustringPacker.unpack(buf)
  print "unpacked: s =", s, ", offset =", offset
