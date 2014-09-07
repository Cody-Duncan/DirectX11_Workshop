#include "Graphics\ShaderFactory.h"
#include "String\StringAlgorithm.h"
#include "Graphics\ShaderReflection.h"
#include <d3dcompiler.h>

ShaderFactory::ShaderFactory(ID3D11Device* device) : 
	m_device(device)
{

}

ShaderFactory::~ShaderFactory()
{

}

void ShaderFactory::FreeShader(std::string shaderFilename)
{

}

/* ----------  Conversion Functions ----------*/

D3D11_SHADER_VERSION_TYPE Convert_ShaderModel_ShaderTypeEnum(std::string& ShaderModel)
{
	if (String::Contains(ShaderModel, "vs"))
	{
		return D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER;
	}
	if (String::Contains(ShaderModel, "ps"))
	{
		return D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER;
	}
	if (String::Contains(ShaderModel, "gs"))
	{
		return D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER;
	}
	if (String::Contains(ShaderModel, "cs"))
	{
		return D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER;
	}

	return D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_RESERVED0;
}

const char* Convert_ShaderTypeEnum_String(D3D11_SHADER_VERSION_TYPE type)
{
	switch (type)
	{
	case D3D11_SHVER_PIXEL_SHADER:    return "D3D11_SHVER_PIXEL_SHADER"; 
	case D3D11_SHVER_VERTEX_SHADER:   return "D3D11_SHVER_VERTEX_SHADER"; 
	case D3D11_SHVER_GEOMETRY_SHADER: return "D3D11_SHVER_GEOMETRY_SHADER"; 
	case D3D11_SHVER_HULL_SHADER:     return "D3D11_SHVER_HULL_SHADER"; 
	case D3D11_SHVER_DOMAIN_SHADER:   return "D3D11_SHVER_DOMAIN_SHADER"; 
	case D3D11_SHVER_COMPUTE_SHADER:  return "D3D11_SHVER_COMPUTE_SHADER"; 
	case D3D11_SHVER_RESERVED0:		  return "D3D11_SHVER_RESERVED0"; 
	default:						  return "default, (unknown, outside of enum)"; 
	}
}

/* ----------  Compile From File ----------*/

HRESULT CompileShaderFromFile(const wchar_t* szFileName, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	#if defined( DEBUG ) || defined( _DEBUG )
		dwShaderFlags |= D3DCOMPILE_DEBUG;
	#endif


	ComPtr<ID3DBlob> pErrorBlob = nullptr;
	hr = D3DCompileFromFile(
		szFileName,				// LPCWSTR pFileName,
		nullptr,				// D3D_SHADER_MACRO* pDefines,
		nullptr,				// ID3DInclude* pInclude,
		szEntryPoint,			// LPCSTR pEntrypoint, 
		szShaderModel,			// LPCSTR pTarget,
		dwShaderFlags,			// UINT Flags1,
		0,						// UINT Flags2,
		ppBlobOut,				// ID3DBlob** ppCode,
		&pErrorBlob				// ID3DBlob** ppErrorMsgs
	);

	ASSERT_DEBUG(SUCCEEDED(hr), 
		"Failed to compile shader\n"
		"Shader Filename: %ls\n"
		"Error Message: %s", 
		szFileName, 
		(char*)pErrorBlob->GetBufferPointer()
	);

	return S_OK;
}


/* ---------- Create Shader Object ----------*/

VertexShader*   ShaderFactory::_CreateVertexShader(ID3DBlob* blob)
{

	ID3D11VertexShader* vertexShader;
	HRESULT hr = m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&vertexShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Vertex Shader");

	return new VertexShader(vertexShader);
}

PixelShader*    ShaderFactory::_CreatePixelShader(ID3DBlob* blob)
{
	ID3D11PixelShader* pixelShader;
	HRESULT hr = m_device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&pixelShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Pixel Shader");

	return new PixelShader(pixelShader);
}

