#pragma once

#include "CAutoCleanup.h"

MAKE_AUTO_CLEANUP(HBRUSH, ::DeleteObject, NULL, CAutoCloseHBRUSH)
MAKE_AUTO_CLEANUP(HICON, ::DestroyIcon, NULL, CAutoHandleIcon)
MAKE_AUTO_CLEANUP(HMENU, ::DestroyMenu, NULL, CAutoHMENU)
MAKE_AUTO_CLEANUP(HFONT, ::DeleteObject, NULL, CAutoHFONT)



