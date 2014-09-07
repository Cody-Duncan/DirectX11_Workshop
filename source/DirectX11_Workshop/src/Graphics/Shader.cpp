#include "Graphics\Shader.h"

Shader::Shader() :
	m_shaderInfo()
{}

Shader::Shader(D3D11_SHADER_VERSION_TYPE shaderType) 
	: m_shaderType(shaderType)
{

}

Shader::~Shader()
{}



VertexShader::VertexShader(ComPtr<ID3D11VertexShader> shader) : 
	Shader(D3D11_SHVER_VERTEX_SHADER),
	m_shader(shader)
{}

VertexShader::~VertexShader()
{}



PixelShader::PixelShader(ComPtr<ID3D11PixelShader> shader) :
	Shader(D3D11_SHVER_PIXEL_SHADER),
	m_shader(shader)
{}

PixelShader::~PixelShader()
{}



GeometryShader::GeometryShader(ComPtr<ID3D11GeometryShader> shader) :
	Shader(D3D11_SHVER_GEOMETRY_SHADER),
	m_shader(shader)
{}

GeometryShader::~GeometryShader()
{}



ComputeShader::ComputeShader(ComPtr<ID3D11ComputeShader> shader) :
	Shader(D3D11_SHVER_COMPUTE_SHADER),
	m_shader(shader)
{}

ComputeShader::~ComputeShader()
{}