cbuffer SceneConstantBuffer {
    float4x4 matWorldViewProj;
    float4 padding[12];	// zapewnia buforowi sta³emu rozmiar 
    // bêd¹cy krotnoœci¹ 256 bajtów
};

struct PSInput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD) {
    PSInput result;
    
    result.position = mul(position, transpose(matWorldViewProj));
    result.uv = uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    float4 color;
    color.r = 1.0 - input.position.z;
    color.b = 1.0 - input.position.z;
    color.g = 1.0 - input.position.z;

    return color;
}
