#include <cstdint>

#pragma once
struct ServerResult {
    uint16_t length; // 2 bytes
    char* result; // pointer to the result data
    // The total size of the struct is 1024 bytes

    char trueValue = 0x01;
    char falseValue = 0x00;

    char value[4];

    void setTrue() {
        length = 1;
        result = &trueValue;
    }

    void setFalse() {
        length = 1;
        result = &falseValue;
    }

    void setByte(uint8_t val) {
        length = 1;
        // map val to 4 bytes value array
        value[0] = val;
        result = &value[0];
    }

    void setWord(uint16_t val) {
        length = 2;
        value[0] = (val >> 8) & 0xFF; // high byte
        value[1] = val & 0xFF; // low byte
        result = &value[0];
    }

    void setDoubleWord(uint32_t val) {
        length = 4;
        value[0] = (val >> 24) & 0xFF; // byte 3
        value[1] = (val >> 16) & 0xFF; // byte 2
        value[2] = (val >> 8) & 0xFF; // byte 1
        value[3] = val & 0xFF; // byte 0
        result = &value[0];
    }
};
