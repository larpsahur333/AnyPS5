# libSceHmd

## add Functional

- [sceHmdInternalGetDebugMode, sceHmdInternalGetDebugTextMode, sceHmdInternalSetDebugMode and sceHmdInternalSetDebugTextMode](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - the library records the debug mode and debug text mode the caller sets and returns the recorded byte from the matching getter, one independent byte each; neither value reaches a headset and no device state is reported. Before `sceHmdInitialize` both getters return `0x81110002` after their null check, both setters return `0x81110002` without storing, and `sceHmdInitialize` starts both at 0. A null or unwritable output returns `0x81110008` and leaves `errno` unchanged. The firmware's own answers, the values it accepts and its output checks are not read. Closes when: measured on a PS5.
