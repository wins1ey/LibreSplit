#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/uio.h>
#include <string>
#include <thread>
#include <variant>

#include "readmem.h"
#include "lasprint.h"
#include "autosplitter.h"

using std::string;
using std::cout;
using std::endl;
using std::runtime_error;
using std::to_string;
using std::get;
using std::variant;
using std::cerr;
using std::exception;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::array;
using std::stringstream;

string processName;
string newProcessName;
uintptr_t memoryOffset = 0;
const char *cCommand;

pid_t pid;

void executeCommand(const string& command, array<char, 128>& buffer, string& output)
{
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        throw runtime_error("Error executing command: " + command);
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        output += buffer.data();
    }

    pclose(pipe);
}

/**
 * @brief Find the memory offset of the process.
 * 
 * @return uintptr_t 
 */
uintptr_t findMemoryOffset()
{
    string mapsCommand = "cat /proc/" + to_string(pid) + "/maps | grep " + newProcessName;
    array<char, 128> buffer;
    string mapsOutput;

    // Execute the command and read the output
    executeCommand(mapsCommand, buffer, mapsOutput);

    size_t dashPos = mapsOutput.find_first_of("-");

    if (dashPos != string::npos)
    {
        string firstNumber = mapsOutput.substr(0, dashPos);
        return stoull(firstNumber, nullptr, 16);
    }
    else
    {
        throw runtime_error("Couldn't find memory offset");
    }
}

/**
 * Retrieves the process ID of the specified process target.
 * @param processTarget The target process to retrieve the ID for.
 * @return The process ID if found, 0 otherwise.
 */
void stockProcessID(const char* processtarget)
{
    string pidCommand = string(processtarget) + " | awk '{print $1}'"; // Command to extract the process ID
    array<char, 128> buffer;
    string pidOutput;

    // Execute the command and read the output
    executeCommand(pidCommand, buffer, pidOutput);

    pid = strtoul(pidOutput.c_str(), nullptr, 10);

    if (pid != 0)
    {
        cout << processName + " is running - PID NUMBER -> " << pid << endl;
        lasPrint("Process: " + processName + "\n");
        lasPrint("PID: " + to_string(pid) + "\n");
    }
    else
    {
        cout << "Error reading process ID: " << strerror(errno) << endl;
    }
}

int findProcessID(lua_State* L)
{
    processName = lua_tostring(L, 1);
    newProcessName = processName.substr(0, 15);
    string command = "pidof " + newProcessName;
    cCommand = command.c_str();

    stockProcessID(cCommand);
    while (pid == 0)
    {
        lasPrint("");
        cout << processName + " isn't running. Retrying...\n";
        sleep_for(milliseconds(1));
        stockProcessID(cCommand);
    }
    lasPrint("\n");

    memoryOffset = findMemoryOffset();

    return 0;
}

template <typename T>
T readMemory(uintptr_t memAddress)
{
    T value;  // Variable to store the read value

    struct iovec memLocal;
    struct iovec memRemote;

    memLocal.iov_base = &value;  // Use the value variable
    memLocal.iov_len = sizeof(value);
    memRemote.iov_len = sizeof(value);
    memRemote.iov_base = reinterpret_cast<void*>(memAddress);

    ssize_t memNread = process_vm_readv(pid, &memLocal, 1, &memRemote, 1, 0);
    if (memNread == -1)
    {
        throw runtime_error("Error reading process memory: " + to_string(errno));
    }
    else if (memNread != memRemote.iov_len)
    {
        throw runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
    }

    return value;  // Return the read value
}

// Template instantiations for different value types, specifying the type as a template parameter.
template int8_t readMemory<int8_t>(uint64_t memAddress);
template uint8_t readMemory<uint8_t>(uint64_t memAddress);
template short readMemory<short>(uint64_t memAddress);
template ushort readMemory<ushort>(uint64_t memAddress);
template int readMemory<int>(uint64_t memAddress);
template uint readMemory<uint>(uint64_t memAddress);
template int64_t readMemory<int64_t>(uint64_t memAddress);
template uint64_t readMemory<uint64_t>(uint64_t memAddress);
template float readMemory<float>(uint64_t memAddress);
template double readMemory<double>(uint64_t memAddress);
template bool readMemory<bool>(uint64_t memAddress);
template string readMemory<string>(uint64_t memAddress);

int readAddress(lua_State* L)
{
    sleep_for(milliseconds(1));
    uintptr_t address = memoryOffset;
    string valueType = lua_tostring(L, 1);
    for (int i = 2; i <= lua_gettop(L); i++)
    {
        address += lua_tointeger(L, i); // Calculate the final memory address by summing the Lua arguments.
    }
    variant<int8_t, uint8_t, short, ushort, int, uint, int64_t, uint64_t, float, double, bool, string> value;

    try
    {
        // Use template specialization to call the appropriate readMemory() function based on the value type.
        if (valueType == "sbyte")
        {
            value = readMemory<int8_t>(address);
            lua_pushinteger(L, get<int8_t>(value));
        }
        else if (valueType == "byte")
        {
            value = readMemory<uint8_t>(address);
            lua_pushinteger(L, get<uint8_t>(value));
        }
        else if (valueType == "short")
        {
            value = readMemory<short>(address);
            lua_pushinteger(L, get<short>(value));
        }
        else if (valueType == "ushort")
        {
            value = readMemory<ushort>(address);
            lua_pushinteger(L, get<ushort>(value));
        }
        else if (valueType == "int")
        {
            value = readMemory<int>(address);
            lua_pushinteger(L, get<int>(value));
        }
        else if (valueType == "uint")
        {
            value = readMemory<uint>(address);
            lua_pushinteger(L, get<uint>(value));
        }
        else if (valueType == "long")
        {
            value = readMemory<long>(address);
            lua_pushinteger(L, get<long>(value));
        }
        else if (valueType == "ulong")
        {
            value = readMemory<ulong>(address);
            lua_pushinteger(L, get<ulong>(value));
        }
        else if (valueType == "float")
        {
            value = readMemory<float>(address);
            lua_pushnumber(L, get<float>(value));
        }
        else if (valueType == "double")
        {
            value = readMemory<double>(address);
            lua_pushnumber(L, get<double>(value));
        }
        else if (valueType == "bool")
        {
            value = readMemory<bool>(address);
            lua_pushboolean(L, get<bool>(value) ? 1 : 0);
        }
        else if (valueType == "string")
        {
            value = readMemory<string>(address);
            lua_pushstring(L, get<string>(value).c_str());
        }
        else
        {
            throw runtime_error("Invalid value type: " + valueType);
        }
    }
    catch (const exception& e)
    {
        cerr << "\033[1;31m" << e.what() << endl << endl;
        throw;
    }

    return 1;
}