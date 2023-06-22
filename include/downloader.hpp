#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

using std::string;

void startDownloader(string autoSplittersDirectory);
void downloadFile(string url);

#endif