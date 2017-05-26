# Tyger

## Introduction

Tyger is a program that generates code (in C or Python) to manipulate
data structures. These data structures are defined using a simple
language. The generated code can be used to read or write them to
(binary) files, send and receive them across a network, or pack and
unpack them into byte buffers.

Tyger requires [libjvs](https://github.com/jaccovanschaik/libjvs).

## Usage

When called without any parameters, Tyger displays its usage, which
looks (at this moment) like this:

	Usage: tyger <options> <input-file>
	
	Options:
		-V, --version			Print version and exit.
		-c, --c-src <C-source-output>	Output C source file here.
		-h, --c-hdr <C-header-output>	Output C header file here.
		-p, --python <python-output>	Output python code here.
		-i, --indent <indent-string>	Use this string as indent.
	
		Switches accepted by the C code generator
		  --c-packsize	Generate packsize functions
		  --c-pack	Generate pack functions
		  --c-unpack	Generate unpack functions
		  --c-wrap	Generate wrap functions
		  --c-unwrap	Generate unwrap functions
		  --c-read-fd	Generate functions to read from an fd
		  --c-write-fd	Generate functions to write to an fd
		  --c-read-fp	Generate functions to read from an FP
		  --c-write-fp	Generate functions to write to an FP
		  --c-print-fp	Generate print functions
		  --c-create	Generate create functions
		  --c-set	Generate set functions
		  --c-copy	Generate copy functions
		  --c-clear	Generate clear functions
		  --c-destroy	Generate destroy functions
		  --c-mx-send	Generate MX send functions
		  --c-mx-bcast	Generate MX broadcast functions
	
		Switches accepted by the Python code generator
		  --py-pack	Generate pack functions
		  --py-unpack	Generate unpack functions
		  --py-recv	Generate recv functions
		  --py-mx-send	Generate MX send functions
		  --py-mx-bcast	Generate MX broadcast functions

## Input files

Tyger input files contain type definitions.

### Integers

Integers are simple whole numbers. Their name is either `int` or `uint`
(defining a signed or an unsigned integer), followed by 8, 16, 32 or 64,
defining the number of bits they contain.

Example:

    Counter = int32

### Floating point numbers

    Coordinate = float64
    
### Strings

    ident = astring
    name = ustring

### Enumerations

    ShapeType = enum {
        ST_LINE    = 1
        ST_POLYGON = 2
        ST_PLANE   = 3
        ST_SPHERE  = 4
    }

### Structures

    Vector = struct {
        Coordinate x
        Coordinate y
        Coordinate z
    }

### Unions

    Shape = union(ShapeType shape_type) {
        ST_LINE:    Line    line
        ST_POLYGON: Polygon polygon
        ST_PLANE:   Plane   plane
        ST_SPHERE:  Sphere  sphere
    }

### Arrays

    Polygon = array(Vector vector)

### Constants

    Dimensions = const uint32 3

## General options

This section describes Tygers general options.

- `-V` or `--version`

  Print version and exit.

- `-c` or `--c-src <C-source-output>`

  Output C source file to the file given in `<C-source-output>`.

- `-h` or `--c-hdr <C-header-output>`

  Output C header file to the file given in `<C-header-output>`.

- `-p` or `--python <python-output>`

  Output python code to the file given in `<python-output>`.

- `-i` or `--indent <indent-string>`

  Use the string given in `<indent-string>` as one level of indent.

## Language-specific switches

This section describes language specific switches.

### C code generator

This section explains the switches accepted by the C code generator.

- `--c-packsize`

  Generate packsize functions.

- `--c-pack`
  
  Generate pack functions

- `--c-unpack`
  
  Generate unpack functions

- `--c-wrap`
  
  Generate wrap functions

- `--c-unwrap`
  
  Generate unwrap functions

- `--c-read-fd`
  
  Generate functions to read from an fd

- `--c-write-fd`
  
  Generate functions to write to an fd

- `--c-read-fp`
  
  Generate functions to read from an FP

- `--c-write-fp`
  
  Generate functions to write to an FP

- `--c-print-fp`
  
  Generate print functions

- `--c-create`
  
  Generate create functions

- `--c-set`
  
  Generate set functions

- `--c-copy`
  
  Generate copy functions

- `--c-clear`
  
  Generate clear functions

- `--c-destroy`
  
  Generate destroy functions

- `--c-mx-send`
  
  Generate MX send functions

- `--c-mx-bcast`
  
  Generate MX broadcast functions

