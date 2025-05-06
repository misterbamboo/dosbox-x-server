#include <string>
#include <thread>
#include <chrono>
#include "hardware.h"
#include <server\result\ServerResult.h>

class CaptureCmd {
public:
    static bool canHandle(std::string cmd) {
        return cmd == "capture";
    }

    static void handle(std::string cmd, ServerResult* serverResult) {
        initServerScreenCapture();
        while(ServerCaptureInProcess)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait for the capture to finish
        }
        serverResult->setTrue();
    }
};
