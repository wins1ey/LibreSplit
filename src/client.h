#ifndef CLIENT_H
#define CLIENT_H

#include <string>

using std::string;

void connectToServer(string ipAddress);
void sendLiveSplitCommand(const char* command);

#endif