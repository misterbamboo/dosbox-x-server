#include <string>
#include <thread>
#include <chrono>
#include "mem.h"

class MemoryReadCmd {
public:
    static constexpr const char* CMD_START = "mem_read";

    static constexpr const char* READ_GENERAL = "mem_readX,";
    static constexpr const char* READ_BYTE = "mem_readb,";
    static constexpr const char* READ_WORD = "mem_readw,";
    static constexpr const char* READ_DWORD = "mem_readd,";

    static bool canHandle(std::string cmd) {
        return cmd.rfind(CMD_START, 0) == 0;
    }

    static std::string handle(std::string cmd) {
        std::string addrHexString = cmd.substr(std::string(READ_GENERAL).size());
        int addr = std::stoi(addrHexString, nullptr, 16);

        if(cmd.rfind(READ_BYTE, 0) == 0) {
            return std::to_string(mem_readb(addr));
        }
        else if(cmd.rfind(READ_WORD, 0) == 0) {
            return std::to_string(mem_readw(addr));
        }
        else if(cmd.rfind(READ_DWORD, 0) == 0) {
            return std::to_string(mem_readd(addr));
        }
        else {
            return "0";
        }
    }
};
