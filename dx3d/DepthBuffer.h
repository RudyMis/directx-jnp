#pragma once
#include "helper.h"

class DepthBuffer {
public:
	HRESULT load_pipeline(ComPtr<ID3D12Device> device, const D2D1_SIZE_U& window_size);

	D3D12_CPU_DESCRIPTOR_HANDLE handle();

private:
	HRESULT create_descriptor_heap(ComPtr<ID3D12Device> device);
	HRESULT create_depth_buffer(ComPtr<ID3D12Device> device, const D2D1_SIZE_U& window_size);
	void create_depth_stencil_view(ComPtr<ID3D12Device> device);

	ComPtr<ID3D12Resource> buffer;
	ComPtr<ID3D12DescriptorHeap> desc_heap;
};

