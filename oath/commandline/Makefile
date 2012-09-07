CFLAGS := -g
CXXFLAGS := -std=c++11 -stdlib=libc++ $(shell pkg-config --cflags libusb) -g
LDFLAGS := $(shell pkg-config --libs libusb)

all: oath-cli

main.o: main.cc

opendevice.o: opendevice.c

oath-cli: opendevice.o main.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o oath-cli

.PHONY: all clean
