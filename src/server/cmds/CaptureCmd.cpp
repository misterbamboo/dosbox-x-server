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
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait for the capture to finish
        }
        return "ok";
    }
};
