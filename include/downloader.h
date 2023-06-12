#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <curl/curl.h>
#include <lasprint.h>

using std::cout;
using std::endl;
using std::cin;
using std::vector;
using std::string;
using std::ifstream;
using std::istringstream;

void startDownloader(string autoSplittersDirectory);
void downloadFile(string url);

#endif