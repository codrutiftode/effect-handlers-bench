# This file uses global variables declared in the top-level Makefile.

BUILD := release

PICOHTTP_LOC := $(shell pwd)
PICOHTTP_LIB := ${PICOHTTP_LOC}/picohttpparser/build/picohttpparser.a
PICOHTTP_INCLUDE_DIRS := -I${PICOHTTP_LOC}/picohttpparser

LD_FLAGS := ${LD_FLAGS} -l:${PICOHTTP_LIB}

CC_FLAGS.debug :=
CC_FLAGS.release := -DNDEBUG -flto=thin
SERVER_COMPILE_FLAGS := ${CC_FLAGS.${BUILD}} $(CC_COMPILE_FLAGS)

LD_FLAGS.debug :=
LD_FLAGS.release := -flto=thin
SERVER_LINK_FLAGS := ${LD_FLAGS.${BUILD}} $(LD_FLAGS)

output:
	mkdir $@

utils: output/net.o output/tl_queue.o output/http_response.o output/net.o

build: output/libscheff.o output/scheff.o utils
	$(CC) $(CC_LINK_FLAGS) -o output/server output/*.o $(SERVER_LINK_FLAGS)

clean:
	-rm output/*
	-rmdir output

output/%.o: %.c $(PICOHTTP_LIB) | output
	$(CC) $(SERVER_COMPILE_FLAGS) $(PICOHTTP_INCLUDE_DIRS) -c $< -o $@ 

${PICOHTTP_LOC}/picohttpparser/build:
	mkdir -p $@

${PICOHTTP_LOC}/picohttpparser/build/picohttpparser.o: ${PICOHTTP_LOC}/picohttpparser/picohttpparser.c ${PICOHTTP_LOC}/picohttpparser/picohttpparser.h | ${PICOHTTP_LOC}/picohttpparser/build
	$(CC) $(CFLAGS) -o $@ -c $<

$(PICOHTTP_LIB): ${PICOHTTP_LOC}/picohttpparser/build/picohttpparser.o | ${PICOHTTP_LOC}/picohttpparser/build
	llvm-ar-14 -rcs $@ $<

bench: build
	output/server &	
	wrk -t2 -c100 -d30s -R2000 http://127.0.0.1:8082/
	kill "$$(pgrep server)"	