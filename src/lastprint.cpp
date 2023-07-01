#include <iostream>
#include <string>

#include <lua.hpp>

#include "headers/lastprint.hpp"

using std::string;
using std::cout;
using std::endl;

static string output = "LAST (Linux Auto Splitting Timer)\n";

void lastPrint(string message)
{
    if (message == "clear")
    {
        output = "LAST (Linux Auto Splitting Timer)\n";
    }
    else
    {
        output += message;
    }
    cout << "\033[2J\033[H"; // Clear the console
    cout << output << endl;
}

int luaPrint(lua_State* L)
{
    string message = lua_tostring(L, 1);
    lastPrint(message);
    return 0;
}