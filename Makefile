CFLAGS = -std=c++17 -O2 -pthread -I/usr/include/lua5.* -I/usr/include/curl -I./include
LDFLAGS = -llua -lcurl

LinuxAutoSplitter: ./src/*.cpp
	mkdir -p build
	g++ $(CFLAGS) $(LDFLAGS) -o build/LAS ./src/*.cpp