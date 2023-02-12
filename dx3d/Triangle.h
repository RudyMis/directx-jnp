#pragma once
#include "helper.h"
#include <DirectXMath.h>

class Triangle {
public:
    HRESULT load_assets(ComPtr<ID3D12Device> device, UINT height, UINT width);
    void on_render(ComPtr<ID3D12GraphicsCommandList> command_list) const;
private:
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

    // App resources.
    ComPtr<ID3D12Resource> vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
};

