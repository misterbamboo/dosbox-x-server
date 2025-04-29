#include <string>
#include <server\cmds\CaptureCmd.cpp>
#include <server\cmds\MemoryReadCmd.cpp>
#include <server\cmds\MouseCmd.cpp>

static std::string executeServerCmd(std::string& cmd) {
    if(cmd.empty()) {
        return "";
    }

    if(CaptureCmd::canHandle(cmd)) {
        return CaptureCmd::handle(cmd);
    }
    else if(MemoryReadCmd::canHandle(cmd)) {
        return MemoryReadCmd::handle(cmd);
    }
    else if(MouseCmd::canHandle(cmd)) {
        return MouseCmd::handle(cmd);
    }

    return "";
}
