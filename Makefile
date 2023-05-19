CFLAGS = -std=c++17 -O2

JSRLinuxAutosplitter: *.cpp *.hpp
	g++ $(CFLAGS) -o JSRLinuxAutosplitter *.cpp *.hpp