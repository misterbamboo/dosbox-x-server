#include <cstdint>

#pragma once
struct ServerResult {
    uint16_t length; // 2 bytes
    char result[1022]; // 1022 bytes
    // The total size of the struct is 1024 bytes

    void setTrue() {
        length = 1;
        result[0] = 1;
    }

    void setFalse() {
        length = 1;
        result[0] = 0;
    }

    void setByte(uint8_t val) {
        length = 1;
        result[0] = val;
    }

    void setWord(uint16_t val) {
        length = 2;
        result[0] = (val >> 8) & 0xFF; // high byte
        result[1] = val & 0xFF; // low byte
    }

    void setDoubleWord(uint32_t val) {
        length = 4;
        result[0] = (val >> 24) & 0xFF; // byte 3
        result[1] = (val >> 16) & 0xFF; // byte 2
        result[2] = (val >> 8) & 0xFF; // byte 1
        result[3] = val & 0xFF; // byte 0
    }
};
