#ifndef LASPRINT_HPP
#define LASPRINT_HPP

#include <string>

#include <lua.hpp>

using std::string;

void lasPrint(string message);
int luaPrint(lua_State* L);

#endif