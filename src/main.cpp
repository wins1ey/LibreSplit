#include <cstring>
#include <thread>

#include "headers/downloader.hpp"
#include "headers/autosplitter.hpp"
extern "C" {
    #include "headers/last-gtk.h"
}

using std::thread;

void launchArgs(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-downloader") == 0)
        {
            startDownloader(autoSplittersDirectory);
        }
    }
}

int autoSplitterThread(int argc, char *argv[])
{
    checkDirectories();
    launchArgs(argc, argv);
    chooseAutoSplitter();
    runAutoSplitter();

    return 0;
}

int main(int argc, char *argv[])
{
    thread t1(autoSplitterThread, argc, argv);
    thread t2(open_timer, argc, argv);

    t1.join();
    t2.join();

    return 0;
}