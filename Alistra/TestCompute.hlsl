RWTexture2D<float4> Destination : register(u0);

cbuffer ConstantBuffer : register(b0)
{
    float2 resolution;
    float time;
};

float map(float3 p)
{
    float3 q = frac(p) * 2.0 - 1.0;
    return length(q) - 0.25;
}

float trace(float3 o, float3 r)
{
    float t = 0.0;
    for (int i = 0; i < 64; i++) 
    {
        float3 p = o + r * t;

        float d = map(p);
        t += d * 0.5;
    }
    return t;
}

[numthreads(1, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint width;
    uint height;
    Destination.GetDimensions(width, height);

    float2 uv = id.xy / float2(width, height);
    uv = (uv * 2.0 - 1.0);
    uv.x *= resolution.x / resolution.y;

    float3 ray = normalize(float3(uv, 1.0));
    float timeSlow = time * 0.1;
    ray.xz = mul(ray.xz, float2x2(cos(timeSlow), -sin(timeSlow), sin(timeSlow), cos(timeSlow)));

    float3 cameraPos = float3(0.0, 0.0, time);

    float traceResult = trace(cameraPos, ray);

    float fog = 1.0 / (1.0 + traceResult * traceResult * 0.1);

    Destination[id.xy] = float4(fog.xxx, 1.0);

    //float4 pz = float4(p, time, 1.0);
    //float4 d = pz;
    //float4 t, c;

    //for (int i = 0; i < 90; ++i)
    //{
    //    t = fmod(pz += d * t.z * 0.2, 6.0) - 3.0;
    //    t = length(t.xy) - 0.5;
    //    Destination[id.xy] = t;
    //}
}