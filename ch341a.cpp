/*
 * This file is part of the ch341prog project.
 *
 * Copyright (C) 2014 Pluto Yang (yangyj.ee@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * verbose functionality forked from https://github.com/vSlipenchuk/ch341prog/commit/5afb03fe27b54dbcc88f6584417971d045dd8dab
 *
 */

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Helper.h"
#include "ch341a.h"
#include "CH341.api/EXAM.h"
#include "prg_error.h"

//struct signal saold;
int force_stop = 0;
void v_print(int mode, int len) ;

ULONG ch341_sn = 0;
HANDLE	WINAPI device_handle = INVALID_HANDLE_VALUE;

/* Configure CH341A, find the device and set the default interface. */
void CALLBACK CH341_NOTIFY_ROUTINE(ULONG iEventStatus);
int32_t ch341Configure()
{
    if ((device_handle = CH341OpenDevice(ch341_sn)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Couldn't open device CH341.\n");
        return -1;
    }
    fprintf(stderr, "CH341GetVerIC = %s.\n", ToString((IC_VER_t)CH341GetVerIC(0)).c_str());
    if (!CH341SetParaMode(ch341_sn, mCH341_PARA_MODE_EPP)) {
        std::cout << "Error CH341SetParaMode.\n";
        goto close_handle;
    }
    if (!CH341SetDeviceNotify(ch341_sn, NULL, CH341_NOTIFY_ROUTINE)) {
        std::cout << "Error CH341SetDeviceNotify.\n";
        goto close_handle;
    }

    return 0;

close_handle:
    CH341CloseDevice(ch341_sn);

    return -1;
}

/* release libusb structure and ready to exit */
int32_t ch341Release(void)
{
    CH341CloseDevice(ch341_sn);
    device_handle = INVALID_HANDLE_VALUE;

    return 0;
}

/*   set the i2c bus speed (speed(b1b0): 0 = 20kHz; 1 = 100kHz, 2 = 400kHz, 3 = 750kHz)
 *   set the spi bus data width(speed(b2): 0 = Single, 1 = Double)  */
int32_t ch341SetStream(uint32_t speed) {
   return !CH341SetStream(ch341_sn, speed);
}

/* ch341 requres LSB first, swap the bit order before send and after receive  */
uint8_t swapByte(uint8_t c)
{
    static const uint8_t reverse_table[] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
    };
    return reverse_table[c];
}

/* assert or deassert the chip-select pin of the spi device */
void ch341SpiCs(uint8_t *ptr, bool selected)
{
    // @TODO:
}

/* transfer len bytes of data to the spi device */
int32_t ch341SpiStream(uint8_t *out, uint8_t *in, uint32_t len)
{
    uint8_t inBuf[CH341_PACKET_LENGTH], outBuf[CH341_PACKET_LENGTH], *inPtr, *outPtr;
    int32_t ret = 0, packetLen;
    bool done;
#if 0
    if (devHandle == NULL) return -1;

    ch341SpiCs(outBuf, true);
    ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 4);
    if (ret < 0) return -1;

    inPtr = in;

    do {
        done=true;
        packetLen=len+1;    // STREAM COMMAND + data length
        if (packetLen>CH341_PACKET_LENGTH) {
            packetLen=CH341_PACKET_LENGTH;
            done=false;
        }
        outPtr = outBuf;
        *outPtr++ = CH341A_CMD_SPI_STREAM;
        for (int i = 0; i < packetLen-1; ++i)
            *outPtr++ = swapByte(*out++);
        ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, packetLen);
        if (ret < 0) return -1;
        ret = usbTransfer(__func__, BULK_READ_ENDPOINT, inBuf, packetLen-1);
        if (ret < 0) return -1;
        len -= ret;

        for (int i = 0; i < ret; ++i) // swap the buffer
            *inPtr++ = swapByte(inBuf[i]);
    } while (!done);

    ch341SpiCs(outBuf, false);
    ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 3);
    if (ret < 0) return -1;
#endif
    return 0;
}

