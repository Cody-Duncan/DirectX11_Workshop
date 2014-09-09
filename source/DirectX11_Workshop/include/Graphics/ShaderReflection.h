#pragma once

struct ID3D11ShaderReflection;
#include <d3dcommon.h>

struct ResourceInfo
{
	char                        Name[64];       // Name of the resource
	D3D_SHADER_INPUT_TYPE       Type;           // Type of resource (e.g. texture, cbuffer, etc.)
	unsigned int                BindPoint;      // Starting bind point (register). A BindPoint 0 for a texture means it is bound to register t0.
	unsigned int                BindCount;      // Number of contiguous bind points (for arrays)

	unsigned int                uFlags;         // Input binding flags
	D3D_RESOURCE_RETURN_TYPE    ReturnType;     // Return type (if texture)
	D3D_SRV_DIMENSION           Dimension;      // Dimension (if texture)
	unsigned int                NumSamples;     // Number of samples (0 if not MultiSampled texture)
};

struct VariableInfo
{
	char            Name[64];       // Name of the variable
	char            TypeName[64];   // Name of the variable's type.
	unsigned int    StartOffset;    // Offset in constant buffer's backing store
	unsigned int    Size;           // Size of variable (in bytes)
	unsigned int    uFlags;         // Variable flags

	unsigned int    StartTexture;   // First texture index (or -1 if no textures used)
	unsigned int    TextureSize;    // Number of texture slots possibly used.

	unsigned int    StartSampler;   // First sampler index (or -1 if no textures used)
	unsigned int    SamplerSize;    // Number of sampler slots possibly used.

	D3D_SHADER_VARIABLE_CLASS Class; // The variable's class . Scalar, Vector, Matrix, Object, Struct.
	D3D_SHADER_VARIABLE_TYPE  Type;  // Variable Type. Much better specifier of the particular type.
	unsigned int Rows;				 // (if matrix) number of rows.
	unsigned int Columns;            // (if matrix) number of columns.
	unsigned int ElementCount;       // (if array) number of elements contained.
	unsigned int MemberCount;        // (if struct/class) number of members.
};


struct ConstantBufferInfo
{
	char                Name[64];       // Name of the constant buffer
	D3D_CBUFFER_TYPE    Type;           // Indicates type of buffer content
	unsigned            VariableCount;  // Number of member variables
	unsigned            Size;           // Size of CB (in bytes)
	unsigned            uFlags;         // Buffer description flags

	std::vector<VariableInfo> variables;
};

struct ShaderInfo
{
	unsigned int ConstantBufferCount;		// Number of Constant Buffers
	unsigned int InputParameterCount;		// Number of Input Parameters (number of elements in input layout)
	unsigned int BoundResourceCount;		// Number of Resources (Global Variables) on shader.
	D3D_FEATURE_LEVEL MinFeatureLevel;		// Minimum DirectX feature level supported by this shader.

	std::vector<ResourceInfo> ResourceInfo;					// Information about the Resources (Global Variables) on the shader.
	std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutData;  // Information about the Input Parameters, used to build Input Layout.
	std::vector<ConstantBufferInfo> ConstantBuffers;        // Information about the Constant Buffers
};


ShaderInfo BuildShaderReflectionData(ID3D11ShaderReflection* refData);

