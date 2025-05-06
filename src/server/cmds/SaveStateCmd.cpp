#include <string>
#include <thread>
#include <chrono>
#include "dosbox.h"
#include "savestates.h"
#include <server\result\ServerResult.h>

class SaveStateCmd {
public:
    static constexpr const char* LOAD_STATE_CMD = "load_state,";

    static bool canHandle(std::string cmd) {
        if(cmd.rfind(LOAD_STATE_CMD, 0) == 0) {
            return true;
        }
        return false;
    }

    static void handle(std::string cmd, ServerResult* serverResult) {
        if(cmd.rfind(LOAD_STATE_CMD, 0) == 0) {
            handlerLoadCmd(cmd);
            serverResult->setTrue();
            return;
        }

        return serverResult->setFalse();
    }

    static void handlerLoadCmd(std::string cmd) {
        std::string pathToSaveFile = cmd.substr(std::string(LOAD_STATE_CMD).size());
        std::string parentDir = pathToSaveFile.substr(0, pathToSaveFile.find_last_of("\\/"));

        setPendingLoadState(parentDir, pathToSaveFile);
        while(getPendingLoadState())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait for the capture to finish
        }
    }
};
