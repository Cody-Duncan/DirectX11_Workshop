#include <directxmath.h>

#ifndef CONSTANTBUFFER_STRUCT
#define CONSTANTBUFFER_STRUCT

struct CBChangesInfrequently
{
    DirectX::XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mWorld;
    DirectX::XMFLOAT4 vMeshColor;
};

struct ConstantBuffer
{
    CBChangesInfrequently infrequentBuffer;
    CBChangesEveryFrame frameBuffer;
};

#endif