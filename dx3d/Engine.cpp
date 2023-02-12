#include "Engine.h"

Engine::Engine(UINT width, UINT height) :
	window_size{ width, height },
	frame_index(0),
	viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
	scissor_rect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
	rtv_descriptor_size(0),
	cube(L"assets/cube.obj") {}

void Engine::get_hardware_adapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter) {
	*ppAdapter = nullptr;
	for (UINT adapterIndex = 0; ; ++adapterIndex) {
		IDXGIAdapter1* pAdapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter)) {
			// No more adapters to enumerate.
			break;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
			*ppAdapter = pAdapter;
			return;
		}
		pAdapter->Release();
	}
}

HRESULT Engine::on_init(HWND hwnd) {
	HRESULT hr = S_OK;
	hr = load_pipeline(hwnd); CHECK;
	hr = load_assets(); CHECK;
	init = true;
	return S_OK;
}

HRESULT Engine::on_render() {
	HRESULT hr = S_OK;

	// Record all the commands we need to render the scene into the command list.
	hr = populate_command_list(); CHECK;

	// Execute the command list.
	ID3D12CommandList* pp_command_lists[] = { command_list.Get()};
	command_queue->ExecuteCommandLists(_countof(pp_command_lists), pp_command_lists);

	// Present the frame.
	hr = swap_chain->Present(1, 0); CHECK;

	hr = wait_for_previous_frame(); CHECK;

	return S_OK;
}

HRESULT Engine::on_update() {
	camera.update();
	update_const_buffer();
	return S_OK;
}

void Engine::on_key_down(UINT8 wparam) {
	camera.on_key_down(wparam);
}

void Engine::on_key_up(UINT8 wparam) {
	camera.on_key_up(wparam);
}

bool Engine::is_init() {
	return init;
}

void Engine::update_const_buffer() {
	auto view = camera.get_view_matrix();
	auto proj = camera.get_projection_matrix(0.8f, window_size.width / window_size.height);

	DirectX::XMFLOAT4X4 data;
	DirectX::XMStoreFloat4x4(&data, view * proj);
	memcpy(const_buffer_data, &data, sizeof(data));
}

HRESULT Engine::load_pipeline(HWND hwnd) {
	HRESULT hr = S_OK;
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debug_controller;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)); CHECK;
		debug_controller->EnableDebugLayer();

		// Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)); CHECK;

	ComPtr<IDXGIAdapter1> hardware_adapter;
	get_hardware_adapter(factory.Get(), &hardware_adapter);

	hr = D3D12CreateDevice(
		hardware_adapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device)
	); CHECK;

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hr = device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)); CHECK;

	RECT rc;
	GetClientRect(hwnd, &rc);

	window_size = {
		(UINT32)rc.right - rc.left,
		(UINT32)rc.bottom - rc.top
	};

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
	swap_chain_desc.BufferCount = FrameCount;
	swap_chain_desc.Width = window_size.width;
	swap_chain_desc.Height = window_size.height;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swap_chain1;
	hr = factory->CreateSwapChainForHwnd(
		command_queue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		hwnd,
		&swap_chain_desc,
		nullptr,
		nullptr,
		&swap_chain1
	); CHECK;
	hr = swap_chain1.As(&swap_chain); CHECK;

	hr = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER); CHECK;

	frame_index = swap_chain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
		rtv_heap_desc.NumDescriptors = FrameCount;
		rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_heap)); CHECK;

		// Describe and create a depth stencil view (DSV) descriptor heap.
		hr = depth_buffer.load_pipeline(device, window_size); CHECK;

		// Create const buffer heap
		D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc = {};
		cbv_heap_desc.NumDescriptors = 1;
		cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hr = device->CreateDescriptorHeap(&cbv_heap_desc, IID_PPV_ARGS(&cbv_heap)); CHECK;

		rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++) {
			hr = swap_chain->GetBuffer(n, IID_PPV_ARGS(&render_targets[n])); CHECK;
			device->CreateRenderTargetView(render_targets[n].Get(), nullptr, rtv_handle);
			rtv_handle.Offset(1, rtv_descriptor_size);
		}
	}

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)); CHECK;
	return S_OK;
}

