
//--------------------------------------------------------------------------------------
// s
//--------------------------------------------------------------------------------------
cbuffer cbNeverChanges : register(b0)
{
	matrix Projection;
};

cbuffer cbPerFrame : register(b1)
{
	matrix View;
};

cbuffer cbPerObject : register(b2)
{
	matrix World;
};

cbuffer cbPerAnim : register(b3)
{
	matrix BoneMatrixArray[100];
};

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 texCoord		: TEXCOORD0;
	float4 boneWeights	: BONEWEIGHTS;
	uint4  boneIndices	: BONEINDICES;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT main(VS_INPUT input)
{
	float4 inputPos = float4(input.pos, 1.0);
	float4 inputNormal = float4(input.normal, 0.0);

	// Animation skinning start
	float4 posAfterSkinning = float4(0.0, 0.0, 0.0, 0.0);
	float4 normalAfterSkinning = float4(0.0, 0.0, 0.0, 0.0);

	for(int boneCount = 0; boneCount < 4; ++boneCount)
	{		
		float4 tempPos;
		float4 tempNormal;

		int boneIndex = input.boneIndices[boneCount];

		// Assumed to already include model to bone space transform
		matrix boneMatrix = BoneMatrixArray[boneIndex];

		// (x, y, z, 1) Homogeneous transform
		tempPos = mul(inputPos, boneMatrix);
		tempPos *= input.boneWeights[boneCount];
		posAfterSkinning += tempPos;

		// (x, y, z, 0) Normal transform remove translation(technically the matrix should be MatrixTranspose(MatrixInverse(tn)) but
		// since we are using only uniform scaling this is equivalent)
		tempNormal = mul(inputNormal, boneMatrix);
		tempNormal *= input.boneWeights[boneCount];
		normalAfterSkinning += tempNormal;
	}

	// If skinning had no effect
	if (posAfterSkinning.x == 0.0 && posAfterSkinning.y == 0.0 && posAfterSkinning.z == 0.0 && posAfterSkinning.w == 0.0)
	{
		// Reset to initial values
		posAfterSkinning = inputPos;
		normalAfterSkinning = inputNormal;
	}
				
	// Animation skinning end
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(posAfterSkinning, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.TexCoord = input.texCoord;

	/*VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = mul(inputPos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.TexCoord = input.texCoord;*/

	return output;
}