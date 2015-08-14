#pragma once
#include "..\gr_shared.h"
#include <Windows.h>
#include <string>

static inline i32 MessageBoxWS(HWND hwnd, std::wstring title, std::wstring msg, u32 mode)
{ return MessageBoxW(hwnd, msg.data(), title.data(), mode); }