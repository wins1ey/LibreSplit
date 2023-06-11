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
        output += message + "\n";
    }
    system("clear");
    cout << output << endl;
}