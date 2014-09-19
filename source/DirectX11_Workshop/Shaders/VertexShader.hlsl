
//--------------------------------------------------------------------------------------
// s
//--------------------------------------------------------------------------------------
cbuffer cbNeverChanges : register(b0)
{
	matrix WorldViewProjection;
};

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 texCoord		: TEXCOORD0;
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
	float4 inputPos    = float4(input.pos   , 1.0);
	float4 inputNormal = float4(input.normal, 0.0);

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(inputPos, WorldViewProjection);
	output.TexCoord = input.texCoord;

	return output;
}