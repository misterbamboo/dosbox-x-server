#include <string>
#include <server\cmds\CaptureCmd.cpp>
#include <server\cmds\MemoryReadCmd.cpp>
#include <server\cmds\MouseCmd.cpp>
#include <server\cmds\SaveStateCmd.cpp>
#include <server\cmds\VgaReadCmd.cpp>
#include <server\result\ServerResult.h>

static void executeServerCmd(std::string cmd, ServerResult* serverResult, int execCount) {
    serverResult->length = 0;
    if(cmd.empty()) {
        return;
    }

    if(CaptureCmd::canHandle(cmd)) {
        CaptureCmd::handle(cmd, serverResult);
        return;
    }
    else if(VgaReadCmd::canHandle(cmd)) {
        VgaReadCmd::handle(cmd, serverResult, execCount);
        return;
    }
    else if(MemoryReadCmd::canHandle(cmd)) {
        MemoryReadCmd::handle(cmd, serverResult);
        return;
    }
    else if(MouseCmd::canHandle(cmd, serverResult)) {
        MouseCmd::handle(cmd, serverResult);
        return;
    }
    else if(SaveStateCmd::canHandle(cmd)) {
        SaveStateCmd::handle(cmd, serverResult);
        return;
    }
}
