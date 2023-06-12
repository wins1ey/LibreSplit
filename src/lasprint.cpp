#include <lasprint.h>

string output = "LAS (Linux Auto Splitter)\n";

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
    system("clear");
    cout << output << endl;
}

int luaPrint(lua_State* L)
{
    string message = lua_tostring(L, 1);
    lasPrint(message);
    return 0;
}