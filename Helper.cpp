#include "Helper.h"
#include "LT3582.h"

MapTypeToName<IC_VER_t> JIC_VER_t_to_name = {
    { IC_VER_t::DeviceInvalid, "DeviceInvalid"}, 
    { IC_VER_t::CH341, "CH341"},
    { IC_VER_t::CH341A, "CH341A"},
    { IC_VER_t::CH341A3, "CH341A3"}
};

// --------------------------------------------------------------------------
// parse_device()
//   passed an EEPROM name (case-sensitive), returns its byte size

ch341_api_i2c api_i2c;
LT3582_t api_LT3582;

struct dev_desc_t {
    std::tuple<ch341_api*, std::string, int32_t> comm;
    UCHAR DevAddr_def;
};
std::map<std::string, dev_desc_t> support_device = {
    //{ "24c01", 128 }, // 1kbit = 128 bits
    //{ "24c02", 256 },
    //{ "24c04", 512 },
    //{ "24c08", 1024 },
    //{ "24c16", 2048 },
    //{ "24c32", 4096 }, // 32kbit = 4kbyte
    //{ "24c64", 8192 },
    //{ "24c128", 16384 },
    //{ "24c256", 32768 },
    //{ "24c512", 65536 },
    //{ "24c1024", 131072},
    { "24c256_8", { {&api_i2c, "24c256_8", 1}, 0x50} },
    { "LT3582", { {&api_LT3582, "LT3582", 3}, 0x45} },  // CA = 1 0x31; CA = 0 0x45
};
std::tuple<ch341_api*, std::string, int32_t> parse_device(std::string eepromname) {
    try {
        auto desc = support_device.at(eepromname);
        std::get<0>(desc.comm)->set_DevAddr(desc.DevAddr_def);
        return desc.comm;
    } catch ([[maybe_unused]] std::out_of_range& exc) {
        throw std::invalid_argument(eepromname);
    }
}