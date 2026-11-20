# libScePad

## change scePadSetToggleAllReleaseLeonardo

- [scePadSetToggleAllReleaseLeonardo, scePadSetProfileIndicatorAnimation](../../../core/libs/prx/libScePad/Export.cpp) (libScePad) - argument registers recovered (a handle, plus one animation byte). What the Access controller's all-release mode and which animation values are valid are unknown; nothing is measured. Both still throw.

## add Functional

- [scePadSetProfileEditMode, scePadGetProfileEditMode, scePadGetProfileEditModeLeonardo, scePadSetSelectButtonCustomizeMode, scePadGetSelectButtonCustomizeMode](../../../core/libs/prx/libScePad/Export.cpp) (libScePad) - the handle, bool, profile-number and 32-bit mode arguments follow firmware 13.42's exported declarations, but no console answer is measured. Each handle records the requested profile-edit flag and select-button customization flag; the getters report the stored flag as 1 or 0, and `scePadGetProfileEditMode` keeps the profile number in the same record without exposing it. Standard and special handles share the table, the calls need `scePadInit`, a null mode pointer returns `INVALID_ARG` and an unknown handle `INVALID_HANDLE`. The Access controller's profile, edit-mode and customization semantics and the console's answers for an absent controller are unknown.
