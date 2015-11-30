# CXXFLAGS := $(shell pkg-config --cflags zlib libpng)
# LDFLAGS := $(shell pkg-config --libs zlib libpng)

CXX=em++

EM_SCRIPT_OPTIONS=-s EXPORTED_FUNCTIONS='["_mainy"]' -s TOTAL_MEMORY=100000000 --js-library flif-library.js

# optimisation options
EM_SCRIPT_OPTIONS+= -s NO_FILESYSTEM=1 -s NO_BROWSER=1
EM_SCRIPT_OPTIONS+= -s NODE_STDOUT_FLUSH_WORKAROUND=0 -s INVOKE_RUN=0
EM_SCRIPT_OPTIONS+= -s ASSERTIONS=0
EM_SCRIPT_OPTIONS+= --closure 1
EM_SCRIPT_OPTIONS+=-s ASM_JS=2

#EM_SCRIPT_OPTIONS+=-s MODULARIZE=1
EM_SCRIPT_OPTIONS+=-s AGGRESSIVE_VARIABLE_ELIMINATION=1
EM_SCRIPT_OPTIONS+=-s RUNNING_JS_OPTS=1
EM_SCRIPT_OPTIONS+=-s DISABLE_EXCEPTION_CATCHING=1
EM_SCRIPT_OPTIONS+=-s NO_EXIT_RUNTIME=1
EM_SCRIPT_OPTIONS+= -s USE_SDL=0
EM_SCRIPT_OPTIONS+=--memory-init-file 0

CXXFLAGS += ${EM_SCRIPT_OPTIONS}

em-out/flif.html: maniac/*.hpp maniac/*.cpp image/*.hpp image/*.cpp transform/*.hpp transform/*.cpp flif-em.cpp flif-enc.cpp flif-dec.cpp common.cpp flif-enc.hpp flif-dec.hpp common.hpp flif_config.h fileio.hpp bufferio.h io.hpp io.cpp config.h Makefile flif-library.js
	${CXX} -std=gnu++11 $(CXXFLAGS) $(LDFLAGS) -DNDEBUG -Oz -g0 -Wall maniac/chance.cpp image/color_range.cpp transform/factory.cpp flif-em.cpp common.cpp flif-dec.cpp io.cpp -o em-out/flif.html

# for running interface-test
export LD_LIBRARY_PATH=$(shell pwd):$LD_LIBRARY_PATH

FILES_H := maniac/*.hpp maniac/*.cpp image/*.hpp transform/*.hpp flif-enc.hpp flif-dec.hpp common.hpp flif_config.h fileio.hpp io.hpp io.cpp config.h
FILES_CPP := maniac/chance.cpp maniac/symbol.cpp image/crc32k.cpp image/image.cpp image/image-png.cpp image/image-pnm.cpp image/image-pam.cpp image/image-rggb.cpp image/color_range.cpp transform/factory.cpp common.cpp flif-enc.cpp flif-dec.cpp io.cpp

all: flif libflif.so viewflif

# options to consider: -ftree-vectorize -fvect-cost-model=unlimited -funroll-all-loops
OPTIMIZATIONS := -DNDEBUG -O3 -ftree-vectorize
# there are often problems with clang and lto also it doesn't seem to know -fwhole-program
ifeq ($(CXX), g++)
	OPTIMIZATIONS := $(OPTIMIZATIONS) -flto -fwhole-program
endif

LIB_OPTIMIZATIONS := -DNDEBUG -O3
ifeq ($(CXX), g++)
	LIB_OPTIMIZATIONS := $(LIB_OPTIMIZATIONS) -flto
endif

flif: $(FILES_H) $(FILES_CPP) flif.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) $(OPTIMIZATIONS) -g0 -Wall $(FILES_CPP) flif.cpp $(LDFLAGS) -o flif

flif.stats: $(FILES_H) $(FILES_CPP) flif.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) -DSTATS $(OPTIMIZATIONS) -g0 -Wall $(FILES_CPP) flif.cpp $(LDFLAGS) -o flif.stats

flif.prof: $(FILES_H) $(FILES_CPP) flif.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) $(OPTIMIZATIONS) -g0 -pg -Wall $(FILES_CPP) flif.cpp $(LDFLAGS) -o flif.prof

flif.dbg: $(FILES_H) $(FILES_CPP) flif.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) -Og -ggdb3 -Wall $(FILES_CPP) flif.cpp $(LDFLAGS) -o flif.dbg

flif.asan: $(FILES_H) $(FILES_CPP) flif.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) $(OPTIMIZATIONS) -fsanitize=address,undefined -fno-omit-frame-pointer -g3 -Wall $(FILES_CPP) flif.cpp $(LDFLAGS) -o flif.asan

libflif.so: $(FILES_H) $(FILES_CPP) flif.h flif-interface-private.h flif-interface.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) $(LIB_OPTIMIZATIONS) -g0 -Wall -shared -fPIC $(FILES_CPP) flif-interface.cpp $(LDFLAGS) -o libflif.so

libflif.dbg.so: $(FILES_H) $(FILES_CPP) flif.h flif-interface-private.h flif-interface.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) -O1 -ggdb3 -Wall -shared -fPIC $(FILES_CPP) flif-interface.cpp $(LDFLAGS) -o libflif.dbg.so

viewflif: libflif.so flif.h viewflif.c
	$(CC) -std=gnu11 -O2 -ggdb3 $(shell sdl2-config --cflags) -Wall -I. viewflif.c -L. -lflif $(shell sdl2-config --libs) -o viewflif

test-interface: libflif.dbg.so flif.h ../tools/test.c
	$(CC) -O0 -ggdb3 -Wall -I. ../tools/test.c -L. -lflif.dbg  -o test-interface

install: all
	install -s -m 755 flif viewflif $(PREFIX)/bin
	install -s -m 755 libflif.so $(PREFIX)/lib
	install -m 644 ../doc/flif.1 $(PREFIX)/share/man/man1
	if grep -q FLIF /etc/magic; then echo "Magic already known" ; else cat ../doc/flif.magic >> /etc/magic; fi

remove:
	rm -f $(PREFIX)/bin/flif
	rm -f $(PREFIX)/bin/viewflif
	rm -f $(PREFIX)/lib/libflif.so
	rm -f $(PREFIX)/share/man/man1/flif.1

clean:
	rm -f flif libflif*.so viewflif flif.asan flif.dbg flif.prof flif.stats

test: flif test-interface
	mkdir -p ../tmp-test
	./test-interface
	../tools/test-roundtrip.sh ../tools/2_webp_ll.png ../tmp-test/2_webp_ll.flif ../tmp-test/decoded_2_webp_ll.png
	../tools/test-roundtrip.sh ../tools/kodim01.png ../tmp-test/kodim01.flif ../tmp-test/decoded_kodim01.png

