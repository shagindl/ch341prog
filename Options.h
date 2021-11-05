#pragma once

#include <map>
#include <string>
#include "Helper.h"

struct Options_t {
    static Options_t* inst;

    int verbose;
    int device_size;
    std::string device_name = "";
    std::string filename = "";
    int offset = 0, length = 0;
    uint32_t speed = CH341A_STM_I2C_100K;
    ch341_api* api;
    char operation = 0;

    Options_t(int ___argc, char* const* ___argv);
};
