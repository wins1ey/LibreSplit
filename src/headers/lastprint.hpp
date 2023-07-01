#ifndef LASTPRINT_HPP
#define LASTPRINT_HPP

#include <string>

#include <lua.hpp>

using std::string;

void lastPrint(string message);
int luaPrint(lua_State* L);

#endif