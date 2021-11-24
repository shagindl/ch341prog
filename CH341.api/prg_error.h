#pragma once

#include <stdexcept>
#include <string>

#define ENABLE_EXCEPTION    1

#if ENABLE_EXCEPTION == 1
    #define Excp(...) throw prg_error_t (std::string(__FILE__), __func__, __LINE__, __VA_ARGS__)
#else /* !ENABLE_EXCEPTION*/
    #define Excp(...)
#endif /* ENABLE_EXCEPTION */

class prg_error_t : public std::exception {
    std::string str;
public:
    explicit prg_error_t(std::string sFile, std::string sfunc, int nLine) {
        this->str = "In File: " + sFile + "; line: " + std::to_string(nLine) + "; func: " + sfunc + "\n";
    }
    explicit prg_error_t(std::string sFile, std::string sfunc, int nLine, const std::string message) {
        this->str = "In File: " + sFile + "; line: " + std::to_string(nLine) + "; func: " + sfunc + "; Message - " + message + "\n";
    }
    explicit prg_error_t(std::string sFile, std::string sfunc, int nLine, const char *AFmt, ...) {
        char Buffer[512];
        va_list args;
        va_start(args, AFmt);
        vsprintf_s(Buffer, sizeof(Buffer), AFmt, args);
        va_end(args);

        this->str = "In File: " + sFile + "; line: " + std::to_string(nLine) + "; func: " + sfunc + ".\n" + Buffer + "\n";
    }

    char const* what() const override {
        return str.c_str();
    }
};
