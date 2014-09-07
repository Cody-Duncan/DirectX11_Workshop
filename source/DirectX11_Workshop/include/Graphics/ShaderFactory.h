#pragma once
#include "Graphics\ComPtr.h"
#include "Shader.h"
#include <vector>
#include <D3d11shader.h> // D3D11_SHADER_VERSION_TYPE

class ShaderFactory
{
private:
	ID3D11Device* m_device;
	std::vector<Shader> m_shaders;
	Shader* _BuildShaderObject(ComPtr<ID3DBlob> shaderBlob, std::string shaderFilename, D3D11_SHADER_VERSION_TYPE shaderType);

	VertexShader*   _CreateVertexShader(ID3DBlob* blob);
	PixelShader*    _CreatePixelShader(ID3DBlob* blob);
	GeometryShader* _CreateGeometryShader(ID3DBlob* blob);
	ComputeShader*  _CreateComputeShader(ID3DBlob* blob);
	void _LoadShader(ComPtr<ID3DBlob> shaderBlob);
	
public:
	ShaderFactory(ID3D11Device* device);
	~ShaderFactory();

	Shader* BuildSourceShader(std::string shaderFilename, std::string EntryPoint, std::string ShaderModel);
	Shader* BuildCompiledShader(std::string shaderFilename, D3D11_SHADER_VERSION_TYPE shaderType);
	void FreeShader(std::string shaderFilename);

	//Non-Copyable
	ShaderFactory(const ShaderFactory& rhs) = delete;
	ShaderFactory(ShaderFactory&& rhs) = delete;
	ShaderFactory& operator=(const ShaderFactory& rhs) = delete;
};