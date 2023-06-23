#include <cstring>

#include "headers/client.hpp"
#include "headers/downloader.hpp"
#include "headers/autosplitter.hpp"

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

int main(int argc, char *argv[])
{
    checkDirectories();
    launchArgs(argc, argv);
    chooseAutoSplitter();
    setIpAddress();
    runAutoSplitter();

    return 0;
}