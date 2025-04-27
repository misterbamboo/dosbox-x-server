#include <string>
#include <thread>
#include <chrono>
#include "hardware.h"

class CaptureCmd {
public:
    static bool canHandle(std::string cmd) {
        return cmd == "capture";
    }

    static std::string handle(std::string cmd) {
        initServerScreenCapture();
        while(ServerCaptureInProcess)
        {
            // wait for the capture to finish
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return "ok";
    }
};
