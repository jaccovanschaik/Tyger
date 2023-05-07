# Makefile: Makefile for tyger.
#
# Copyright: (c) 2016-2023 Jacco van Schaik (jacco@jaccovanschaik.net)
# Created:   2016-08-24
#
# This software is distributed under the terms of the MIT license. See
# http://www.opensource.org/licenses/mit-license.php for details.

.PHONY: test tags version.h

TYGER = tokenizer.o tokentype.o deftype.o \
        parser.o tyger.o utils.o lang-c.o lang-python.o libtyger.a

JVS_TOP=$(HOME)
JVS_INC=-I$(JVS_TOP)/include
JVS_LIB=-L$(JVS_TOP)/lib -ljvs

CFLAGS = -g -Wall -Wpointer-arith -fPIC -std=gnu99 -Itest $(JVS_INC)

MAKE_ALIB=ar rv
MAKE_SLIB=$(CC) -shared -o

INSTALL_TOP=$(HOME)
INSTALL_BIN=$(INSTALL_TOP)/bin
INSTALL_INC=$(INSTALL_TOP)/include
INSTALL_LIB=$(INSTALL_TOP)/lib
INSTALL_PY=$(INSTALL_TOP)/.python

all: tyger libtyger.a libtyger.so

tyger: $(TYGER)
	$(CC) $(CFLAGS) -o tyger $(TYGER) $(JVS_LIB)

libtyger.a: libtyger.o utf8.o
	$(MAKE_ALIB) $@ $^

libtyger.so: libtyger.o utf8.o
	$(MAKE_SLIB) $@ $^

%.c: %.tgr tyger
	./tyger -c $@ \
            --c-packsize \
            --c-pack \
            --c-unpack \
            --c-clear \
            --c-destroy \
            --c-print \
            $<

%.h: %.tgr tyger
	./tyger -h $@ \
            --c-packsize \
            --c-pack \
            --c-unpack \
            --c-clear \
            --c-destroy \
            --c-print \
            $<

%.py: %.tgr tyger
	./tyger -p $@ \
            --py-pack \
            --py-unpack \
            --py-recv \
            --py-mx-send \
            --py-mx-bcast \
            $<

%.d: %.c
	$(CC) $(CFLAGS) -MM -MG -MF $@ $<

