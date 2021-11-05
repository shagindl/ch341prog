#pragma once

#include <Windows.h>
#include <stdint.h>
#include "ch341a.h"

class LT3582_t : public ch341_api_i2c {
public:
    enum class REG2_t : uint8_t {
        PUSEQ_NoRamp = 0x00,
        PUSEQ_VoutN_Ramp = 0x01,
        PUSEQ_VoutP_Ramp = 0x02,
        PUSEQ_BothRamp = 0x03,

        PDDIS = 0x04,

        /* @TODO: IRMP... */
        /* @TODO: Vplus... */
        LOCK = 0x40
    };
    enum class CMDR_t : uint8_t {
        RSEL0 = 0x01,
        RSEL1 = 0x02,
        RSEL2 = 0x04,
        /*rsrv = 0x08*/
        SWOFF = 0x10,
        RST = 0x20,
        CF_FAULT = 0x40,
        WOTP = 0x80
    };
    enum class AddrReg_t {
        REG_OTP0 = 0x00, REG_OTP1 = 0x01, REG_OTP2 = 0x02,
        CMDR = 0x04
    };
public:
    int32_t Read(uint8_t* buf, uint32_t DataAddr, uint32_t len) final;
    int32_t Write(uint8_t* buf, uint32_t DataAddr, uint32_t len) final;
};

inline LT3582_t::CMDR_t operator|(const LT3582_t::CMDR_t& l, const LT3582_t::CMDR_t& r) {
    return (LT3582_t::CMDR_t)((uint8_t)l | (uint8_t)r);
}
inline uint8_t operator|(const LT3582_t::CMDR_t& l, int r) {
    return (uint8_t)((uint8_t)l | (uint8_t)r);
}
inline uint8_t operator&(const uint8_t& l, const LT3582_t::CMDR_t& r) {
    return (uint8_t)(l & (uint8_t)r);
}
