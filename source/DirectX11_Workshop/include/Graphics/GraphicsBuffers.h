#pragma once
#include <DXTK\DirectXHelpers.h>
#include <DXTK\VertexTypes.h>


void CreateVertexBuffer(ID3D11Device* device, DirectX::VertexPositionColor* vertices, unsigned int length);
void CreateVertexBuffer(ID3D11Device* device, unsigned int* indices, unsigned int length);
