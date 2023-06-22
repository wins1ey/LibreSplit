#include <cstring>

#include "include/client.hpp"
#include "include/downloader.hpp"
#include "include/autosplitter.hpp"

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