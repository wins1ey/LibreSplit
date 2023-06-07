CFLAGS = -std=c++17 -O2 -pthread

AmidEvilLinuxAutosplitter: *.cpp *.hpp
	mkdir -p build
	g++ $(CFLAGS) -o build/AmidEvilLinuxAutosplitter *.cpp *.hpp
