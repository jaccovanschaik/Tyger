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

CFLAGS = -g -Wall -Wpointer-arith -fPIC -std=gnu99 $(JVS_INC)

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

test_objects: test_objects.o Objects.o libtyger.a
	$(CC) $(CFLAGS) -o $@ $^ $(JVS_LIB)

test: test-tokenizer test-libtyger test-objects Objects.py Test.o Test.py
	if command -v python2; then python2 ./test_objects.py; fi
	if command -v python3; then python3 ./test_objects.py; fi

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

            # --c-wrap \
            # --c-unwrap \
            # --c-read-fd \
            # --c-write-fd \
            # --c-read-fp \
            # --c-write-fp \
            # --c-print-fp \
            # --c-create \
            # --c-set \
            # --c-copy \
            # --c-clear \
            # --c-destroy \
            # --c-mx-send \
            # --c-mx-bcast $<

Test.h: test/Test.tgr tyger
	./tyger -h Test.h \
            --c-packsize \
            --c-pack \
            --c-unpack \
            --c-clear \
            --c-destroy \
            --c-print \
            $<

            # --c-wrap \
            # --c-unwrap \
            # --c-read-fd \
            # --c-write-fd \
            # --c-read-fp \
            # --c-write-fp \
            # --c-print-fp \
            # --c-create \
            # --c-set \
            # --c-copy \
            # --c-mx-send \
            # --c-mx-bcast $<

Test.py: test/Test.tgr tyger
	./tyger -p Test.py \
            --py-pack \
            --py-unpack \
            --py-recv \
            --py-mx-send \
            --py-mx-bcast \
            test/Test.tgr $<

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

tokenizer-test: tokenizer-test.o utf8.o
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

Objects.c: test/Objects.tgr tyger
	./tyger --indent='    ' --c-src $@ \
            --c-pack \
            --c-unpack \
            --c-packsize \
            --c-destroy \
            --c-clear \
            --c-print \
            $<

            # --c-wrap \
            # --c-unwrap \
            # --c-copy \
            # --c-set \
            # --c-create \
            # --c-print \

Objects.h: test/Objects.tgr tyger
	./tyger --indent='    ' --c-hdr $@ \
            --c-pack \
            --c-unpack \
            --c-packsize \
            --c-clear \
            --c-destroy \
            --c-print \
            $<

            # --c-wrap \
            # --c-unwrap \
            # --c-copy \
            # --c-set \
            # --c-create \
            # --c-print \

Objects.o: Objects.c Objects.h
	$(CC) $(CFLAGS) -I. -c -o $@ $<
	
Objects.py: test/Objects.tgr tyger
	./tyger --indent='    ' --python $@ \
            --py-pack --py-unpack --py-recv \
            --py-mx-send --py-mx-bcast $<

clean:
	rm -rf *.o *.pyc __pycache__ tyger \
            core core-* vgcore.* tyger.tgz libtyger.a libtyger.so \
            tokenizer-test libtyger-test \
            test_objects Objects.c Objects.h Objects.py \
            version.h tokentype.c tokentype.h deftype.c deftype.h \
            Test.c Test.h Test.py

veryclean: clean
	rm -f tags

tags:
	ctags -R --c-kinds=+p /usr/include $(JVS_TOP)/include .

commit:
	@echo "\033[7mGit status:\033[0m"
	@git status
	@echo -n 'Message: '
	@read msg && if [ "$$msg" != '' ]; then git commit -a -m "$$msg" && git push; fi

tyger.tgz: clean
	tar cvf - `ls | grep -v tyger.tgz` | gzip > tyger.tgz

# Dependencies - generated by deps on Thu Aug 25 10:52:16 PM CEST 2022
deftype.o: deftype.c deftype.h 
lang-c.o: lang-c.c switches.h parser.h utils.h utf8.h lang-c.h 
lang-python.o: lang-python.c switches.h parser.h utils.h lang-python.h 
libtyger.o: libtyger.c libtyger.h utf8.h 
parser.o: parser.c tokenizer.h parser.h utils.h 
test_objects.o: test_objects.c Objects.h 
tokenizer.o: tokenizer.c tokenizer.h tokentype.h 
tokentype.o: tokentype.c tokentype.h 
tyger.o: tyger.c parser.h version.h switches.h lang-c.h lang-python.h 
utf8.o: utf8.c utf8.h 
utils.o: utils.c parser.h utils.h 
