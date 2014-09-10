#pragma once
#include <D3d11shader.h> // D3D11_SHADER_VERSION_TYPE
#include "ComPtr.h"

class Shader
{
public:
	D3D11_SHADER_VERSION_TYPE m_shaderType;
	union
	{
		ID3D11VertexShader*   m_vertexShader;
		ID3D11PixelShader*    m_pixelShader;
		ID3D11GeometryShader* m_geometryShader;
		ID3D11ComputeShader*  m_computeShader;
	};

	enum ShaderType
	{
		SHADER_VERTEX   = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER,
		SHADER_PIXEL    = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER,
		SHADER_GEOMETRY = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER,
		SHADER_COMPUTE  = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER,
	};

	Shader();
	Shader(ID3D11VertexShader*);
	Shader(ID3D11PixelShader*);
	Shader(ID3D11GeometryShader*);
	Shader(ID3D11ComputeShader*);
	virtual ~Shader();

	Shader& operator=(ID3D11VertexShader*);
	Shader& operator=(ID3D11PixelShader*);
	Shader& operator=(ID3D11GeometryShader*);
	Shader& operator=(ID3D11ComputeShader*);

	void SetShader(ID3D11VertexShader*  );
	void SetShader(ID3D11PixelShader*   );
	void SetShader(ID3D11GeometryShader*);
	void SetShader(ID3D11ComputeShader* );

	ShaderType GetShaderType()
	{
		return (ShaderType)m_shaderType;
	}
};