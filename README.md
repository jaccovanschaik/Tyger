# Tyger

## Introduction

Tyger is a program that generates code (in C or Python) to manipulate
data structures. These data structures are defined using a simple
language. The generated code can be used to read or write them to
(binary) files, send and receive them across a network, or pack and
unpack them into byte buffers.

## Usage

When called without any parameters, Tyger displays its usage, which
looks (at this moment) like this:

~~~~
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
	  --c-send-mx	Generate MX send functions
	  --c-bcast-mx	Generate MX broadcast functions

	Switches accepted by the Python code generator
	  --py-pack	Generate pack functions
	  --py-unpack	Generate unpack functions
	  --py-recv	Generate recv functions
	  --py-send-mx	Generate MX send functions
	  --py-bcast-mx	Generate MX broadcast functions
~~~~
