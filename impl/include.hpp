#ifndef INCLUDE_HPP
#define INCLUDE_HPP



#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "dwmapi.lib")

// driver
#include "driver/driver.hpp"
#pragma once

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <winternl.h>
#include <ntstatus.h>
#include <atomic>
#include <mutex>
#include <dwmapi.h>
#include <xmmintrin.h>
#include <d3d9.h>
#include <d3d11.h>
#include <dwmapi.h>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "dwmapi.lib")

#include "../impl/hexarray.hpp"

#include "../impl/importer.hpp"

#include "../impl/hooks/hooks.hpp"

#include "../impl/game/g_math.hpp"

#include "../impl/globals.hpp"

#include "../impl/skcrypt.hpp"

#include "../dependencies/framework/imgui.h"
#include "../dependencies/framework/imgui_impl_dx11.h"
#include "../dependencies/framework/imgui_impl_win32.h"

#include "../impl/utilities/utilities.hpp"

#include "../impl/game/game.hpp"

#include "../dependencies/framework/burbank.hpp"

#include "../impl/render/render.hpp"
#endif // !INCLUDE_HPP