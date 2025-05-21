#include <string>
#include <server\cmds\CaptureCmd.cpp>
#include <server\cmds\MemoryReadCmd.cpp>
#include <server\cmds\MouseCmd.cpp>
#include <server\cmds\SaveStateCmd.cpp>
#include <CmdCallback.cpp>

static void executeServerCmd(std::string& cmd, CmdCallback callback) {
    if(cmd.empty()) {
        return;
    }

    if(CaptureCmd::canHandle(cmd)) {
        std::string value = CaptureCmd::handle(cmd);
        callback(value);
    }
    else if(MemoryReadCmd::canHandle(cmd)) {
        std::string value = MemoryReadCmd::handle(cmd);
        callback(value);
    }
    else if(MouseCmd::canHandle(cmd)) {
        MouseCmd::handle(cmd, callback);
    }
    else if(SaveStateCmd::canHandle(cmd)) {
        std::string value = SaveStateCmd::handle(cmd);
        callback(value);
    }
}