#define JEDEC_ID_LEN 0x52    // additional byte due to SPI shift
/* read the JEDEC ID of the SPI Flash */
int32_t ch341SpiCapacity(void)
{
    uint8_t out[JEDEC_ID_LEN];
    uint8_t in[JEDEC_ID_LEN], *ptr, cap = 0;
    int32_t ret;
#if 0
    if (devHandle == NULL)
        return -1;

    ptr = out;
    *ptr++ = 0x9F; // Read JEDEC ID

    for (int i = 0; i < JEDEC_ID_LEN - 1; ++i)
        *ptr++ = 0x00;

    ret = ch341SpiStream(out, in, JEDEC_ID_LEN);

    if (ret < 0)
        return ret;

    if (! (in[1] == 0xFF && in[2] == 0xFF && in[3] == 0xFF))
    {
        printf("Manufacturer ID: %02x\n", in[1]);
        printf("Memory Type: %02x%02x\n", in[2], in[3]);

        if (in[0x11] == 'Q' && in[0x12] == 'R' && in[0x13] == 'Y')
        {
            cap = in[0x28];
            printf("Reading device capacity from CFI structure\n");
        }
        else
        {
            cap = in[3];
            printf("No CFI structure found, trying to get capacity from device ID. Set manually if detection fails.\n");
        }

        printf("Capacity: %02x\n", cap);
    }
    else
    {
        printf("Chip not found or missed in ch341a. Check connection\n");
        exit(0);
    }
#endif
    return cap;
}

/* read status register */
int32_t ch341ReadStatus(void)
{
    uint8_t out[2];
    uint8_t in[2] = {0};
    int32_t ret;

    //if (devHandle == NULL) return -1;
    //out[0] = 0x05; // Read status
    //ret = ch341SpiStream(out, in, 2);
    //if (ret < 0) return ret;
    return (in[1]);
}

/* write status register */
int32_t ch341WriteStatus(uint8_t status)
{
    uint8_t out[2];
    uint8_t in[2];
    int32_t ret;
#if 0
    if (devHandle == NULL) return -1;
    out[0] = 0x06; // Write enable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    out[0] = 0x01; // Write status
    out[1] = status;
    ret = ch341SpiStream(out, in, 2);
    if (ret < 0) return ret;
    out[0] = 0x04; // Write disable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
#endif
    return 0;
}

/* chip erase */
int32_t ch341EraseChip(void)
{
    uint8_t out[1];
    uint8_t in[1];
    int32_t ret;
#if 0
    if (devHandle == NULL) return -1;
    out[0] = 0x06; // Write enable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    out[0] = 0xC7; // Chip erase
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    out[0] = 0x04; // Write disable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
#endif
    return 0;
}

/* read the content of SPI device to buf, make sure the buf is big enough before call  */
int32_t ch341_api_spi::Read(uint8_t *buf, uint32_t add, uint32_t len)
{
    uint32_t ret = 0;
    uint8_t out[CH341_MAX_PACKET_LEN];
    uint8_t in[CH341_PACKET_LENGTH];
    bool fourbyte = (add + len) > (1 << 24);
#if 0
    if (devHandle == NULL) return -1;
    /* what subtracted is: 1. first cs package, 2. leading command for every other packages,
     * 3. second package contains read flash command and 3 bytes address */
    const uint32_t max_payload = CH341_MAX_PACKET_LEN - CH341_PACKET_LENGTH
        - CH341_MAX_PACKETS + 1 - 4 - (fourbyte? 1: 0);
    uint32_t pkg_len, pkg_count;
    struct libusb_transfer *xferBulkIn, *xferBulkOut;
    uint32_t idx = 0;
    int32_t old_counter;
    struct timeval tv = {0, 100};
    struct spi_transfer_in bulk_in = {0};

    v_print( 0, len); // verbose

    memset(out, 0xff, CH341_MAX_PACKET_LEN);
    for (int i = 1; i < CH341_MAX_PACKETS; ++i) // fill CH341A_CMD_SPI_STREAM for every packet
        out[i * CH341_PACKET_LENGTH] = CH341A_CMD_SPI_STREAM;
    memset(in, 0x00, CH341_PACKET_LENGTH);
    xferBulkIn  = libusb_alloc_transfer(0);
    xferBulkOut = libusb_alloc_transfer(0);

    printf("Read started!\n");
    while (len > 0) {
        v_print( 1, len); // verbose
        fflush(stdout);
        ch341SpiCs(out, true);
        idx = CH341_PACKET_LENGTH + 1;
        out[idx++] = swapByte(fourbyte? 0x13: 0x03);
        if (fourbyte)
            out[idx++] = swapByte(add >> 24);
        out[idx++] = swapByte(add >> 16);
        out[idx++] = swapByte(add >> 8);
        out[idx++] = swapByte(add);
        bulk_in.skip_bytes = fourbyte? 5: 4;
        if (len > max_payload) {
            pkg_len = CH341_MAX_PACKET_LEN;
            pkg_count = CH341_MAX_PACKETS - 1;
            len -= max_payload;
            add += max_payload;
        } else {
            pkg_count = (len + 4) / (CH341_PACKET_LENGTH - 1);
            if ((len + 4) % (CH341_PACKET_LENGTH - 1)) pkg_count ++;
            pkg_len = (pkg_count) * CH341_PACKET_LENGTH + ((len + 4) % (CH341_PACKET_LENGTH - 1)) + 1;
            len = 0;
        }
        bulk_in.bulk_count = 0;
        bulk_in.read_buffer = buf;
        libusb_fill_bulk_transfer(xferBulkIn, devHandle, BULK_READ_ENDPOINT, in,
                CH341_PACKET_LENGTH, cbBulkIn, &bulk_in, DEFAULT_TIMEOUT);
        buf += max_payload; // advance user's pointer
        libusb_submit_transfer(xferBulkIn);
        libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT, out,
                pkg_len, cbBulkOut, NULL, DEFAULT_TIMEOUT);
        libusb_submit_transfer(xferBulkOut);
        old_counter = bulk_in.bulk_count;
        while (bulk_in.bulk_count < pkg_count) {
            libusb_handle_events_timeout(NULL, &tv);
            if (bulk_in.bulk_count == -1) { // encountered error
                len = 0;
                ret = -1;
                break;
            }
            if (old_counter != bulk_in.bulk_count) { // new package came
                if (bulk_in.bulk_count != pkg_count)
                    libusb_submit_transfer(xferBulkIn);  // resubmit bulk in request
                old_counter = bulk_in.bulk_count;
            }
        }
        ch341SpiCs(out, false);
        ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, out, 3);
        if (ret < 0) break;
        if (force_stop == 1) { // user hit ctrl+C
            force_stop = 0;
            if (len > 0)
                fprintf(stderr, "User hit Ctrl+C, reading unfinished.\n");
            break;
        }
    }
    libusb_free_transfer(xferBulkIn);
    libusb_free_transfer(xferBulkOut);
    v_print(2, 0);
