#pragma once

// Windows headers - MinGW uses lowercase, MSVC uses uppercase
#if defined(__MINGW32__) || defined(__MINGW64__)
#  include <windows.h>
#else
#  include <Windows.h>
#endif

#include <cocos2d.h>
#include <MinHook.h>
#include <gd.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdio>

#include "Offsets.hpp"

using std::uintptr_t;

using namespace cocos2d;