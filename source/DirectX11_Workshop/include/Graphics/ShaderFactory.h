#pragma once
#include "Graphics\ComPtr.h"
#include "Shader.h"
#include <vector>
#include <D3d11shader.h> // D3D11_SHADER_VERSION_TYPE

class ShaderFactory
{
private:
	ID3D11Device* m_device;
	std::vector<Shader> m_vertexShaders;
	std::vector<Shader> m_pixelShaders;
	std::vector<Shader> m_geometryShaders;
	std::vector<Shader> m_computeShaders;

	Shader* _BuildShaderObject(ComPtr<ID3DBlob> shaderBlob, std::string shaderFilename, D3D11_SHADER_VERSION_TYPE shaderType);

	Shader*  _CreateVertexShader(ID3DBlob* blob);
	Shader*  _CreatePixelShader(ID3DBlob* blob);
	Shader*  _CreateGeometryShader(ID3DBlob* blob);
	Shader*  _CreateComputeShader(ID3DBlob* blob);
	
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