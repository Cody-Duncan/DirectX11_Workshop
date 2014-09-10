#include "Graphics\Shader.h"

Shader::Shader()
{}

Shader::Shader(ID3D11VertexShader* pShader) : 
	m_shaderType(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER),
	m_vertexShader(pShader)

{}

Shader::Shader(ID3D11PixelShader* pShader) :
	m_shaderType(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER),
	m_pixelShader(pShader)
{}

Shader::Shader(ID3D11GeometryShader* pShader) :
	m_shaderType(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER),
	m_geometryShader(pShader)
{}

Shader::Shader(ID3D11ComputeShader* pShader) : 
	m_shaderType(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER),
	m_computeShader(pShader)
{}

Shader::~Shader()
{
	switch (m_shaderType)
	{
		case D3D11_SHVER_PIXEL_SHADER:    m_pixelShader->Release();    break;
		case D3D11_SHVER_VERTEX_SHADER:   m_vertexShader->Release();   break;
		case D3D11_SHVER_GEOMETRY_SHADER: m_geometryShader->Release(); break;
		case D3D11_SHVER_COMPUTE_SHADER:  m_computeShader->Release();  break;

		case D3D11_SHVER_HULL_SHADER:
		case D3D11_SHVER_DOMAIN_SHADER:
		case D3D11_SHVER_RESERVED0:
		default:
			break;
	}
}

Shader& Shader::operator = (ID3D11VertexShader* pShader)
{
	SetShader(pShader); 
	return *this;
}

Shader& Shader::operator=(ID3D11PixelShader* pShader)
{
	SetShader(pShader);
	return *this;
}

Shader& Shader::operator=(ID3D11GeometryShader* pShader)
{
	SetShader(pShader);
	return *this;
}

Shader& Shader::operator=(ID3D11ComputeShader* pShader)
{
	SetShader(pShader);
	return *this;
}


void Shader::SetShader(ID3D11VertexShader* pShader)
{
	m_shaderType = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER;
	m_vertexShader = pShader;
}

void Shader::SetShader(ID3D11PixelShader* pShader)
{
	m_shaderType = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER;
	m_pixelShader = pShader;
}

void Shader::SetShader(ID3D11GeometryShader* pShader)
{
	m_shaderType = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER;
	m_geometryShader = pShader;
}

void Shader::SetShader(ID3D11ComputeShader* pShader)
{
	m_shaderType = D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER;
	m_computeShader = pShader;
}
