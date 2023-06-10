CFLAGS = -std=c++17 -O2 -pthread -I /usr/include/lua5.*/ -I /usr/include/curl
LDFLAGS = -llua -lcurl

LinuxAutoSplitter: *.cpp *.hpp
	mkdir -p build
	g++ $(CFLAGS) $(LDFLAGS) -o build/LAS *.cpp *.hpp