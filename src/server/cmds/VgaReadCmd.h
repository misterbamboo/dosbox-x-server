#pragma once
#include <string>
#include <thread>
#include <chrono>
#include "vga.h"
#include <server\result\ServerResult.h>

class VgaReadCmd {
public:
    static constexpr const char* VGA_READ_CMD = "vga_read,";

    static constexpr int WIDTH = 640;
    static constexpr int HEIGHT = 480;
    static constexpr int COLORS = 3; // RGB

    static uint8_t image_output[WIDTH * HEIGHT * COLORS];  // Storage allocation

    static bool canHandle(std::string cmd);
    static void handle(std::string cmd, ServerResult* serverResult);
    static void handlerVgaReadCmd(std::string cmd, ServerResult* serverResult);
    static void dumpVgaMem(ServerResult* serverResult);
};
