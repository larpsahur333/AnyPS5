# libSceSystemService

## add Functional

- [sceShellCoreUtilGetUIStatus, sceShellCoreUtilSetUIStatus, sceShellCoreUtilGetGnmCompositorOnScreenProfilerFlag, sceShellCoreUtilSetGnmCompositorOnScreenProfilerFlag](../../../core/libs/prx/libSceSystemService/PendingExports.cpp) (libSceSystemService) - the shell UI status is one 64-bit word stored and read back; the setter replaces the bits its mask selects and keeps the others, read from the two arguments, and the word starts at 0 because AnyPS5 has no shell UI state to report. The GNM compositor on-screen profiler flag is stored and read back too and starts off; a non-zero value stores true. Neither changes anything on screen: AnyPS5 has no shell UI and no compositor profiler. A null or unwritable status output returns `0x80E30005`, a host-safety fallback. `sceSystemServiceGetUIStatus` forwards to the ShellCore getter. The initial values, the mask meaning and the console's errors are unmeasured.
