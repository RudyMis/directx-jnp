#pragma once
#include <map>
#include "helper.h"

// TODO: Wzi¹æ to jakoœ zabezpieczyæ, klasa wala siê sama po pamiêci
class Bitmaps {
public:
    static HRESULT load_wic_bitmap_from_file(
        CComPtr<IWICImagingFactory> wic_factory,
        PCWSTR uri);

    static std::map<PCWSTR, IWICFormatConverter*> wic_bitmaps;
};

