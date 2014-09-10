#pragma once

#include <d3d11.h>
#include "DXTK\DirectXHelpers.h"

// Helper for creating a D3D vertex or index buffer.
template<typename T, unsigned TNameLength>
void CreateBuffer(ID3D11Device* device, T const& data, D3D11_BIND_FLAG bindFlags, const char(&debugName)[TNameLength], ID3D11Buffer** pBuffer)
{
	ASSERT_DEBUG(pBuffer != 0, "Output parameter 'pBuffer' is null");

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.ByteWidth = (UINT)data.size() * sizeof(T::value_type);
	bufferDesc.BindFlags = bindFlags;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA dataDesc = { 0 };
	dataDesc.pSysMem = &data.front();

	HRESULT hr = device->CreateBuffer(&bufferDesc, &dataDesc, pBuffer);
	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to create Buffer");

	DirectX::SetDebugObjectName(*pBuffer, debugName);
}
