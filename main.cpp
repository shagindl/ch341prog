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
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <synchapi.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "ch341a.h"
#include "Helper.h"
#include "Options.h"

int main(int argc, char* argv[])
{
    int32_t ret;
    uint8_t *buf = NULL;
    FILE *fp;

    Options_t opt{ argc, argv};
    try {
        if (ret = ch341Configure() < 0) return -1;
        if (ret = ch341SetStream(opt.speed)) goto out;

        if (opt.operation == 'i') goto out;
        if (opt.operation == 'e') {
            uint8_t timeout = 0;
            ret = ch341EraseChip();
            if (ret < 0) goto out;
            do {
                Sleep(1);
                ret = ch341ReadStatus();
                if (ret < 0) goto out;
                printf(".");
                fflush(stdout);
                timeout++;
                if (timeout == 100) break;
            } while (ret != 0);
            if (timeout == 100)
                fprintf(stderr, "Chip erase timeout.\n");
            else
                printf("Chip erase done!\n");
        }
        if ((opt.operation == 'r') || (opt.operation == 'w')) {
            buf = (uint8_t*)malloc(opt.length);
            if (!buf) {
                fprintf(stderr, "Malloc failed for read buffer.\n");
                goto out;
            }
        }
        if (opt.operation == 'r') {
            try {
                ret = opt.api->Read(buf, opt.offset, opt.device_size);
            } catch (std::exception& exc) {
                std::cout << exc.what();
                goto out;
            }
            if (ret < 0)
                goto out;
            fp = fopen(opt.filename.c_str(), "wb");
            if (!fp) {
                fprintf(stderr, "Couldn't open file %s for writing.\n", opt.filename);
                goto out;
            }
            fwrite(buf, 1, opt.length, fp);
            if (ferror(fp))
                fprintf(stderr, "Error writing file [%s]\n", opt.filename);
            fclose(fp);
        }
        if (opt.operation == 'w') {
            fp = fopen(opt.filename.c_str(), "rb");
            if (!fp) {
                fprintf(stderr, "Couldn't open file %s for reading.\n", opt.filename);
                goto out;
            }
            ret = fread(buf, 1, opt.device_size, fp);
            if (ferror(fp)) {
                fprintf(stderr, "Error reading file [%s]\n", opt.filename);
                if (fp)
                    fclose(fp);
                goto out;
            }
            opt.device_size = ret;
            fprintf(stderr, "File Size is [%d]\n", ret);
            ret = opt.api->Write(buf, opt.offset, ret);
            if (ret == 0) {
                printf("\nWrite ok! Try to verify... ");
                FILE* test_file;
                char* test_filename;
                test_filename = (char*)malloc(strlen("./test-firmware.bin") + 1);
                strcpy(test_filename, "./test-firmware.bin");

                ret = opt.api->Read(buf, opt.offset, opt.device_size);
                test_file = fopen(test_filename, "w+b");

                if (!test_file) {
                    fprintf(stderr, "Couldn't open file %s for writing.\n", test_filename);
                    goto out;
                }
                fwrite(buf, 1, opt.device_size, test_file);

                if (ferror(test_file))
                    fprintf(stderr, "Error writing file [%s]\n", test_filename);

                fseek(fp, 0, SEEK_SET);
                fseek(test_file, 0, SEEK_SET);

                int ch1, ch2;
                int checked_count = 0;
                ch1 = getc(fp);
                ch2 = getc(test_file);

                while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
                    ch1 = getc(fp);
                    ch2 = getc(test_file);
                    checked_count++;
                }

                if (ch1 == ch2 || (checked_count == opt.device_size))
                    printf("\nWrite completed successfully. \n");
                else
                    printf("\nError while writing. Check your device. May be it need to be erased.\n");

                if (remove(test_filename) == 0)
                    printf("\nAll done. \n");
                else
                    printf("\nTemp file could not be deleted");
            }
            fclose(fp);
        }
    } catch (std::exception& exc) {
        printf("\nException error: %s", exc.what());
    }
out:
    ch341Release();
    return 0;
}

