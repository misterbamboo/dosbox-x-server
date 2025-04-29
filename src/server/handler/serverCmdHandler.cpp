#include <string>
#include "..\cmds\CaptureCmd.cpp"
#include "..\cmds\MemoryReadCmd.cpp"

static std::string executeServerCmd(std::string& cmd) {
    if(cmd.empty()) {
        return "";
    }

    if(CaptureCmd::canHandle(cmd)) {
        return CaptureCmd::handle(cmd);
    }

    if(MemoryReadCmd::canHandle(cmd)) {
        return MemoryReadCmd::handle(cmd);
    }

    return "";
}
