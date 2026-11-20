#include "SceTypes.hpp"
#include "prx/libScePad/include/Pad.hpp"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <source_location>

static void Require(bool value, std::source_location loc = std::source_location::current()) {
    if (!value) {
        std::fprintf(stderr, "Require failed at line %u\n", loc.line());
        std::fflush(stderr);
        std::abort();
    }
}

int main() {
    std::int32_t mode = 99;
    Require(scePadSetProfileEditMode(PAD_HANDLE, true, 2) == PAD_ERROR_NOT_INITIALIZED);
    Require(scePadGetProfileEditMode(PAD_HANDLE, &mode) == PAD_ERROR_NOT_INITIALIZED && mode == 99);
    Require(scePadGetProfileEditModeLeonardo(PAD_HANDLE) == PAD_ERROR_NOT_INITIALIZED);
    Require(scePadSetSelectButtonCustomizeMode(PAD_HANDLE, true) == PAD_ERROR_NOT_INITIALIZED);
    Require(scePadGetSelectButtonCustomizeMode(PAD_HANDLE) == PAD_ERROR_NOT_INITIALIZED);

    Require(scePadInit_nid_postfix() == PAD_OK);
    const int first = scePadOpen_nid_postfix(0x10000000, PAD_PORT_TYPE_STANDARD, 0, nullptr);
    const int second = scePadOpen_nid_postfix(0x10000001, PAD_PORT_TYPE_STANDARD, 0, nullptr);
    Require(first > 0);
    Require(second > 0);
    Require(first != second);

    Require(scePadGetProfileEditMode(first, nullptr) == PAD_ERROR_INVALID_ARG);
    Require(scePadSetProfileEditMode(12345, true, 1) == PAD_ERROR_INVALID_HANDLE);
    Require(scePadGetProfileEditMode(12345, &mode) == PAD_ERROR_INVALID_HANDLE);
    Require(scePadGetProfileEditModeLeonardo(12345) == PAD_ERROR_INVALID_HANDLE);
    Require(scePadSetSelectButtonCustomizeMode(12345, true) == PAD_ERROR_INVALID_HANDLE);
    Require(scePadGetSelectButtonCustomizeMode(12345) == PAD_ERROR_INVALID_HANDLE);

    mode = 99;
    Require(scePadGetProfileEditMode(first, &mode) == PAD_OK && mode == 0);
    Require(scePadGetProfileEditModeLeonardo(first) == 0);
    Require(scePadSetProfileEditMode(first, true, 3) == PAD_OK);
    mode = 99;
    Require(scePadGetProfileEditMode(first, &mode) == PAD_OK && mode == 1);
    Require(scePadGetProfileEditModeLeonardo(first) == 1);
    Require(scePadGetProfileEditMode(second, &mode) == PAD_OK && mode == 0);
    Require(scePadSetProfileEditMode(second, true, 7) == PAD_OK);
    Require(scePadGetProfileEditMode(second, &mode) == PAD_OK && mode == 1);
    Require(scePadSetProfileEditMode(first, false, 3) == PAD_OK);
    Require(scePadGetProfileEditMode(first, &mode) == PAD_OK && mode == 0);
    Require(scePadGetProfileEditMode(second, &mode) == PAD_OK && mode == 1);

    errno = 71;
    Require(scePadGetProfileEditMode(first, reinterpret_cast<std::int32_t*>(0x10)) == PAD_ERROR_INVALID_ARG);
    Require(errno == 71);

    Require(scePadGetSelectButtonCustomizeMode(first) == 0);
    Require(scePadGetSelectButtonCustomizeMode(second) == 0);
    Require(scePadSetSelectButtonCustomizeMode(first, true) == PAD_OK);
    Require(scePadGetSelectButtonCustomizeMode(first) == 1);
    Require(scePadGetSelectButtonCustomizeMode(second) == 0);
    Require(scePadSetSelectButtonCustomizeMode(first, false) == PAD_OK);
    Require(scePadGetSelectButtonCustomizeMode(first) == 0);

    const int special = scePadOpen_nid_postfix(0x10000002, PAD_PORT_TYPE_SPECIAL, 0, nullptr);
    Require(special > 0);
    Require(scePadSetProfileEditMode(special, true, 1) == PAD_OK);
    Require(scePadGetProfileEditMode(special, &mode) == PAD_OK && mode == 1);
    Require(scePadSetSelectButtonCustomizeMode(special, true) == PAD_OK);
    Require(scePadGetSelectButtonCustomizeMode(special) == 1);
}
