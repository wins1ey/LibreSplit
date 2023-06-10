CFLAGS = -std=c++17 -O2 -pthread -I /usr/include/lua5.*/ -llua -I /usr/include/curl -lcurl

LinuxAutoSplitter: *.cpp *.hpp
	mkdir -p build
	g++ $(CFLAGS) -o build/LAS *.cpp *.hpp