#endif
    return ret;
}

#define WRITE_PAYLOAD_LENGTH 301 // 301 is the length of a page(256)'s data with protocol overhead
/* write buffer(*buf) to SPI flash */
int32_t ch341_api_spi::Write(uint8_t *buf, uint32_t add, uint32_t len)
{
    uint8_t out[WRITE_PAYLOAD_LENGTH];
    uint8_t in[CH341_PACKET_LENGTH];
    uint32_t tmp, pkg_count;
    struct libusb_transfer *xferBulkIn, *xferBulkOut;
    uint32_t idx = 0;
    uint32_t ret = 0;
#if 0
    int32_t old_counter;
    struct timeval tv = {0, 100};
    bool fourbyte = (add + len) > (1 << 24);
    struct spi_transfer_in bulk_in = { .read_buffer = NULL };

    v_print(0, len); // verbose

    if (devHandle == NULL) return -1;
    memset(out, 0xff, WRITE_PAYLOAD_LENGTH);
    xferBulkIn  = libusb_alloc_transfer(0);
    xferBulkOut = libusb_alloc_transfer(0);

    printf("Write started!\n");
    while (len > 0) {
        v_print(1, len);

        out[0] = 0x06; // Write enable
        ret = ch341SpiStream(out, in, 1);
        ch341SpiCs(out, true);
        idx = CH341_PACKET_LENGTH;
        out[idx++] = CH341A_CMD_SPI_STREAM;
        out[idx++] = swapByte(fourbyte? 0x12: 0x02);
        if (fourbyte)
            out[idx++] = swapByte(add >> 24);
        out[idx++] = swapByte(add >> 16);
        out[idx++] = swapByte(add >> 8);
        out[idx++] = swapByte(add);

        tmp = 0;
        pkg_count = 1;

        while ((idx < WRITE_PAYLOAD_LENGTH) && (len > tmp)) {
            if (idx % CH341_PACKET_LENGTH == 0) {
                out[idx++] = CH341A_CMD_SPI_STREAM;
                pkg_count ++;
            } else {
                out[idx++] = swapByte(*buf++);
                tmp++;
                if (((add + tmp) & 0xFF) == 0) // cross page boundary
                    break;
            }
        }
        len -= tmp;
        add += tmp;
        bulk_in.bulk_count = 0;
        libusb_fill_bulk_transfer(xferBulkIn, devHandle, BULK_READ_ENDPOINT, in,
                CH341_PACKET_LENGTH, cbBulkIn, &bulk_in, DEFAULT_TIMEOUT);
        libusb_submit_transfer(xferBulkIn);
        libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT, out,
                idx, cbBulkOut, NULL, DEFAULT_TIMEOUT);
        libusb_submit_transfer(xferBulkOut);
        old_counter = bulk_in.bulk_count;
        ret = 0;
        while (bulk_in.bulk_count < pkg_count) {
            libusb_handle_events_timeout(NULL, &tv);
            if (bulk_in.bulk_count == -1) { // encountered error
                ret = -1;
                break;
            }
            if (old_counter != bulk_in.bulk_count) { // new package came
                if (bulk_in.bulk_count != pkg_count)
                    libusb_submit_transfer(xferBulkIn);  // resubmit bulk in request
                old_counter = bulk_in.bulk_count;
            }
        }
        if (ret < 0) break;
        ch341SpiCs(out, false);
        ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, out, 3);
        if (ret < 0) break;
        out[0] = 0x04; // Write disable
        ret = ch341SpiStream(out, in, 1);
        do {
            ret = ch341ReadStatus();
            if (ret != 0)
                libusb_handle_events_timeout(NULL, &tv);
        } while(ret != 0);
        if (force_stop == 1) { // user hit ctrl+C
            force_stop = 0;
            if (len > 0)
                fprintf(stderr, "User hit Ctrl+C, writing unfinished.\n");
            break;
        }
    }
    libusb_free_transfer(xferBulkIn);
    libusb_free_transfer(xferBulkOut);

    v_print(2, 0);
