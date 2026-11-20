#include "prx/libSceSystemService/include/SystemService.hpp"
#include "SceTypes.hpp"
#include <cstdint>
#include <cstdlib>

namespace {

void Require(bool value) { if (!value) std::abort(); }

constexpr int ShellCoreInvalidArgument = static_cast<int>(0x80E30005u);

}

int main() {
    std::uint64_t status = ~static_cast<std::uint64_t>(0);
    Require(sceShellCoreUtilGetUIStatus(&status) == 0 && status == 0);
    Require(sceSystemServiceGetUIStatus(&status) == 0 && status == 0);
    Require(sceShellCoreUtilSetUIStatus(0x5, 0xFF) == 0);
    status = 0;
    Require(sceShellCoreUtilGetUIStatus(&status) == 0 && status == 0x5);
    Require(sceShellCoreUtilSetUIStatus(0x2, 0x2) == 0);
    status = 0;
    Require(sceShellCoreUtilGetUIStatus(&status) == 0 && status == 0x7);
    Require(sceShellCoreUtilSetUIStatus(0, 0x1) == 0);
    status = 0;
    Require(sceShellCoreUtilGetUIStatus(&status) == 0 && status == 0x6);
    Require(sceShellCoreUtilSetUIStatus(~static_cast<std::uint64_t>(0), 0x80) == 0);
    status = 0;
    Require(sceShellCoreUtilGetUIStatus(&status) == 0 && status == 0x86);
    Require(sceShellCoreUtilGetUIStatus(nullptr) == ShellCoreInvalidArgument);
    auto* const inaccessible = reinterpret_cast<std::uint64_t*>(0x10);
    Require(sceShellCoreUtilGetUIStatus(inaccessible) == ShellCoreInvalidArgument);
    status = 0;
    Require(sceShellCoreUtilGetUIStatus(&status) == 0 && status == 0x86);
    Require(!sceShellCoreUtilGetGnmCompositorOnScreenProfilerFlag());
    Require(sceShellCoreUtilSetGnmCompositorOnScreenProfilerFlag(1) == 0);
    Require(sceShellCoreUtilGetGnmCompositorOnScreenProfilerFlag());
    Require(sceShellCoreUtilSetGnmCompositorOnScreenProfilerFlag(7) == 0);
    Require(sceShellCoreUtilGetGnmCompositorOnScreenProfilerFlag());
    Require(sceShellCoreUtilSetGnmCompositorOnScreenProfilerFlag(0) == 0);
    Require(!sceShellCoreUtilGetGnmCompositorOnScreenProfilerFlag());
}
