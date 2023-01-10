#pragma once
#include <map>
#include "helper.h"

// TODO: Wzi�� to jako� zabezpieczy�, klasa wala si� sama po pami�ci
class Bitmaps {
public:
    static HRESULT load_wic_bitmap_from_file(
        CComPtr<IWICImagingFactory> wic_factory,
        PCWSTR uri);

    static std::map<PCWSTR, IWICFormatConverter*> wic_bitmaps;
};

