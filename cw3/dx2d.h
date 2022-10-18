#pragma once


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1_3.h>

void init_direct_2d(HWND hwnd);

void destroy_direct_2d();

void paint(HWND hwnd);
