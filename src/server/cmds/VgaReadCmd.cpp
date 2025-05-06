#include <string>
#include <thread>
#include <chrono>
#include "vga.h"
#include <server\result\ServerResult.h>

class VgaReadCmd {
public:
    static constexpr const char* VGA_READ_CMD = "vga_read,";

    static uint8_t image_output[320 * 200 * 3]; // RGB output

    static bool canHandle(std::string cmd) {
        if(cmd.rfind(VGA_READ_CMD, 0) == 0) {
            return true;
        }
        return false;
    }

    static void handle(std::string cmd, ServerResult* serverResult, int execCount) {
        if(cmd.rfind(VGA_READ_CMD, 0) == 0) {
            handlerVgaReadCmd(cmd, serverResult, execCount);
            return;
        }
    }

    static void handlerVgaReadCmd(std::string cmd, ServerResult* serverResult, int execCount) {
        std::string parameter = cmd.substr(std::string(VGA_READ_CMD).size());

        int startIndex = execCount * 1022;

        // copy bytes from linear_base to result
        Bitu size = vga.draw.address_add;
        uint8_t* linear_base = vga.draw.linear_base;
        int index = 0;
        int resultSize = 0;
        for(int i = 0; i < 1022; i++) {
            index = startIndex + i;
            serverResult->result[i] = linear_base[index];
            if(index >= size) {
                resultSize = i;
                break;
            }
        }

        serverResult->length = resultSize;
    }

    static void dumpVga() {
        uint8_t* framebuffer = &vga.mem.linear[0];
        
        for(int y = 0; y < 200; ++y) {
            for(int x = 0; x < 320; ++x) {
                uint8_t color_index = framebuffer[y * 320 + x];
                RGBEntry color = vga.dac.rgb[color_index];

                // Scale 6-bit DAC (0–63) to 8-bit RGB (0–255)
                uint8_t r = color.red * 4;
                uint8_t g = color.green * 4;
                uint8_t b = color.blue * 4;

                int out_offset = (y * 320 + x) * 3;
                image_output[out_offset + 0] = r;
                image_output[out_offset + 1] = g;
                image_output[out_offset + 2] = b;
            }
        }
    }
};