OBJECT_TGR = $(wildcard test/*.tgr)
OBJECT_SRC = $(patsubst %.tgr,%.c,$(OBJECT_TGR))
OBJECT_HDR = $(patsubst %.tgr,%.h,$(OBJECT_TGR))
OBJECT_OBJ = $(patsubst %.tgr,%.o,$(OBJECT_TGR))
OBJECT_PY  = $(patsubst %.tgr,%.py,$(OBJECT_TGR))

.PRECIOUS: $(OBJECT_SRC) $(OBJECT_HDR)

test_objects.o: test_objects.c $(OBJECT_HDR)

test_objects: test_objects.o $(OBJECT_OBJ) libtyger.a
	$(CC) $(CFLAGS) -o $@ $^ $(JVS_LIB)

test: test-tokenizer test-libtyger test-objects $(OBJECT_PY) Test.o Test.py
	if command -v python2; then \
            PYTHONPATH=$$PYTHONPATH:test python2 ./test_objects.py; \
        fi
	if command -v python3; then \
            PYTHONPATH=$$PYTHONPATH:test python3 ./test_objects.py; \
        fi

test-tokenizer: tokenizer-test
	./tokenizer-test

test-libtyger: libtyger-test
	./libtyger-test

test-objects: test_objects
	./test_objects

Test.o: Test.c Test.h
	$(CC) -c $(CFLAGS) -o $@ $< $(JVS_LIB)

Test.c: test/Test.tgr tyger
	./tyger -c Test.c \
            --c-packsize \
            --c-pack \
            --c-unpack \
            --c-clear \
            --c-destroy \
            --c-print \
            $<

Test.h: test/Test.tgr tyger
	./tyger -h Test.h \
            --c-packsize \
            --c-pack \
            --c-unpack \
            --c-clear \
            --c-destroy \
            --c-print \
            $<

Test.py: test/Test.tgr tyger
	./tyger -p Test.py \
            --py-pack \
            --py-unpack \
            --py-recv \
            --py-mx-send \
            --py-mx-bcast \
            $<

tokentype.c: tokentype.txt
	./gen_enum -c -p TT -t tkType $< > $@

tokentype.h: tokentype.txt
	./gen_enum -h -p TT -t tkType $< > $@

deftype.c: deftype.txt
	./gen_enum -c -p DT -t DefinitionType $< > $@

deftype.h: deftype.txt
	./gen_enum -h -p DT -t DefinitionType $< > $@

version.h:
	echo "#define VERSION" \
        \"$$(git log -n 1 --format=%cd --date=format:%Y-%m-%d/%H:%M:%S)\" \
        > version.h

tokenizer-test.o: tokenizer.c tokenizer.h tokentype.h
	$(CC) $(CFLAGS) -c -DTEST -o $@ $<

libtyger-test.o: libtyger.c libtyger.h tokentype.h
	$(CC) $(CFLAGS) -c -DTEST -o $@ $<

tokenizer-test: tokenizer-test.o tokentype.o utf8.o
	$(CC) $(CFLAGS) -DTEST -o $@ $^ $(JVS_LIB)

libtyger-test: libtyger-test.o utf8.o
	$(CC) $(CFLAGS) -DTEST -o $@ $^ $(JVS_LIB)

force-install: tyger libtyger.a libtyger.so
	if [ ! -d $(INSTALL_BIN) ]; then mkdir -p $(INSTALL_BIN); fi
	cp tyger $(INSTALL_BIN)
	if [ ! -d $(INSTALL_INC) ]; then mkdir -p $(INSTALL_INC); fi
	cp libtyger.h $(INSTALL_INC)
	if [ ! -d $(INSTALL_LIB) ]; then mkdir -p $(INSTALL_LIB); fi
	cp libtyger.a libtyger.so $(INSTALL_LIB)
	if [ ! -d $(INSTALL_PY) ]; then mkdir -p $(INSTALL_PY); fi
	cp tyger.py $(INSTALL_PY)

install: test force-install

clean:
	rm -rf *.o *.pyc __pycache__ test/__pycache__ tyger \
            core core-* vgcore.* tyger.tgz libtyger.a libtyger.so \
            tokenizer-test libtyger-test test_objects \
            version.h tokentype.c tokentype.h deftype.c deftype.h \
            test/*.c test/*.h test/*.py test/*.pyc test/*.d test/*.o \
            Test.c Test.h Test.py

veryclean: clean
	rm -f tags

tags:
	ctags -R /usr/include $(JVS_TOP)/Projects/libjvs .

commit:
	@echo "\033[7mGit status:\033[0m"
	@git status
	@echo -n 'Message: '
	@read msg && if [ "$$msg" != '' ]; then git commit -a -m "$$msg" && git push; fi

tyger.tgz: clean
	tar cvf - `ls | grep -v tyger.tgz` | gzip > tyger.tgz

# Dependencies - generated by deps on Sun Jan 15 01:43:45 PM CET 2023
deftype.o: deftype.c deftype.h 
lang-c.o: lang-c.c switches.h parser.h utils.h utf8.h lang-c.h 
lang-python.o: lang-python.c switches.h parser.h utils.h lang-python.h 
libtyger.o: libtyger.c libtyger.h utf8.h 
parser.o: parser.c tokenizer.h parser.h utils.h 
tokenizer.o: tokenizer.c tokenizer.h tokentype.h 
tokentype.o: tokentype.c tokentype.h 
tyger.o: tyger.c parser.h version.h switches.h lang-c.h lang-python.h 
utf8.o: utf8.c utf8.h 
utils.o: utils.c parser.h utils.h 