HRESULT Engine::load_assets() {
	HRESULT hr = S_OK;
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
		feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data)))) {
			feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[1] = {};
		CD3DX12_ROOT_PARAMETER1 root_parameters[1] = {};

		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

		D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc = {};
		root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, root_signature_flags);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		hr = D3DX12SerializeVersionedRootSignature(&root_signature_desc, feature_data.HighestVersion, &signature, &error); CHECK;
		hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature)); CHECK;
	}

	// Create the pipeline state, which includes compiling and loading shaders.
	{
		ComPtr<ID3DBlob> vertex_shader;
		ComPtr<ID3DBlob> pixel_shader;

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compile_flags = 0;
#endif

		hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compile_flags, 0, &vertex_shader, nullptr); CHECK;
		hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compile_flags, 0, &pixel_shader, nullptr); CHECK;

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
		pso_desc.InputLayout = { input_element_descs, _countof(input_element_descs) };
		pso_desc.pRootSignature = root_signature.Get();
		pso_desc.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.Get());
		pso_desc.PS = CD3DX12_SHADER_BYTECODE(pixel_shader.Get());
		pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pso_desc.SampleMask = UINT_MAX;
		pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pso_desc.NumRenderTargets = 1;
		pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pso_desc.SampleDesc.Count = 1;
		pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		hr = device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state)); CHECK;
	}

	// Create the command list.
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator.Get(), pipeline_state.Get(), IID_PPV_ARGS(&command_list)); CHECK;

	// Load meshes into vertex buffer
	vertex_buffer.add(cube, {{DirectX::XMFLOAT4X4()}});
	hr = vertex_buffer.bake(device, command_list); CHECK;

	// Create the constant buffer.
	{
		const UINT buffer_size = sizeof(SceneConstantBuffer);    // CB size is required to be 256-byte aligned.

		auto props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

		hr = device->CreateCommittedResource(
			&props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&const_buffer)); CHECK;

		// Describe and create a constant buffer view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
		cbv_desc.BufferLocation = const_buffer->GetGPUVirtualAddress();
		cbv_desc.SizeInBytes = buffer_size;
		device->CreateConstantBufferView(&cbv_desc, cbv_heap->GetCPUDescriptorHandleForHeapStart());

		// Map and initialize the constant buffer. We don't unmap this until the
		// app closes. Keeping things mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE read_range(0, 0);        // We do not intend to read from this resource on the CPU.
		hr = const_buffer->Map(0, &read_range, reinterpret_cast<void**>(&const_buffer_data)); CHECK;
		update_const_buffer();
	}

	hr = command_list->Close(); CHECK;
	ID3D12CommandList* command_lists[] = { command_list.Get() };
	command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)); CHECK;
		fence_value = 1;

		// Create an event handle to use for frame synchronization.
		fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fence_event == nullptr) {
			hr = HRESULT_FROM_WIN32(GetLastError()); CHECK;
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		hr = wait_for_previous_frame(); CHECK;
	}

	return S_OK;
}

HRESULT Engine::wait_for_previous_frame() {
	HRESULT hr = S_OK;

	const UINT64 prev_fence_val = fence_value;
	hr = command_queue->Signal(fence.Get(), prev_fence_val); CHECK;
	fence_value++;

	// Wait until the previous frame is finished.
	if (fence->GetCompletedValue() < prev_fence_val) {
		hr = fence->SetEventOnCompletion(prev_fence_val, fence_event); CHECK;
		WaitForSingleObject(fence_event, INFINITE);
	}
	frame_index = swap_chain->GetCurrentBackBufferIndex();

	return S_OK;
}

HRESULT Engine::populate_command_list() {
	HRESULT hr = S_OK;

	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	hr = command_allocator->Reset(); CHECK;

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	hr = command_list->Reset(command_allocator.Get(), pipeline_state.Get()); CHECK;

	// Set necessary state.
	command_list->SetGraphicsRootSignature(root_signature.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { cbv_heap.Get() };
	command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	command_list->SetGraphicsRootDescriptorTable(0, cbv_heap->GetGPUDescriptorHandleForHeapStart());

	command_list->RSSetViewports(1, &viewport);
	command_list->RSSetScissorRects(1, &scissor_rect);

	// Indicate that the back buffer will be used as a render target.
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	command_list->ResourceBarrier(1, &barrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart(), frame_index, rtv_descriptor_size);
	CD3DX12_CPU_DESCRIPTOR_HANDLE depth_handle(depth_buffer.handle());
    command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, &depth_handle);

	const float clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
	command_list->ClearDepthStencilView(depth_buffer.handle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command_list->IASetVertexBuffers(0, 1, vertex_buffer.view());

	// For more than one mesh, it's data should be used
	command_list->DrawInstanced(vertex_buffer.size(), 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	command_list->ResourceBarrier(1, &barrier);

	hr = command_list->Close(); CHECK;

	return S_OK;
}
