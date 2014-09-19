#include "Graphics\GraphicsBuffers.h"

void CreateVertexBuffer(ID3D11Device* device,  DirectX::VertexPositionColor* vertices, unsigned int length)
{
	 //generate vertex buffer
	CD3D11_BUFFER_DESC bufferDescription
	(
		sizeof(DirectX::VertexPositionColor) * length,
		D3D11_BIND_VERTEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = vertices;

	ID3D11Buffer* newVertexBuffer;
	HRESULT hr = device->CreateBuffer( &bufferDescription, &InitData, &newVertexBuffer );
	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Vertex Buffer");

	DirectX::SetDebugObjectName(newVertexBuffer,"vertex buffer");
}

void CreateVertexBuffer(ID3D11Device* device,  unsigned int* indices, unsigned int length)
{
	//generate index buffer
	CD3D11_BUFFER_DESC bufferDescription
	(
		sizeof( unsigned int ) * length,
		D3D11_BIND_INDEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = indices;

	ID3D11Buffer* newIndexBuffer;
	HRESULT hr = device->CreateBuffer( &bufferDescription, &InitData, &newIndexBuffer );
	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Vertex Buffer");;

	DirectX::SetDebugObjectName(newIndexBuffer, "index buffer");
}