#include <vector>
#include <list>
#include "LT3582.h"
#include "prg_error.h"

int32_t LT3582_t::Read(uint8_t* buf, uint32_t DataAddr, uint32_t len) {
    std::vector<uint8_t> Data = {CMDR_t::SWOFF | 0x00 };
    auto pbuf = buf;
    auto size = len;
    auto Addr = DataAddr;

    ch341_api_i2c::Write(Data.data(), (uint32_t)AddrReg_t::CMDR, Data.size());
    while (size) {
        ch341_api_i2c::Read(pbuf, Addr, 1);
        // --
        Addr++;
        pbuf++;
        size -= 1;
    }

    return 0;
}
int32_t LT3582_t::Write(uint8_t* buf, uint32_t DataAddr, uint32_t len) {
    auto pbuf = buf;
    auto size = len;
    auto Addr = DataAddr;

    if (len > 3) Excp("Error: len = %d", len);

    // -- Begin write data
    {
        uint8_t Data = { CMDR_t::CF_FAULT | CMDR_t::SWOFF | 0x00 };
        ch341_api_i2c::Write(&Data, (uint32_t)AddrReg_t::CMDR, 1);
    }

    while (size) {
        ch341_api_i2c::Write(pbuf, Addr, 1);
        // --
        Addr++;
        pbuf++;
        size -= 1;
    }
    std::list<std::vector<uint8_t>> DataScenario = {
        // -- OTR_0
        {
            CMDR_t::SWOFF | CMDR_t::RSEL0 | 0x00,
            CMDR_t::WOTP | CMDR_t::SWOFF | CMDR_t::RSEL0 | 0x00,
            CMDR_t::SWOFF | CMDR_t::RSEL0 | 0x00,
        },
        // -- OTR_1
        {
            CMDR_t::SWOFF | CMDR_t::RSEL1 | 0x00,
            CMDR_t::WOTP | CMDR_t::SWOFF | CMDR_t::RSEL1 | 0x00,
            CMDR_t::SWOFF | CMDR_t::RSEL1 | 0x00,
        },
        // -- OTR_2
        {
            CMDR_t::SWOFF | CMDR_t::RSEL2 | 0x00,
            CMDR_t::WOTP | CMDR_t::SWOFF | CMDR_t::RSEL2 | 0x00,
            CMDR_t::SWOFF | CMDR_t::RSEL2 | 0x00,
        },
    };
    uint8_t cmdr;
    for (auto& Data : DataScenario) {
        for (auto& dt : Data) {
            ch341_api_i2c::Write(&dt, (uint32_t)AddrReg_t::CMDR, 1);
        }
        ch341_api_i2c::Read(&cmdr, (uint32_t)AddrReg_t::CMDR, 1);
        if (0 != cmdr & CMDR_t::CF_FAULT) Excp("Error: CF_FAULT is sett, no write");
    }
    // -- Check valid write
    std::vector<uint8_t> buff_write(buf, buf + len);
    std::vector<uint8_t> buff_read(len);
    Read(buff_read.data(), DataAddr, len);
    // --
    if (buff_write != buff_read) Excp(" write != read");
    // -- End
    {
        uint8_t Data = 0x00;
        ch341_api_i2c::Write(&Data, (uint32_t)AddrReg_t::CMDR, 1);
    }

    return 0;
}