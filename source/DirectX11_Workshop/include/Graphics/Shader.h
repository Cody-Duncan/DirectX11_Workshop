#pragma once
#include <D3d11shader.h> // D3D11_SHADER_VERSION_TYPE
#include "ComPtr.h"
#include "ShaderReflection.h"

class Shader
{
private:
	D3D11_SHADER_VERSION_TYPE m_shaderType;
	ShaderInfo m_shaderInfo;

public:
	Shader();
	Shader(D3D11_SHADER_VERSION_TYPE shaderType);
	virtual ~Shader();

	void SetShaderInfo(ShaderInfo&& shaderInfo) { m_shaderInfo = shaderInfo; }
};



class VertexShader : public Shader
{
	ComPtr<ID3D11VertexShader> m_shader;

public:
	VertexShader(ComPtr<ID3D11VertexShader>);
	virtual ~VertexShader();
};



class PixelShader : public Shader
{
	ComPtr<ID3D11PixelShader> m_shader;

public:
	PixelShader(ComPtr<ID3D11PixelShader>);
	virtual ~PixelShader();
};



class GeometryShader : public Shader
{
	ComPtr<ID3D11GeometryShader> m_shader;

public:
	GeometryShader(ComPtr<ID3D11GeometryShader>);
	virtual ~GeometryShader();
};



class ComputeShader : public Shader
{
	ComPtr<ID3D11ComputeShader> m_shader;

public:
	ComputeShader(ComPtr<ID3D11ComputeShader>);
	virtual ~ComputeShader();
};