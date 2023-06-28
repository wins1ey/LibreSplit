#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include <lua.hpp>

using std::string;

void connectToServer(string ipAddress);
void setIpAddress();
void sendLiveSplitCommand(const char* command);
int sendCommand(lua_State* L);

#endif