#include "bitmaps.h"

std::map<PCWSTR, IWICFormatConverter*> Bitmaps::wic_bitmaps{};

HRESULT Bitmaps::load_wic_bitmap_from_file(CComPtr<IWICImagingFactory> wic_factory, PCWSTR uri) {
    IWICBitmapDecoder* decoder;
    IWICBitmapFrameDecode* source;
    IWICStream* stream;
    IWICFormatConverter* converter;
    IWICBitmapScaler* scaler;

    HRESULT hr = wic_factory->CreateDecoderFromFilename(
        uri,
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    ); CHECK;

    hr = decoder->GetFrame(0, &source); CHECK;
    
    hr = wic_factory->CreateFormatConverter(&converter); CHECK;

    hr = converter->Initialize(
        source,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeMedianCut
    ); CHECK;

    Bitmaps::wic_bitmaps[uri] = converter;
    return S_OK;
}
