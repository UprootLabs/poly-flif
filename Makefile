# CXXFLAGS := $(shell pkg-config --cflags zlib libpng)
# LDFLAGS := $(shell pkg-config --libs zlib libpng)

CXX=em++

EM_SCRIPT_OPTIONS=-s EXPORTED_FUNCTIONS='["_mainy"]' -s TOTAL_MEMORY=20000000 --js-library flif-library.js

# optimisation options
EM_SCRIPT_OPTIONS+= -s NO_FILESYSTEM=1 -s NO_BROWSER=1
EM_SCRIPT_OPTIONS+= -s NODE_STDOUT_FLUSH_WORKAROUND=0 -s INVOKE_RUN=0
EM_SCRIPT_OPTIONS+= -s ASSERTIONS=0
EM_SCRIPT_OPTIONS+= --closure 1
EM_SCRIPT_OPTIONS+=-s ASM_JS=2

CXXFLAGS += ${EM_SCRIPT_OPTIONS}

em-out/flif.html: maniac/*.h maniac/*.cpp image/*.h image/*.cpp transform/*.h transform/*.cpp flif-em.cpp flif-enc.cpp flif-dec.cpp common.cpp flif-enc.h flif-dec.h common.h flif_config.h fileio.h bufferio.h io.h io.cpp Makefile flif-library.js
	${CXX} -std=gnu++11 $(CXXFLAGS) $(LDFLAGS) -DNDEBUG -Oz -g0 -Wall maniac/util.cpp maniac/chance.cpp image/crc32k.cpp image/color_range.cpp transform/factory.cpp flif-em.cpp common.cpp flif-dec.cpp io.cpp -o em-out/flif.html

flif: maniac/*.h maniac/*.cpp image/*.h image/*.cpp transform/*.h transform/*.cpp flif.cpp flif-enc.cpp flif-dec.cpp common.cpp flif-enc.h flif-dec.h common.h flif_config.h fileio.h io.h io.cpp
	$(CXX) -std=gnu++11 $(CXXFLAGS) -DNDEBUG -O3 -g0 -Wall maniac/chance.cpp image/crc32k.cpp image/image.cpp image/image-png.cpp image/image-pnm.cpp image/image-pam.cpp image/color_range.cpp transform/factory.cpp flif.cpp common.cpp flif-enc.cpp flif-dec.cpp io.cpp -o flif $(LDFLAGS)

flif.prof: maniac/*.h maniac/*.cpp image/*.h image/*.cpp transform/*.h transform/*.cpp flif.cpp flif_config.h
	$(CXX) -std=gnu++11 $(CXXFLAGS) $(LDFLAGS) -DNDEBUG -O3 -g0 -pg -Wall maniac/chance.cpp image/crc32k.cpp image/image.cpp image/image-png.cpp image/image-pnm.cpp image/image-pam.cpp image/color_range.cpp transform/factory.cpp flif.cpp common.cpp flif-enc.cpp flif-dec.cpp -o flif.prof

flif.dbg: maniac/*.h maniac/*.cpp image/*.h image/*.cpp transform/*.h transform/*.cpp flif.cpp flif_config.h
	$(CXX) -std=gnu++11 $(CXXFLAGS) $(LDFLAGS) -O0 -ggdb3 -Wall maniac/chance.cpp image/crc32k.cpp image/image.cpp image/image-png.cpp image/image-pnm.cpp image/image-pam.cpp image/color_range.cpp transform/factory.cpp flif.cpp common.cpp flif-enc.cpp flif-dec.cpp -o flif.dbg

test: flif
	mkdir -p testFiles
	./tools/test-roundtrip.sh benchmark/input/webp_gallery/2_webp_ll.png testFiles/2_webp_ll.flif testFiles/decoded_2_webp_ll.png
	./tools/test-roundtrip.sh benchmark/input/kodak/kodim01.png testFiles/kodim01.flif testFiles/decoded_kodim01.png

