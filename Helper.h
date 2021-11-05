#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <list>
#include "ch341a.h"
#include "CH341.api/CH341DLL.H"

template<typename T> using MapTypeToName = std::map<T, std::string>;

template<typename T> std::string Type_ToString(const T& t, MapTypeToName<T>& map) {
    std::string instance;
    std::stringstream _s;

    try {
        instance = map.at(t);
    } catch (std::out_of_range& exc) {
        instance = "" + std::string{ exc.what(), exc.what() + strlen(exc.what()) };
    }

    _s << instance << "(" << (int)t << "; 0x" << std::hex << std::uppercase << (int)t << ")";

    return _s.str();
}

enum class IC_VER_t : ULONG{
    DeviceInvalid = 0x00,
    CH341 = 0x10,
    CH341A = IC_VER_CH341A,
    CH341A3 = IC_VER_CH341A3
};

struct EEPROM {
    const char* name;
    uint32_t size;
};

extern MapTypeToName<IC_VER_t> JIC_VER_t_to_name;
inline std::ostream& operator<<(std::ostream& sstr, const IC_VER_t& id){ sstr << Type_ToString(id, JIC_VER_t_to_name); return sstr; }
inline std::string ToString(const IC_VER_t& id) { return Type_ToString(id, JIC_VER_t_to_name); }

std::tuple<ch341_api*, std::string, int32_t> parse_device(std::string eepromname);