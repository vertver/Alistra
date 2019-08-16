#pragma once
#include <unordered_map>

class ComputeShader final
{
private:
    std::unordered_map<std::string, unsigned> shaderCache;
public:
    void Dispatch(unsigned ThreadGroupX, unsigned ThreadGroupY, unsigned ThreadGroupZ);
};
