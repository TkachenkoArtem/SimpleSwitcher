#pragma once

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows:
#include <windows.h>
#include <Winuser.h>
#include <Psapi.h>

//#include <VersionHelpers.h>


// Файлы заголовков C RunTime
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//#include <boost/thread.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <string>

#include "tools-include.h"
#include "SwUtils.h"
#include "consts.h"
//#include "SettingsGui.h"

# pragma comment(lib, "Psapi.lib")










