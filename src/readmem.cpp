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
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::array;
using std::stringstream;

string processName;
string newProcessName;
uintptr_t memoryOffset = 0;

int pid = 0;

void setMemoryOffset()
{
    string command = "cat /proc/" + to_string(pid) + "/maps | grep " + newProcessName;
    array<char, 128> buffer;
    string result;

    // Open the command for reading
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cout << "Error executing command: " << command << std::endl;
    }

    // Read the command output line by line
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    // Close the pipe
    pclose(pipe);

    size_t dashPos = result.find_first_of("-");

    if (dashPos != string::npos)
    {
        string firstNumber = result.substr(0, dashPos);
        memoryOffset = stoull(firstNumber, nullptr, 16);
    }
}

struct StockPid
{
    pid_t pid;
    char buff[512];
    FILE *pid_pipe;
} stockthepid;

void Func_StockPid(const char *processtarget)
{
    stockthepid.pid_pipe = popen(processtarget, "r");
    if (!fgets(stockthepid.buff, 512, stockthepid.pid_pipe))
    {
        cout << "Error reading process ID: " << strerror(errno) << endl;
    }

    stockthepid.pid = strtoul(stockthepid.buff, nullptr, 10);

    if (stockthepid.pid != 0)
    {
        cout << processName + " is running - PID NUMBER -> " << stockthepid.pid << endl;
        lasPrint("Process: " + processName + "\n");
        lasPrint("PID: " + to_string(stockthepid.pid) + "\n");
        pclose(stockthepid.pid_pipe);
        pid = stockthepid.pid;
    }
    else {
        pclose(stockthepid.pid_pipe);
    }
}

int processID(lua_State* L)
{
    processName = lua_tostring(L, 1);
    newProcessName = processName.substr(0, 15);
    string command = "pidof " + newProcessName;
    const char *cCommand = command.c_str();

    Func_StockPid(cCommand);
    while (pid == 0)
    {
        cout << processName + " isn't running. Retrying...\n";
        sleep_for(milliseconds(2000));
        lasPrint("");
        Func_StockPid(cCommand);
    }
    lasPrint("\n");

    setMemoryOffset();

    return 0;
}

template <typename T>
T readMem(int pid, uintptr_t memAddress)
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
        throw runtime_error("Error reading process memory");
    }
    else if (memNread != memRemote.iov_len)
    {
        throw runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
    }

    return value;  // Return the read value
}

// Template instantiations for different value types, specifying the type as a template parameter.
template int8_t readMem<int8_t>(int pid, uint64_t memAddress);
template uint8_t readMem<uint8_t>(int pid, uint64_t memAddress);
template int16_t readMem<int16_t>(int pid, uint64_t memAddress);
template uint16_t readMem<uint16_t>(int pid, uint64_t memAddress);
template int32_t readMem<int32_t>(int pid, uint64_t memAddress);
template uint32_t readMem<uint32_t>(int pid, uint64_t memAddress);
template int64_t readMem<int64_t>(int pid, uint64_t memAddress);
template uint64_t readMem<uint64_t>(int pid, uint64_t memAddress);
template float readMem<float>(int pid, uint64_t memAddress);
template double readMem<double>(int pid, uint64_t memAddress);
template bool readMem<bool>(int pid, uint64_t memAddress);
template string readMem<string>(int pid, uint64_t memAddress);

int readAddress(lua_State* L)
{
    uintptr_t address = memoryOffset;
    string valueType = lua_tostring(L, 1);
    for (int i = 2; i <= lua_gettop(L); i++)
    {
        address += lua_tointeger(L, i); // Calculate the final memory address by summing the Lua arguments.
    }
    variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, bool, string> value;

    try
    {
        // Use template specialization to call the appropriate readMem() function based on the value type.
        if (valueType == "sbyte")
        {
            value = readMem<int8_t>(pid, address);
            lua_pushinteger(L, get<int8_t>(value));
        }
        else if (valueType == "byte")
        {
            value = readMem<uint8_t>(pid, address);
            lua_pushinteger(L, get<uint8_t>(value));
        }
        else if (valueType == "short")
        {
            value = readMem<int16_t>(pid, address);
            lua_pushinteger(L, get<int16_t>(value));
        }
        else if (valueType == "ushort")
        {
            value = readMem<uint16_t>(pid, address);
            lua_pushinteger(L, get<uint16_t>(value));
        }
        else if (valueType == "int")
        {
            value = readMem<int32_t>(pid, address);
            lua_pushinteger(L, get<int32_t>(value));
        }
        else if (valueType == "uint")
        {
            value = readMem<uint32_t>(pid, address);
            lua_pushinteger(L, get<uint32_t>(value));
        }
        else if (valueType == "long")
        {
            value = readMem<int64_t>(pid, address);
            lua_pushinteger(L, get<int64_t>(value));
        }
        else if (valueType == "ulong")
        {
            value = readMem<uint64_t>(pid, address);
            lua_pushinteger(L, get<uint64_t>(value));
        }
        else if (valueType == "float")
        {
            value = readMem<float>(pid, address);
            lua_pushnumber(L, get<float>(value));
        }
        else if (valueType == "double")
        {
            value = readMem<double>(pid, address);
            lua_pushnumber(L, get<double>(value));
        }
        else if (valueType == "bool")
        {
            value = readMem<bool>(pid, address);
            lua_pushboolean(L, get<bool>(value) ? 1 : 0);
        }
        else if (valueType == "string")
        {
            value = readMem<string>(pid, address);
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

    sleep_for(microseconds(1));

    return 1;
}