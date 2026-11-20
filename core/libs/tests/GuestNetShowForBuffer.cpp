#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include "prx/libSceNet/libSceNet.hpp"

namespace {

constexpr std::uint32_t ErrorBase = 0x80410100u;
constexpr std::uint32_t ENotInit = 200;
constexpr std::uint32_t EInval = 22;
constexpr std::uint32_t EFault = 14;
constexpr std::size_t ShowLimit = 0x2000;
constexpr std::size_t BufferSize = 0x4000;
constexpr char Fill = static_cast<char>(0xa5);

void Require(bool condition) {
    if (!condition) std::abort();
}

void RequireError(int result, std::uint32_t error, int netErrno) {
    Require(result == static_cast<int>(ErrorBase | error));
    Require(*sceNetErrnoLoc() == netErrno);
}

template <typename TCall>
std::pair<std::string, int> CaptureStdout(TCall call) {
    std::fflush(stdout);
    std::FILE* file = std::tmpfile();
    Require(file != nullptr);
#ifdef _WIN32
    const int saved = _dup(_fileno(stdout));
    Require(saved >= 0);
    Require(_dup2(_fileno(file), _fileno(stdout)) == 0);
#else
    const int saved = dup(fileno(stdout));
    Require(saved >= 0);
    Require(dup2(fileno(file), fileno(stdout)) == 0);
#endif
    const int result = call();
    std::fflush(stdout);
#ifdef _WIN32
    Require(_dup2(saved, _fileno(stdout)) == 0);
    _close(saved);
#else
    Require(dup2(saved, fileno(stdout)) == 0);
    close(saved);
#endif
    std::rewind(file);
    std::string text;
    std::array<char, 4096> chunk{};
    for (;;) {
        const std::size_t read = std::fread(chunk.data(), 1, chunk.size(), file);
        text.append(chunk.data(), read);
        if (read != chunk.size()) break;
    }
    std::fclose(file);
    return {text, result};
}

std::vector<char> FilledBuffer() {
    return std::vector<char>(BufferSize, Fill);
}

void RequireUntouched(const std::vector<char>& buffer, std::size_t from) {
    Require(std::all_of(buffer.begin() + static_cast<std::ptrdiff_t>(from), buffer.end(),
        [](char byte) { return byte == Fill; }));
}

std::string PrintedIfconfig() {
    const auto [text, result] = CaptureStdout([] { return sceNetShowIfconfigForBuffer(nullptr, 0, 0); });
    Require(result == 0);
    Require(text.find("lo0 (index=2)\n") != std::string::npos);
    Require(text.find("   inet:127.0.0.1/255.0.0.0\n") != std::string::npos);
    Require(text.find("dns addresses\n") != std::string::npos);
    return text;
}

}  // namespace

int main() {
    std::array<char, BufferSize> scratch{};
    RequireError(sceNetShowIfconfigForBuffer(scratch.data(), BufferSize, 0), ENotInit, 200);
    RequireError(sceNetShowNetstatForBuffer(scratch.data(), BufferSize, 0), ENotInit, 200);
    RequireError(sceNetShowNetstatExForBuffer(scratch.data(), BufferSize, 0, 0), ENotInit, 200);

    Require(sceNetInit_nid_postfix() == 0);

    RequireError(sceNetShowIfconfigForBuffer(scratch.data(), ShowLimit - 1, 0), EInval, 22);
    RequireError(sceNetShowNetstatForBuffer(scratch.data(), ShowLimit - 1, 0), EInval, 22);
    RequireError(sceNetShowNetstatExForBuffer(scratch.data(), ShowLimit - 1, 0, 0), EInval, 22);
    RequireError(sceNetShowNetstatExForBuffer(nullptr, 0, 0, 0x10000), EInval, 22);
    RequireError(sceNetShowIfconfigForBuffer(reinterpret_cast<char*>(1), BufferSize, 0), EFault, EFault);

    *sceNetErrnoLoc() = 33;
    const std::string ifconfig = PrintedIfconfig();
    Require(*sceNetErrnoLoc() == 33);
    Require(ifconfig.size() < ShowLimit);

    std::vector<char> ifconfigBuffer = FilledBuffer();
    const auto [ifconfigPrinted, ifconfigResult] = CaptureStdout(
        [&] { return sceNetShowIfconfigForBuffer(ifconfigBuffer.data(), BufferSize, 0); });
    Require(ifconfigResult == 0);
    Require(ifconfigPrinted.empty());
    Require(std::memcmp(ifconfigBuffer.data(), ifconfig.data(), ifconfig.size()) == 0);
    Require(ifconfigBuffer[ifconfig.size()] == '\0');
    RequireUntouched(ifconfigBuffer, ifconfig.size() + 1);

    std::vector<int> sockets;
    for (int index = 0; index < 40; ++index) {
        const int socket = sceNetSocket("show-for-buffer", 2, 1, 6);
        Require(socket >= 0);
        sockets.push_back(socket);
    }

    const auto [netstat, netstatResult] = CaptureStdout([] { return sceNetShowNetstatForBuffer(nullptr, 0, 0); });
    Require(netstatResult == 0);
    Require(netstat.size() > ShowLimit);
    Require(netstat.find("show-for-buffer") != std::string::npos);

    std::vector<char> netstatBuffer = FilledBuffer();
    const auto [netstatPrinted, netstatBufferResult] = CaptureStdout(
        [&] { return sceNetShowNetstatForBuffer(netstatBuffer.data(), BufferSize, 0); });
    Require(netstatBufferResult == 0);
    Require(netstatPrinted.empty());
    Require(std::strlen(netstatBuffer.data()) == ShowLimit - 1);
    Require(netstatBuffer[ShowLimit - 1] == '\0');
    Require(std::memcmp(netstatBuffer.data(), netstat.data(), ShowLimit - 1) == 0);
    RequireUntouched(netstatBuffer, ShowLimit);

    std::vector<char> exBuffer = FilledBuffer();
    const auto [exPrinted, exResult] = CaptureStdout(
        [&] { return sceNetShowNetstatExForBuffer(exBuffer.data(), BufferSize, 0, 0); });
    Require(exResult == 0);
    Require(exPrinted.empty());
    Require(std::memcmp(exBuffer.data(), netstat.data(), ShowLimit - 1) == 0);
    RequireUntouched(exBuffer, ShowLimit);

    for (const int socket : sockets) Require(sceNetSocketClose(socket) == 0);
    Require(sceNetTerm() == 0);
    std::printf("show-for-buffer: ifconfig, netstat, truncation and error checks passed\n");
}
