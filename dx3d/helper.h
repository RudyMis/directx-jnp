#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX

#include <Windows.h>
#include <wincodec.h>

#include "dx12helper.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <directxmath.h>
#include <D3Dcompiler.h>
#include <dwrite_3.h>
#include <atlbase.h>
#include <vector>
#include <optional>

#define CHECK if (FAILED(hr)) { return hr; }

using Microsoft::WRL::ComPtr;

// helper type for the std::visit
template<class... Ts> struct match : Ts... { using Ts::operator()...; };

inline HRESULT get_resource_size(ComPtr<ID3D12Resource> resource, UINT64& resource_size) {
	auto desc = resource.Get()->GetDesc();
	CComPtr<ID3D12Device> device_p = nullptr;
	HRESULT hr = resource.Get()->GetDevice(IID_PPV_ARGS(&device_p)); CHECK;
	device_p->GetCopyableFootprints(
		&desc, 0, 1, 0, nullptr, nullptr, nullptr, &resource_size
	);
	return S_OK;
}

struct SceneConstantBuffer {
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4 padding[12]; // Padding so the constant buffer is 256-byte aligned.
};

static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
