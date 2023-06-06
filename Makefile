CFLAGS = -std=c++17 -O2 -pthread

JSRLinuxAutosplitter: *.cpp *.hpp
	mkdir build
	g++ $(CFLAGS) -o build/AmidEvilLinuxAutosplitter *.cpp *.hpp