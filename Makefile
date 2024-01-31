CFLAGS = -O3
LDFLAGS =
appname = test

all: $(appname)
clean:
	rm -f $(appname) *.o
.PHONY: all clean

sdl_cflags := $(shell pkg-config --cflags sdl2)
sdl_libs := $(shell pkg-config --libs sdl2)
override CFLAGS += $(sdl_cflags)
override LIBS += $(sdl_libs)

$(appname):
	$(CC) $(LDFLAGS) -o test.o $^ $(LIBS)
