#ifndef LASPRINT_H
#define LASPRINT_H

#include <string>

#include <lua.hpp>

using std::string;

void lasPrint(string message);
int luaPrint(lua_State* L);

#endif