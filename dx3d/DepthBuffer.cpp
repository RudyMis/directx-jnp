#include "DepthBuffer.h"

HRESULT DepthBuffer::load_pipeline(ComPtr<ID3D12Device> device, const D2D1_SIZE_U& window_size) {
	HRESULT hr = S_OK;
	hr = create_descriptor_heap(device); CHECK;
	hr = create_depth_buffer(device, window_size); CHECK;
	create_depth_stencil_view(device);
	return S_OK;
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthBuffer::handle() {
	return desc_heap->GetCPUDescriptorHandleForHeapStart();
}

HRESULT DepthBuffer::create_descriptor_heap(ComPtr<ID3D12Device> device) {
	D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
	};
	return device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap));
}

HRESULT DepthBuffer::create_depth_buffer(ComPtr<ID3D12Device> device, const D2D1_SIZE_U& window_size) {
	CD3DX12_HEAP_PROPERTIES props(D3D12_HEAP_TYPE_DEFAULT);
	auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		window_size.width,
		window_size.height,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	CD3DX12_CLEAR_VALUE clear_value(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
	return device->CreateCommittedResource(
		&props,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clear_value,
		IID_PPV_ARGS(&buffer)
	);
}

void DepthBuffer::create_depth_stencil_view(ComPtr<ID3D12Device> device) {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = desc_heap->GetCPUDescriptorHandleForHeapStart();
	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {
		.Format = DXGI_FORMAT_D32_FLOAT,
		.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
		.Flags = D3D12_DSV_FLAG_NONE,
		.Texture2D = {}
	};
	device->CreateDepthStencilView(
		buffer.Get(),
		&desc,
		handle
	);
}
