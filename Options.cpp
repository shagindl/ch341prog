#include <stdint.h>
#include <getopt.h>
#include "Options.h"


extern "C" {
    const char usage[] =
        "\nUsage:\n"\
        " -h, --help             display this message\n"\
        " -i, --info             read the chip ID info\n"\
        " -e, --erase            erase the entire chip\n"\
        " -s, --size             size of EEPROM {24c256|LT3582}\n" \
        " -v, --verbose          print verbose info\n"\
        " -l, --length <bytes>   manually set length\n"\
        " -w, --write <filename> write chip with data from filename\n"\
        " -o, --offset <bytes>   write data starting from specific offset\n"\
        " -r, --read <filename>  read chip and save data to filename\n"\
        " -t, --turbo            increase the i2c bus speed (-tt to use much faster speed)\n"\
        " -d, --double           double the spi bus speed\n";
    const struct option options[] = {
        {"help",    no_argument,        0, 'h'},
        {"info",    no_argument,        0, 'i'},
        {"size",    required_argument,  0, 's'},
        {"erase",   no_argument,        0, 'e'},
        {"write",   required_argument,  0, 'w'},
        {"length",  required_argument,  0, 'l'},
        {"verbose", no_argument,        0, 'v'},
        {"write",   required_argument,  0, 'w'},
        {"offset",  required_argument,  0, 'o'},
        {"read",    required_argument,  0, 'r'},
        {"turbo",   no_argument,        0, 't'},
        {"double",  no_argument,        0, 'd'},
        {0, 0, 0, 0}
    };
};

Options_t *Options_t::inst = nullptr;

Options_t::Options_t(int ___argc, char* const* ___argv) {
    int32_t optidx = 0;
    int8_t c;

    while ((c = getopt_long(___argc, ___argv, "hies:w:r:l:tdvo:", options, &optidx)) != -1) {
        switch (c) {
        case 'i':
        case 'e':
            if (!operation)
                operation = c;
            else
                operation = 'x';
            break;
        case 'v':
            verbose = 1;
            break;
        case 's':
            std::tie(api, device_name, device_size) = parse_device(optarg);
            length = device_size;
            break;
        case 'w':
        case 'r':
            if (!operation) {
                operation = c;
                filename = { optarg };
            } else
                operation = 'x';
            break;
        case 'l':
            length = atoi(optarg);
            break;
        case 't':
            if ((speed & 3) < 3) {
                speed++;
            }
            break;
        case 'd':
            speed |= CH341A_STM_SPI_DBL;
            break;
        case 'o':
            offset = atoi(optarg);
            break;
        default:
            printf("%s\n", usage);
            throw std::runtime_error("");
        }
    }
    if (operation == 0) {
        fprintf(stderr, "%s\n", usage);
        throw std::runtime_error("operation == 0");
    }
    if (operation == 'x') {
        fprintf(stderr, "Conflicting options, only one option at a time.\n");
        throw std::runtime_error("operation == x");
    }
    inst = this;
}
void v_print(int mode, int len) { // mode: begin=0, progress = 1
    if (!Options_t::inst->verbose) return;

    static unsigned int size = 0;
    static time_t started, reported;
    unsigned int dur, done;
    time_t now;
    time(&now);

    switch (mode) {
    case 0: // setup
        size = len;
        started = reported = now;
        break;
    case 1: // progress
        if (now == started) return;

        dur = now - started;
        done = size - len;
        if (done > 0 && reported != now) {
            printf("Bytes: %d (%d%c),  Time: %d, ETA: %d   \r", done,
                (done * 100) / size, '%', dur, (int)((1.0 * dur * size) / done - dur));
            fflush(stdout);
            reported = now;
        }
        break;
    case 2: // done
        dur = now - started; if (dur < 1) dur = 1;
        printf("Total:  %d sec,  average speed  %d  bytes per second.\n", dur, size / dur);
        break;
    default:
        break;
    }
}