GeometryShader* ShaderFactory::_CreateGeometryShader(ID3DBlob* blob)
{
	ID3D11GeometryShader* geometryShader;
	HRESULT hr = m_device->CreateGeometryShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&geometryShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Geometry Shader");

	return new GeometryShader(geometryShader);
}

ComputeShader*  ShaderFactory::_CreateComputeShader(ID3DBlob* blob)
{
	ID3D11ComputeShader* computeShader;
	HRESULT hr = m_device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&computeShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Compute Shader");

	return new ComputeShader(computeShader);
}


/* ------------------------------  PUBLIC API ------------------------------*/


Shader* ShaderFactory::BuildSourceShader(std::string shaderFilename, std::string EntryPoint, std::string ShaderModel)
{
	// Assert the file is an .hlsl file
	ASSERT_WARNING(
		String::Ends_With(shaderFilename, ".hlsl"), 
		"Building source shader with filetype %s. Are you sure that's the right format?", 
		String::SubstringAfter(shaderFilename, ".").c_str()
	);

	std::wstring w_shaderFilename(shaderFilename.begin(), shaderFilename.end());

	// Compile the Shader
	ComPtr<ID3DBlob> shaderBlob = nullptr;
	HRESULT hr = CompileShaderFromFile(w_shaderFilename.c_str(), EntryPoint.c_str(), ShaderModel.c_str(), &shaderBlob);
	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Compile Shader");

	Shader* newShader = nullptr;
	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_VERSION_TYPE shaderType = Convert_ShaderModel_ShaderTypeEnum(ShaderModel);
		newShader = _BuildShaderObject(shaderBlob, shaderFilename, shaderType);
	}

	return newShader;
}

Shader* ShaderFactory::BuildCompiledShader(std::string shaderFilename, D3D11_SHADER_VERSION_TYPE shaderType)
{
	// Assert the file is an .cso file
	ASSERT_WARNING(
		String::Ends_With(shaderFilename, ".cso"),
		"Building source shader with filetype %s. Are you sure that's the right format?",
		String::SubstringAfter(shaderFilename, ".").c_str()
	);

	std::wstring w_shaderFilename(shaderFilename.begin(), shaderFilename.end());

	// Load a Compiled Shader
	ComPtr<ID3DBlob> shaderBlob = nullptr;
	HRESULT hr = D3DReadFileToBlob(w_shaderFilename.c_str(), &shaderBlob);
	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Load Compiled Shader.\n    Shader File Name: %s", shaderFilename.c_str());

	Shader* newShader = nullptr;
	if (SUCCEEDED(hr))
	{
		newShader = _BuildShaderObject(shaderBlob, shaderFilename, shaderType);
	}

	return newShader;
}

Shader* ShaderFactory::_BuildShaderObject(ComPtr<ID3DBlob> shaderBlob, std::string shaderFilename, D3D11_SHADER_VERSION_TYPE shaderType)
{
	Shader* newShader = nullptr;

	switch (shaderType)
	{
	case D3D11_SHVER_PIXEL_SHADER:
		newShader = _CreatePixelShader(shaderBlob.Get());
		break;
	case D3D11_SHVER_VERTEX_SHADER:
		newShader = _CreateVertexShader(shaderBlob.Get());
		break;
	case D3D11_SHVER_GEOMETRY_SHADER:
		newShader = _CreateGeometryShader(shaderBlob.Get());
		break;
	case D3D11_SHVER_COMPUTE_SHADER:
		newShader = _CreateComputeShader(shaderBlob.Get());
		break;
	case D3D11_SHVER_DOMAIN_SHADER:
	case D3D11_SHVER_HULL_SHADER:
	case D3D11_SHVER_RESERVED0:
	default:
		ASSERT_DEBUG(false, "Shader type is not supported. Type is %d(%s)", shaderType, Convert_ShaderTypeEnum_String(shaderType));
		break;
	}

	ASSERT_DEBUG(newShader != nullptr, "Failed to Create Shader Object.");
	if (newShader != nullptr)
	{
		ID3D11ShaderReflection* pReflector = NULL;
		D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
		newShader->SetShaderInfo(BuildShaderReflectionData(pReflector));
	}

	return newShader;
}
