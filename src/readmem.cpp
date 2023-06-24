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
#include <signal.h>

#include "headers/readmem.hpp"
#include "headers/lasprint.hpp"
#include "headers/autosplitter.hpp"

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
bool memoryError;

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
    string pidCommand = string(processtarget); // Command to extract the process ID
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
    string command = "pgrep " + newProcessName;
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

template <typename ValueType>
ValueType readMemory(uint64_t memAddress)
{
    ValueType value;  // Variable to store the read value

    struct iovec memLocal;
    struct iovec memRemote;

    memLocal.iov_base = &value;  // Use the value variable
    memLocal.iov_len = sizeof(value);
    memRemote.iov_len = sizeof(value);
    memRemote.iov_base = reinterpret_cast<void*>(memAddress);

    ssize_t memNread = process_vm_readv(pid, &memLocal, 1, &memRemote, 1, 0);
    if (memNread == -1 && !kill(pid, 0))
    {
        memoryError = true;
    }
    else if (memNread == -1 && kill(pid, 0))
    {
        runAutoSplitter();
    }
    else if (memNread != memRemote.iov_len)
    {
        throw runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
    }

    return value;  // Return the read value
}

string readMemory(uint64_t memAddress, int bufferSize)
{
    char buffer[bufferSize]; // Buffer to store the read string

    struct iovec memLocal;
    struct iovec memRemote;

    memLocal.iov_base = &buffer;  // Use the buffer to store the string
    memLocal.iov_len = bufferSize;
    memRemote.iov_len = bufferSize;
    memRemote.iov_base = reinterpret_cast<void*>(memAddress);

    ssize_t memNread = process_vm_readv(pid, &memLocal, 1, &memRemote, 1, 0);
    if (memNread == -1 && !kill(pid, 0))
    {
        buffer[0] = '\0';
    }
    else if (memNread == -1 && kill(pid, 0))
    {
        runAutoSplitter();
    }
    else if (memNread != memRemote.iov_len)
    {
        throw runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
    }

    return string(buffer);  // Return the read string
}

// Template instantiations for different value types, specifying the type as a template parameter.
template int8_t readMemory<int8_t>(uint64_t memAddress);
template uint8_t readMemory<uint8_t>(uint64_t memAddress);
template short readMemory<short>(uint64_t memAddress);
template ushort readMemory<ushort>(uint64_t memAddress);
template int readMemory<int>(uint64_t memAddress);
template uint readMemory<uint>(uint64_t memAddress);
template long readMemory<long>(uint64_t memAddress);
template ulong readMemory<ulong>(uint64_t memAddress);
template float readMemory<float>(uint64_t memAddress);
template double readMemory<double>(uint64_t memAddress);
template bool readMemory<bool>(uint64_t memAddress);

int readAddress(lua_State* L)
{
    sleep_for(milliseconds(1));
    memoryError = false;
    variant<int8_t, uint8_t, short, ushort, int, uint, int64_t, uint64_t, float, double, bool, string> value;

    uint64_t address;
    string valueType;
    int i;

    if (lua_isnumber(L, 2))
    {
        valueType = lua_tostring(L, 1);
        address = memoryOffset + lua_tointeger(L, 2);
        i = 3;
    }
    else
    {
        valueType = lua_tostring(L, 2);
        address = memoryOffset + lua_tointeger(L, 3);
        i = 4;
    }

    for (i; i <= lua_gettop(L); i++)
    {
        if (address <= UINT32_MAX)
        {
            address = readMemory<uint32_t>(static_cast<uint64_t>(address));
        }
        else
        {
            address = readMemory<uint64_t>(address);
        }
        address = address + lua_tointeger(L, i);
    }

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
        else if (valueType.find("string") != string::npos)
        {
            int bufferSize = stoi(valueType.substr(6));
            value = readMemory(address, bufferSize);
            lua_pushstring(L, get<string>(value).c_str());
            return 1;
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

    if (memoryError)
    {
        lua_pushinteger(L, -1);
    }

    return 1;
}