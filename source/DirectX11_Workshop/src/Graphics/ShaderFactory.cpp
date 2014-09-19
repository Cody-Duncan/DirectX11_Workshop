#include "Graphics\ShaderFactory.h"
#include "String\StringAlgorithm.h"
#include "ComError.h"
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
		"Shader Filename: %ls\n\n"
		"Error Message: %s\n", 
		szFileName, 
		(char*)pErrorBlob->GetBufferPointer()
	);

	return S_OK;
}

/* ----------  Build Input Layout ----------*/

//Function Creates an input layout from the vertex shader, after compilation.
//Input layout can be reused with any vertex shaders that use the same input layout.
HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
{
	// Reflect shader info
	ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
	HRESULT hr = S_OK;
	if (FAILED(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
	{
		return S_FALSE;
	}
	// get shader description
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);
	// Read input layout description from shader info
	unsigned int byteOffset = 0;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (unsigned int i = 0; i< shaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);
		// create input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = byteOffset;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;
		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			byteOffset += 4;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			byteOffset += 8;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			byteOffset += 12;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			byteOffset += 16;
		}
		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}
	// Try to create Input Layout
	hr = pD3DDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

	pVertexShaderReflection->Release();
	return hr;
}

D3D11_INPUT_ELEMENT_DESC defaultLayout[] =
{
	{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
unsigned int defaultLayoutSize = sizeof(defaultLayout) / sizeof(defaultLayout[0]);

HRESULT CreateInputLayoutDefault(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
{
	HRESULT hr = pD3DDevice->CreateInputLayout(defaultLayout, defaultLayoutSize, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);
	if (FAILED(hr))
	{
		const wchar_t* result = HresultGetMessage(hr);
		ASSERT_DEBUG(SUCCEEDED(hr), "%ls", result);
	}
	return hr;
}


/* ---------- Create Shader Object ----------*/

Shader*   ShaderFactory::_CreateVertexShader(ID3DBlob* blob)
{
	// -- Create Vertex Shader --
	ID3D11VertexShader* vertexShader;
	HRESULT hr = m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&vertexShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Vertex Shader");

	m_vertexShaders.emplace_back(vertexShader);
	Shader& newShader = m_vertexShaders.back();


	// -- Create Input Layout --
	ID3D11InputLayout* newInputLayout;
	hr = CreateInputLayoutDefault(blob, m_device, &newInputLayout);
	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to build the input layout");
	m_inputLayouts.emplace_back(newInputLayout);
	newShader.m_inputLayout = m_inputLayouts.back().Get();

	return &newShader;
}

Shader*    ShaderFactory::_CreatePixelShader(ID3DBlob* blob)
{
	ID3D11PixelShader* pixelShader;
	HRESULT hr = m_device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&pixelShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Pixel Shader");

	m_pixelShaders.emplace_back(pixelShader);
	return &m_pixelShaders.back();
}

Shader* ShaderFactory::_CreateGeometryShader(ID3DBlob* blob)
{
	ID3D11GeometryShader* geometryShader;
	HRESULT hr = m_device->CreateGeometryShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&geometryShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Geometry Shader");

	m_geometryShaders.emplace_back(geometryShader);
	return &m_geometryShaders.back();
}

Shader*  ShaderFactory::_CreateComputeShader(ID3DBlob* blob)
{
	ID3D11ComputeShader* computeShader;
	HRESULT hr = m_device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&computeShader
	);

	ASSERT_DEBUG(SUCCEEDED(hr), "Failed to Create Compute Shader");

	m_computeShaders.emplace_back(computeShader);
	return &m_computeShaders.back();
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

	return newShader;
}
