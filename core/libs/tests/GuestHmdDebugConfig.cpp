#include "prx/libSceHmd/Hmd.hpp"
#include "prx/libSceHmd/include/Api.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

namespace {

void Require(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "HMD debug configuration: %s\n", message);
        std::abort();
    }
}

void CheckRefusedQueries() {
    std::uint8_t value = 0xa5;
    Require(sceHmdInternalGetDebugMode(&value) == Hmd::NotInitialized, "debug mode before initialize");
    Require(value == 0xa5, "debug mode query before initialize wrote its output");
    value = 0xa5;
    Require(sceHmdInternalGetDebugTextMode(&value) == Hmd::NotInitialized, "debug text mode before initialize");
    Require(value == 0xa5, "debug text mode query before initialize wrote its output");
    Require(sceHmdInternalGetDebugMode(nullptr) == Hmd::ParameterNull, "null debug mode output before initialize");
    Require(sceHmdInternalGetDebugTextMode(nullptr) == Hmd::ParameterNull, "null debug text mode output before initialize");
    errno = 71;
    Require(sceHmdInternalSetDebugMode(0x5a) == Hmd::NotInitialized, "debug mode set before initialize");
    Require(errno == 71, "debug mode set before initialize changed errno");
    errno = 71;
    Require(sceHmdInternalSetDebugTextMode(0x5a) == Hmd::NotInitialized, "debug text mode set before initialize");
    Require(errno == 71, "debug text mode set before initialize changed errno");
}

void CheckDefaults() {
    std::uint8_t mode = 0xa5;
    Require(sceHmdInternalGetDebugMode(&mode) == 0, "debug mode default query");
    Require(mode == 0, "debug mode did not start disabled");
    mode = 0xa5;
    Require(sceHmdInternalGetDebugTextMode(&mode) == 0, "debug text mode default query");
    Require(mode == 0, "debug text mode did not start disabled");
}

void CheckRoundTrip() {
    for (const auto mode : {std::uint8_t{0x00}, std::uint8_t{0x01}, std::uint8_t{0x5a}, std::uint8_t{0xff}}) {
        Require(sceHmdInternalSetDebugMode(mode) == 0, "debug mode set");
        std::uint8_t read = static_cast<std::uint8_t>(mode ^ 0xa5);
        Require(sceHmdInternalGetDebugMode(&read) == 0, "debug mode query");
        Require(read == mode, "debug mode did not keep the value the caller set");
        read = static_cast<std::uint8_t>(mode ^ 0xa5);
        Require(sceHmdInternalSetDebugTextMode(mode) == 0, "debug text mode set");
        Require(sceHmdInternalGetDebugTextMode(&read) == 0, "debug text mode query");
        Require(read == mode, "debug text mode did not keep the value the caller set");
        Require(sceHmdInternalSetDebugMode(mode) == 0, "repeated debug mode set");
        read = static_cast<std::uint8_t>(mode ^ 0xa5);
        Require(sceHmdInternalGetDebugMode(&read) == 0, "debug mode query after a repeated set");
        Require(read == mode, "repeated debug mode set changed the value");
    }
}

void CheckIndependentValues() {
    Require(sceHmdInternalSetDebugMode(0x5a) == 0, "debug mode set");
    Require(sceHmdInternalSetDebugTextMode(0x11) == 0, "debug text mode set");
    std::uint8_t mode = 0;
    std::uint8_t textMode = 0;
    Require(sceHmdInternalGetDebugMode(&mode) == 0, "debug mode query");
    Require(sceHmdInternalGetDebugTextMode(&textMode) == 0, "debug text mode query");
    Require(mode == 0x5a, "debug text mode set changed the debug mode");
    Require(textMode == 0x11, "debug mode set changed the debug text mode");
}

void CheckGuardedOutput() {
    std::uint8_t value = 0xa5;
    errno = 71;
    Require(sceHmdInternalGetDebugMode(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x10))) == Hmd::ParameterNull,
            "unwritable debug mode output");
    Require(errno == 71, "unwritable debug mode output changed errno");
    errno = 71;
    Require(sceHmdInternalGetDebugTextMode(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x10))) == Hmd::ParameterNull,
            "unwritable debug text mode output");
    Require(errno == 71, "unwritable debug text mode output changed errno");
    Require(sceHmdInternalGetDebugMode(nullptr) == Hmd::ParameterNull, "null debug mode output");
    Require(sceHmdInternalGetDebugTextMode(nullptr) == Hmd::ParameterNull, "null debug text mode output");
    Require(sceHmdInternalGetDebugMode(&value) == 0, "debug mode query after refused queries");
    Require(value == 0x5a, "a refused query changed the debug mode");
}

}

int main() {
    Hmd::InitializeParam param{};

    CheckRefusedQueries();
    Require(sceHmdInitialize(&param) == 0, "initialize");
    CheckDefaults();
    CheckRoundTrip();
    CheckIndependentValues();
    CheckGuardedOutput();
    Require(sceHmdTerminate() == 0, "terminate");
    std::uint8_t value = 0xa5;
    Require(sceHmdInternalGetDebugMode(&value) == Hmd::NotInitialized, "debug mode after terminate");
    Require(value == 0xa5, "debug mode query after terminate wrote its output");
    Require(sceHmdInternalSetDebugTextMode(0) == Hmd::NotInitialized, "debug text mode set after terminate");
    Require(sceHmdInitialize(&param) == 0, "reinitialize");
    CheckDefaults();
    Require(sceHmdTerminate() == 0, "second terminate");
}
