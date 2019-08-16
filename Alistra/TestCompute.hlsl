#pragma kernel CSMain
RWTexture2D<float4> Destination : register(u0);

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix WorldInverse;
    float3 lightDirection;
};

cbuffer InputBuffer : register(b1)
{
    float  power;
    float  darkness;
    float  blackAndWhite;
    float3 colorAMix;
    float3 colorBMix;
};

static const float epsilon = 0.001f;
static const float maxDistance = 250;
static const int maxStepCount = 300;

struct Ray {
    float3 origin;
    float3 direction;
};

Ray CreateRay(float3 origin, float3 direction)
{
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    return ray;
}

Ray CreateCameraRay(float2 uv)
{
    float3 origin = mul(World, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    float3 direction = mul(WorldInverse, float4(uv, 0.0f, 1.0f)).xyz;
    direction = mul(World, float4(direction, 0.0f)).xyz;
    direction = normalize(direction);
    return CreateRay(origin, direction);
}

//http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/
float2 SceneInfo(float3 position) {
    float3 z = position;
    float dr = 1.0;
    float r = 0.0;
    int iterations = 0;

    for (int i = 0; i < 15; i++) {
        iterations = i;
        r = length(z);

        if (r > 2) {
            break;
        }

        // convert to polar coordinates
        float theta = acos(z.z / r);
        float phi = atan2(z.y, z.x);
        dr = pow(r, power - 1.0)*power*dr + 1.0;

        // scale and rotate the point
        float zr = pow(r, power);
        theta = theta * power;
        phi = phi * power;

        // convert back to cartesian coordinates
        z = zr * float3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
        z += position;
    }
    float dst = 0.5*log(r)*r / dr;
    return float2(iterations, dst * 1);
}

float3 EstimateNormal(float3 p) {
    float x = SceneInfo(float3(p.x + epsilon, p.y, p.z)).y - SceneInfo(float3(p.x - epsilon, p.y, p.z)).y;
    float y = SceneInfo(float3(p.x, p.y + epsilon, p.z)).y - SceneInfo(float3(p.x, p.y - epsilon, p.z)).y;
    float z = SceneInfo(float3(p.x, p.y, p.z + epsilon)).y - SceneInfo(float3(p.x, p.y, p.z - epsilon)).y;
    return normalize(float3(x, y, z));
}

[numthreads(1, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint width;
    uint height;
    Destination.GetDimensions(width, height);

    float2 uv = id.xy / float2(width, height);

    Ray ray = CreateCameraRay(uv * 2.0f - 1.0f); 
    float rayDistance = 0.0f;
    int marchSteps = 0;

    float4 result = lerp(float4(51, 3, 20, 1), float4(16, 6, 28, 1), uv.y) / 255.0;

    while (rayDistance < maxDistance && marchSteps < maxStepCount)
    {
        marchSteps++;
        float2 sceneInfo = SceneInfo(ray.origin);
        float distance = sceneInfo.y;

        if (distance <= epsilon)
        {
            float iterations = sceneInfo.x;
            float3 normal = EstimateNormal(ray.origin - ray.direction * epsilon * 2.0f);

            float colorA = saturate(dot(normal*.5 + .5, -lightDirection));
            float colorB = saturate(iterations / 16.0);
            float3 colorMix = saturate(colorA * colorAMix + colorB * colorBMix);

            result = float4(colorMix, 1.0f);
            break;
        }
        ray.origin += ray.direction * distance;
        rayDistance += distance;
    }

    float rim = marchSteps / darkness;
    Destination[id.xy] = lerp(result, 1.0f, blackAndWhite) * rim;
}