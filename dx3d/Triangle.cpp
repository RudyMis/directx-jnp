#include "Triangle.h"

HRESULT Triangle::load_assets(ComPtr<ID3D12Device> device, UINT height, UINT width) {
    HRESULT hr = S_OK;
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    // Create the vertex buffer.
    // Define the geometry for a triangle.
    Vertex triangleVertices[] =
    {
        { { 0.0f, 0.25f * aspect_ratio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    const UINT vertexBufferSize = sizeof(triangleVertices);

    // Note: using upload heaps to transfer static data like vert buffers is not 
    // recommended. Every time the GPU needs it, the upload heap will be marshalled 
    // over. Please read up on Default Heap usage. An upload heap is used here for 
    // code simplicity and because there are very few verts to actually transfer.
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertex_buffer)); CHECK;

    // Copy the triangle data to the vertex buffer.
    UINT8* vertex_data_begin = nullptr;
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    hr = vertex_buffer->Map(0, &readRange, reinterpret_cast<void**>(&vertex_data_begin)); CHECK;
    memcpy(vertex_data_begin, triangleVertices, sizeof(triangleVertices));
    vertex_buffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
    vertex_buffer_view.StrideInBytes = sizeof(Vertex);
    vertex_buffer_view.SizeInBytes = vertexBufferSize;
}

void Triangle::on_render(ComPtr<ID3D12GraphicsCommandList> command_list) const {
    command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
    command_list->DrawInstanced(3, 1, 0, 0);
}
