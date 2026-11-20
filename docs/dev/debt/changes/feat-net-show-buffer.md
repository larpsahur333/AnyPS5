# libSceNet

## change [sceNetShowIfconfigForBuffer, ShowRouteForBuffer, ShowRoute6ForBuffer, ShowNetstatForBuffer, ShowNetstatExForBuffer]

- [sceNetShowIfconfigForBuffer, ShowRouteForBuffer, ShowRoute6ForBuffer, ShowNetstatForBuffer, ShowNetstatExForBuffer](../../../core/libs/prx/libSceNet/Export.cpp) (libSceNet) - PS4 10.71 checks: init, then a non-null buffer under 0x2000 bytes is `EINVAL`; NetstatEx rejects flag 0x10000 first. Ifconfig, Netstat and NetstatEx then write the text `sceNetShowIfconfig` and `sceNetShowNetstat` print, cut to 0x2000 bytes whatever the size, or print it when the buffer is null; the pool id is ignored because no pool is read, and that text is AnyPS5's own, not the kernel tables the module formats. RouteForBuffer and Route6ForBuffer still throw, because the emulator models no route table. Confirmed on a PS4 Pro, firmware 10.71: a buffer under 0x2000 is EINVAL, and a 0x2000 buffer with pool 0 prints the interface text and returns 0.
