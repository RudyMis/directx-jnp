#pragma once
#define MEAN_AND_LEAN
#include <Windows.h>
#include <wincodec.h>

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <atlbase.h>
#include <vector>

#define CHECK if (!SUCCEEDED(hr)) { return hr; }

typedef D2D1_POINT_2F Point2;

// helper type for the std::visit
template<class... Ts> struct match : Ts... { using Ts::operator()...; };
