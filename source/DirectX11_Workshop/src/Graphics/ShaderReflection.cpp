#include <d3dcompiler.h>
#include "Graphics\ComPtr.h"
#include "Graphics\ShaderReflection.h"

// ------------------ Lookup Tables ------------------
// Maps an [integer value] -> number of 1's (bits set) in that integer.
unsigned int MaskToNumBitsSet[16] =
{
	0,						   // 0 set bits
	1, 1,                      // [1, 2], 1 set bit
	2, 2, 2, 2,                // [3, 6], 2 set bits
	3, 3, 3, 3, 3, 3, 3, 3,    // [7, 14], 3 set bits
	4                          // 15, 4 set bits (max unsigned char)
};

// Maps the [Number of ones in Mask][D3D_REGISTER_COMPONENT_TYPE] -> DXGI_FORMAT 
DXGI_FORMAT DXGI_Format_LookupTable[4][4] =
{
	{ DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_UINT         , DXGI_FORMAT_R32_SINT         , DXGI_FORMAT_R32_FLOAT },
	{ DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32_UINT      , DXGI_FORMAT_R32G32_SINT      , DXGI_FORMAT_R32G32_FLOAT },
	{ DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32_UINT   , DXGI_FORMAT_R32G32B32_SINT   , DXGI_FORMAT_R32G32B32_FLOAT },
	{ DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_FLOAT },
};

ShaderInfo BuildShaderReflectionData(ID3D11ShaderReflection* refData)
{
	HRESULT hr = S_OK;
	ShaderInfo shaderInfo;

	// Constant Buffer and Input Layout Parameter counts.
	D3D11_SHADER_DESC shaderDesc;
	hr = refData->GetDesc(&shaderDesc);
	if (SUCCEEDED(hr))
	{
		shaderInfo.ConstantBufferCount = shaderDesc.ConstantBuffers;
		shaderInfo.InputParameterCount = shaderDesc.InputParameters;
		shaderInfo.BoundResourceCount  = shaderDesc.BoundResources;
	}

	// Minimum Feature Level
	D3D_FEATURE_LEVEL level;
	refData->GetMinFeatureLevel(&level);
	shaderInfo.MinFeatureLevel = level;
	
	//---- Texture Samplers ----
	for (unsigned i = 0; i < shaderInfo.BoundResourceCount; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceBindingDesc;
		refData->GetResourceBindingDesc(i, &resourceBindingDesc);
		
		ResourceInfo resourceInfo;
		std::memcpy(resourceInfo.Name, resourceBindingDesc.Name, strlen(resourceBindingDesc.Name) + 1);
		resourceInfo.Type       = resourceBindingDesc.Type;
		resourceInfo.BindPoint  = resourceBindingDesc.BindPoint;
		resourceInfo.BindCount  = resourceBindingDesc.BindCount;
		resourceInfo.uFlags     = resourceBindingDesc.uFlags;
		resourceInfo.ReturnType = resourceBindingDesc.ReturnType;
		resourceInfo.Dimension  = resourceBindingDesc.Dimension;
		resourceInfo.NumSamples = resourceBindingDesc.NumSamples;
	}
	

	//---- Input Layout ----
	
	unsigned int byteOffset = 0;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (unsigned i = 0; i < shaderInfo.InputParameterCount; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		hr = refData->GetInputParameterDesc(i, &paramDesc);
		ASSERT_DEBUG(SUCCEEDED(hr), "Failed to get input parameter description. Input Parameter index: %d", i);
		if (FAILED(hr))
		{
			continue;
		}

		// create input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = byteOffset;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format and Byte Offset
		unsigned numOfRegistersUsed = MaskToNumBitsSet[paramDesc.Mask];
		elementDesc.Format = DXGI_Format_LookupTable[numOfRegistersUsed-1][paramDesc.ComponentType];
		byteOffset += numOfRegistersUsed * 4;

		inputLayoutDesc.push_back(elementDesc);
	}
	shaderInfo.InputLayoutData = std::move(inputLayoutDesc);

	// ---- Samplers ----

	// ---- Constant Buffer ----
	for (unsigned i = 0; i < shaderInfo.ConstantBufferCount; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* refConBuffer = refData->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		refConBuffer->GetDesc(&bufferDesc);

		shaderInfo.ConstantBuffers.emplace_back();
		ConstantBufferInfo& bufferInfo = shaderInfo.ConstantBuffers.back();
		std::memcpy(bufferInfo.Name, bufferDesc.Name, strlen(bufferDesc.Name) + 1);
		bufferInfo.Size = bufferDesc.Size;
		bufferInfo.Type = bufferDesc.Type;
		bufferInfo.uFlags = bufferDesc.uFlags;
		bufferInfo.VariableCount = bufferDesc.Variables;

		// ---- Constant Buffer Variables ----
		for (unsigned i = 0; i < bufferInfo.VariableCount; ++i)
		{
			ID3D11ShaderReflectionVariable* refConBufVariable = refConBuffer->GetVariableByIndex(i);
			D3D11_SHADER_VARIABLE_DESC variableDesc;
			refConBufVariable->GetDesc(&variableDesc);

			bufferInfo.variables.emplace_back();
			VariableInfo& varInfo = bufferInfo.variables.back();
			std::memcpy(varInfo.Name, variableDesc.Name, strlen(variableDesc.Name) + 1);
			variableDesc.StartOffset = variableDesc.StartOffset;
			variableDesc.Size = variableDesc.Size;
			variableDesc.uFlags = variableDesc.uFlags;
			variableDesc.StartTexture = variableDesc.StartTexture;
			variableDesc.TextureSize = variableDesc.TextureSize;
			variableDesc.StartSampler = variableDesc.StartSampler;
			variableDesc.SamplerSize = variableDesc.SamplerSize;


			// Get the variable type name.
			ID3D11ShaderReflectionType* reflectionType = refConBufVariable->GetType();
			D3D11_SHADER_TYPE_DESC typeDesc;
			reflectionType->GetDesc(&typeDesc);
			std::memcpy(varInfo.TypeName, typeDesc.Name, strlen(typeDesc.Name) + 1);
			varInfo.Type    = typeDesc.Type;
			varInfo.Rows    = typeDesc.Rows;
			varInfo.Columns = typeDesc.Columns;
			varInfo.Class = typeDesc.Class;
			varInfo.ElementCount = typeDesc.Elements;
			varInfo.MemberCount = typeDesc.Members;
			
		}// end Variables
	}// end Constant Buffer

	return std::move(shaderInfo);
}
