#pragma once

#include "singleton.hpp"
#ifdef _WIN32
#include <Windows.h>
// whyyyy gdi
#undef RegisterClass
#undef ERROR
#endif
#include <cstdio>
#include <cstdint>

#define LOG(...) logger.Log(__VA_ARGS__)

#define LOG_COLOR Color(255, 0, 0)

#define INFO(...) LOG("[I] " __VA_ARGS__)
#define WARNING(...) LOG("[W] " __VA_ARGS__)
#define ERROR(...) LOG("[E] " __VA_ARGS__)

struct Color {
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) { }
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) { }
    Color() : r(0), g(0), b(0), a(255) { }

    inline uint32_t ToBGR() {
        return (static_cast<uint32_t>(this->b) << 16) | (static_cast<uint32_t>(this->g) << 8) | (static_cast<uint32_t>(this->r));
    }

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class Logger {
public:
    template<typename... Args> void Log(const char* format, Args... args) {
#ifdef DEBUG
        printf(format, args...);
#endif
        if(this->ColorMsg != nullptr) {
            this->ColorMsg(LOG_COLOR, format, args...);
        }
    }

    Logger();
    ~Logger();

    using ColorMsg_t = void(*)(const Color& color, const char* format, ...);
    ColorMsg_t ColorMsg = nullptr;
};

extern Logger logger;