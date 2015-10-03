CXXFLAGS := $(shell pkg-config --cflags zlib libpng)
LDFLAGS := $(shell pkg-config --libs zlib libpng)

#COMPILER=g++
COMPILER=em++

EM_SCRIPT_OPTIONS=-s EXPORTED_FUNCTIONS='["_mainy"]' -s TOTAL_MEMORY=50000000 -s USE_LIBPNG=1 

flif.html: maniac/*.h maniac/*.cpp image/*.h image/*.cpp transform/*.h transform/*.cpp flif.cpp flif.h flif_config.h
	${COMPILER} ${EM_SCRIPT_OPTIONS} -std=gnu++11 $(CXXFLAGS) $(LDFLAGS) -DNDEBUG -O3 -g0 -Wall maniac/util.cpp maniac/chance.cpp image/crc32k.cpp image/image.cpp image/image-png.cpp image/image-pnm.cpp image/image-pam.cpp image/color_range.cpp transform/factory.cpp flif.cpp -lpng -o flif.html --preload-file assets

flif.prof: maniac/*.h maniac/*.cpp image/*.h image/*.cpp transform/*.h transform/*.cpp flif.cpp flif.h flif_config.h
	g++ -std=gnu++11 $(CXXFLAGS) $(LDFLAGS) -DNDEBUG -O3 -g0 -pg -Wall maniac/util.cpp maniac/chance.cpp image/crc32k.cpp image/image.cpp image/image-png.cpp image/image-pnm.cpp image/image-pam.cpp image/color_range.cpp transform/factory.cpp flif.cpp -lpng -o flif.prof

flif.dbg: maniac/*.h maniac/*.cpp image/*.h image/*.cpp transform/*.h transform/*.cpp flif.cpp flif.h flif_config.h
	g++ -std=gnu++11 $(CXXFLAGS) $(LDFLAGS) -O0 -ggdb3 -Wall maniac/util.cpp maniac/chance.cpp image/crc32k.cpp image/image.cpp image/image-png.cpp image/image-pnm.cpp image/image-pam.cpp image/color_range.cpp transform/factory.cpp flif.cpp -lpng -o flif.dbg