#endif
    return ret;
}
//-----------------------------------------------------------------------------
// -- ch341_api_i2c
//-----------------------------------------------------------------------------
[[nodiscard]] int32_t ch341_api_i2c::Read(uint8_t* buf, uint32_t DataAddr, uint32_t len) {
    int32_t ret = 0;

    std::list<std::vector<uint8_t>> out_scenario = { 
        {
            (uint8_t)((_DevAddr << 1) | 0x00), // Write
            /*(uint8_t)(DataAddr >> 8),*/ (uint8_t)(DataAddr >> 0),
        },
        {
            (uint8_t)((_DevAddr << 1) | 0x01), // Read
        }
    };
    for (auto& out : out_scenario) {
        if (!IIC_IssueStart(ch341_sn)) Excp();
        for (auto& dt : out) if (!IIC_OutByteCheckAck(ch341_sn, dt)) Excp("dt = 0x%02X", dt);
    }
    for (int i = 0; i < len - 1; i++) {
        if(!IIC_InBlockByAck(ch341_sn, 1, &buf[i]) ) Excp("Error read buf[%d]", i);
    }
    if (len && !IIC_InByteNoAck(ch341_sn, &buf[len - 1]) ) Excp("Error read buf[%d]", len - 1);
    if (!IIC_IssueStop(ch341_sn)) Excp();

    return 0;
}
[[nodiscard]] int32_t ch341_api_i2c::Write(uint8_t* buf, uint32_t DataAddr, uint32_t len) {
    int32_t ret = 0;

    std::vector<uint8_t> out = {
        (uint8_t)((_DevAddr << 1) | 0x00), // Write
        /*(uint8_t)(DataAddr >> 8),*/ (uint8_t)(DataAddr >> 0),
    };
    for (auto i = 0; i < len; i++) {
        if (!IIC_IssueStart(ch341_sn)) Excp();
        for (auto& dt : out) if (!IIC_OutByteCheckAck(ch341_sn, dt)) Excp("dt = 0x%02X", dt);
        if (!IIC_OutByteCheckAck(ch341_sn, buf[i])) Excp("dt = 0x%02X", dt);
        if (!IIC_IssueStop(ch341_sn)) Excp();
    }

    return 0;
}
//-----------------------------------------------------------------------------
// -- Event
//-----------------------------------------------------------------------------
// Device event notification callback program
void CALLBACK CH341_NOTIFY_ROUTINE(ULONG iEventStatus) {
    if (iEventStatus == CH341_DEVICE_ARRIVAL) { // Device insertion event, already inserted
        if (CH341OpenDevice(ch341_sn)) {
            std::cout <<  "CH341EVT **The device is unplugged.\n";
        } else {
            std::cout << "CH341EVT **Device is plugged in.\n";
            CH341SetParaMode(ch341_sn, mCH341_PARA_MODE_EPP);
        }
    } else if (iEventStatus == CH341_DEVICE_REMOVE) { // Device unplug event, unplugged
        std::cout << "CH341EVT **The device is unplugged.\n";
    }
}