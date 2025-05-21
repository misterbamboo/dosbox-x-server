#include <string>
#include <server\cmds\CaptureCmd.cpp>
#include <server\cmds\MemoryReadCmd.cpp>
#include <server\cmds\MouseCmd.cpp>
#include <server\cmds\SaveStateCmd.cpp>
#include <CmdCallback.cpp>
#include <server\cmds\VgaReadCmd.h>
#include <server\result\ServerResult.h>

static void executeServerCmd(std::string cmd, CmdCallback callback, ServerResult* serverResult) {
    serverResult->length = 0;
    if(cmd.empty()) {
        return;
    }

    if(CaptureCmd::canHandle(cmd)) {
        CaptureCmd::handle(cmd, serverResult);
        callback(serverResult);
    }
    else if(MemoryReadCmd::canHandle(cmd)) {
        MemoryReadCmd::handle(cmd, serverResult);
        callback(serverResult);
    }
    else if(MouseCmd::canHandle(cmd)) {
        MouseCmd::handle(cmd, serverResult, callback);
    }
    else if(SaveStateCmd::canHandle(cmd)) {
        SaveStateCmd::handle(cmd,serverResult);
        callback(serverResult);
    }
    else if(VgaReadCmd::canHandle(cmd)) {
        VgaReadCmd::handle(cmd, serverResult);
        callback(serverResult);
    }
}
