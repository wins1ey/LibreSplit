#include <iostream>
#include <string>

#include <lua.hpp>

#include <lasprint.hpp>

using std::string;
using std::cout;
using std::endl;

static string output = "LAS (Linux Auto Splitter)\n";

void lasPrint(string message)
{
    if (message == "clear")
    {
        output = "LAS (Linux Auto Splitter)\n";
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
    lasPrint(message);
    return 0;
}