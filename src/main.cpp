#include <cstring>
#include <thread>
#include <unistd.h>

#include "headers/downloader.hpp"
#include "headers/autosplitter.hpp"
extern "C" {
    #include "headers/last-gtk.h"
}

void launchArgs(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-downloader") == 0)
        {
            startDownloader();
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    checkDirectories();
    launchArgs(argc, argv);

    std::thread t1(openAutoSplitter);
    std::thread t2(open_timer, argc, argv);

    t1.join();
    t2.join();
    
    return 0;
}