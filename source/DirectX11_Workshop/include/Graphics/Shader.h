#pragma once
#include <D3d11shader.h> // D3D11_SHADER_VERSION_TYPE
#include "ComPtr.h"

class Shader
{
private:
	D3D11_SHADER_VERSION_TYPE m_shaderType;
	union
	{
		ID3D11VertexShader*   m_vertexShader;
		ID3D11PixelShader*    m_pixelShader;
		ID3D11GeometryShader* m_geometryShader;
		ID3D11ComputeShader*  m_computeShader;
	};

public:
	Shader();
	Shader(ID3D11VertexShader*);
	Shader(ID3D11PixelShader*);
	Shader(ID3D11GeometryShader*);
	Shader(ID3D11ComputeShader*);
	virtual ~Shader();
};

