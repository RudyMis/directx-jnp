#pragma once
#include "helper.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "Camera.h"
#include "DepthBuffer.h"

class Engine {
public:
	Engine(UINT width, UINT height);

	HRESULT on_init(HWND hwnd);
	HRESULT on_render();
	HRESULT on_update();

	void on_key_down(UINT8 wparam);
	void on_key_up(UINT8 wparam);

	bool is_init();

	void update_const_buffer();

private:
	
	static const UINT FrameCount = 2;

	HRESULT load_pipeline(HWND hwnd);
	HRESULT load_assets();
	HRESULT wait_for_previous_frame();
	HRESULT populate_command_list();

	void get_hardware_adapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);

	// Pipeline objects.
	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissor_rect;
	ComPtr<IDXGISwapChain3> swap_chain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12Resource> render_targets[2];
	ComPtr<ID3D12Resource> const_buffer;
	ComPtr<ID3D12CommandAllocator> command_allocator;
	ComPtr<ID3D12CommandQueue> command_queue;
	ComPtr<ID3D12RootSignature> root_signature;
	ComPtr<ID3D12DescriptorHeap> rtv_heap;
	ComPtr<ID3D12DescriptorHeap> cbv_heap;
	ComPtr<ID3D12PipelineState> pipeline_state;
	ComPtr<ID3D12GraphicsCommandList> command_list;
	UINT rtv_descriptor_size;
	DepthBuffer depth_buffer;

	D2D1_SIZE_U window_size;

	// Synchronization objects.
	UINT frame_index;
	HANDLE fence_event;
	ComPtr<ID3D12Fence> fence;
	UINT64 fence_value;

	VertexBuffer vertex_buffer;
	Mesh cube;
	Camera camera;
	SceneConstantBuffer* const_buffer_data;

	IWICImagingFactory* wic_factory;

	ComPtr<IDWriteFactory> write_factory;
	ComPtr<IDWriteTextFormat> text_format;
	bool init = false;
};

