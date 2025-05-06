#include <string>
#include <thread>
#include <chrono>
#include "mem.h"
#include <server\result\ServerResult.h>

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

    static void handle(std::string cmd, ServerResult* serverResult) {
        std::string addrHexString = cmd.substr(std::string(READ_GENERAL).size());
        int addr = std::stoi(addrHexString, nullptr, 16);

        if(cmd.rfind(READ_BYTE, 0) == 0) {
            uint8_t val8bits = mem_readb(addr);
            serverResult->setByte(val8bits);
            return;
        }
        else if(cmd.rfind(READ_WORD, 0) == 0) {
            uint16_t val16bits = mem_readw(addr);
            serverResult->setWord(val16bits);
            return;
        }
        else if(cmd.rfind(READ_DWORD, 0) == 0) {
            uint32_t val32bits = mem_readd(addr);
            serverResult->setDoubleWord(val32bits);
            return;
        }
        else {
            serverResult->setFalse();
            return;
        }
    }
};
