#ifndef LASPRINT_H
#define LASPRINT_H

#include <iostream>
#include <string>
#include <lua.hpp>

using std::string;
using std::cout;
using std::endl;

void lasPrint(string message);
int luaPrint(lua_State* L);

#endif