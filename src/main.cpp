#include <cstring>
#include <thread>

#include "headers/client.hpp"
#include "headers/downloader.hpp"
#include "headers/autosplitter.hpp"
extern "C" {
    #include "headers/urn-gtk.h"
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
    setIpAddress();
    runAutoSplitter();

    return 0;
}

int timerThread(int argc, char *argv[])
{
    startUrn(argc, argv);

    return 0;
}

int main(int argc, char *argv[])
{
    thread t1(autoSplitterThread, argc, argv);
    thread t2(timerThread, argc, argv);

    t1.join();
    t2.join();

    return 0;
}