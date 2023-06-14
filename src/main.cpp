#include <cstring>

#include "client.h"
#include "downloader.h"
#include "autosplitter.h"

int main(int argc, char *argv[])
{
    checkDirectories();

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-downloader") == 0)
        {
            startDownloader(autoSplittersDirectory);
        }
    }

    chooseAutoSplitter();
    setIpAddress();
    runAutoSplitter();

    return 0;
}