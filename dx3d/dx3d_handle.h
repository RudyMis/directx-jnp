#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
//#include <memory>
#include <string>

#include "DxBase.h"

namespace {
	using Microsoft::WRL::ComPtr;
}

class Dx3dHandle : DxBase {
public:
	Dx3dHandle(UINT width, UINT height, std::wstring name);
	~Dx3dHandle();

	void init();
	void update();
	void render();

private:
	static const UINT frame_count = 2;
	static const UINT width = 600;
	static const UINT height = 400;

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};
	
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor_rect;
	//std::shared_ptr<IDXGISwapChain3> swap_chain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12Resource> render_targets[frame_count];
	ComPtr<ID3D12CommandAllocator> command_allocator;
	ComPtr<ID3D12CommandQueue> command_queue;
	ComPtr<ID3D12RootSignature> root_signature;
	ComPtr<ID3D12DescriptorHeap> rtv_heap;
	ComPtr<ID3D12PipelineState> pipeline_state;
	ComPtr<ID3D12GraphicsCommandList> command_list;

	ComPtr<IDXGISwapChain3> swap_chain;

	UINT rtv_descriptor_size;

	ComPtr<ID3D12Resource> vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;

	UINT frame_index;
	HANDLE fence_event;
	ComPtr<ID3D12Fence> fence;
	UINT64 fence_value;

	void load_pipeline();
	void load_assets();
	void populate_command_list();
	void wait_for_previous_frame();
};