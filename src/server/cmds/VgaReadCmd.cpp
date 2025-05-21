#include <string>
#include <thread>
#include <chrono>
#include "vga.h"
#include <server\result\ServerResult.h>
#include <server\cmds\VgaReadCmd.h>

uint8_t VgaReadCmd::image_output[VgaReadCmd::WIDTH * VgaReadCmd::HEIGHT * VgaReadCmd::COLORS] = { 0 };  // Storage allocation

bool VgaReadCmd::canHandle(std::string cmd) {
    if(cmd.rfind(VGA_READ_CMD, 0) == 0) {
        return true;
    }
    return false;
}

void VgaReadCmd::handle(std::string cmd, ServerResult* serverResult) {
    if(cmd.rfind(VGA_READ_CMD, 0) == 0) {
        dumpVgaMem(serverResult);
        return;
    }
}

void VgaReadCmd::dumpVgaMem(ServerResult* serverResult) {
    uint8_t* framebuffer = &vga.mem.linear[0];

    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            uint8_t color_index = framebuffer[y * WIDTH + x] + 0x04000u; // graphics offset
            RGBEntry color = vga.dac.rgb[color_index];

            // Scale 6-bit DAC (0–63) to 8-bit RGB (0–255)
            uint8_t r = color.red * 4;
            uint8_t g = color.green * 4;
            uint8_t b = color.blue * 4;

            int out_offset = (y * WIDTH + x) * 3;
            image_output[out_offset + 0] = static_cast<uint8_t>(r);
            image_output[out_offset + 1] = static_cast<uint8_t>(g);
            image_output[out_offset + 2] = static_cast<uint8_t>(b);
        }
    }

    serverResult->result = reinterpret_cast<char*>(&image_output);
    serverResult->length = WIDTH * HEIGHT * COLORS;
}
