#include "dx3d_handle.h"
#include <dxgi1_6.h>

void Dx3dHandle::init() {
	load_pipeline();
	load_assets();
}

void Dx3dHandle::load_pipeline() {
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug_controller;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
		debug_controller->EnableDebugLayer();
	}
#endif
	ComPtr<IDXGIFactory7> factory;
	auto err = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	if (!SUCCEEDED(err)) {
		return;
	}
	
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	err = device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue));
	if (!SUCCEEDED(err)) {
		return;
	}
	
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = frame_count;
	swap_chain_desc.BufferDesc.Width = width;
	swap_chain_desc.BufferDesc.Height = height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.OutputWindow = Win32Application::GetHwnd(); // TODO!
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.Windowed = TRUE;

	ComPtr<IDXGISwapChain> swap_chain_creator;
	err = factory->CreateSwapChain(
		command_queue.Get(),
		&swap_chain_desc,
		&swap_chain_creator
		);
	if (!SUCCEEDED(err)) {
		return;
	}
	
	err = swap_chain_creator.As(&swap_chain);
	if (!SUCCEEDED(err)) {
		return;
	}
	
	err = factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
	if (!SUCCEEDED(err)) {
		return;
	}

	frame_index = swap_chain->GetCurrentBackBufferIndex();

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
		rtv_heap_desc.NumDescriptors = frame_count;
		rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		err = device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_heap));
		if (!SUCCEEDED(err)) {
			return;
		}

		rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// TODO: Frame resources (in chain)
	err = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator));
	if (!SUCCEEDED(err)) { // For future me to implement err handling
		return;
	}
}
