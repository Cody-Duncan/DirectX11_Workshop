//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D txDiffuse    : register(t0);
SamplerState samLinear : register(s0);

cbuffer textureSlideBuffer : register(b1)
{
	float time;
	float frequency;
}

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT vsOutput) : SV_Target
{
	float2 texCoord = vsOutput.TexCoord;
	return txDiffuse.Sample(samLinear, texCoord);
}