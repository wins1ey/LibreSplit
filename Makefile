CFLAGS = -std=c++17 -O2 -pthread -I /usr/include/lua5.3/ -llua5.3

LinuxAutoSplitter: *.cpp *.hpp
	mkdir -p build
	g++ $(CFLAGS) -o build/LinuxAutoSplitter *.cpp *.hpp
