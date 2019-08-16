Texture2D Destination : register(t0);

SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
struct VS_QUARD_INPUT {
    uint VertexID: 		SV_VertexID;
};

struct PS_QUARD_INPUT {
    float4 Pos: 		SV_POSITION;
    float2 texCoord: 	TEXCOORD;
};


PS_QUARD_INPUT VS(VS_QUARD_INPUT input) {
    PS_QUARD_INPUT output;
    uint id = input.VertexID;
    float x = -1, y = -1;
    x = (id == 2) ? 3.0 : -1.0;
    y = (id == 1) ? 3.0 : -1.0;
    output.Pos = float4(x, y, 1.0, 1.0);
    output.texCoord = output.Pos.xy * 0.5 + 0.5;
    output.texCoord.y = 1.0 - output.texCoord.y;
    return output;
}

float4 PS(PS_QUARD_INPUT input) : SV_Target {
    return Destination.Sample(MeshTextureSampler, input.texCoord);
}
