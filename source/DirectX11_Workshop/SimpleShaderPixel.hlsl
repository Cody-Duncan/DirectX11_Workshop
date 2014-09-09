//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D txDiffuse    : register(t0);
Texture2D txOverlay    : register(t1);
Texture2D txMask       : register(t2);
Texture2D txMaskDiffuse: register(t3);

SamplerState samLinear : register(s0);

cbuffer textureEffectBuffer : register(b0)
{
	int useMask;
	float maskIntensity;
}

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
	float4 overlay = txOverlay.Sample(samLinear, vsOutput.TexCoord);
	float4 maskColor = float4(0,0,0,0);
	if(useMask)
	{
		float4 mask = txMask.Sample(samLinear, vsOutput.TexCoord);
		if(mask.x > 0)
		{
			float4 maskUnderColor = txMaskDiffuse.Sample(samLinear, vsOutput.Pos.xy / (float2(1280,720)/4));
			maskColor = mask * maskUnderColor * maskIntensity;
			return lerp(txDiffuse.Sample(samLinear, vsOutput.TexCoord), mask * maskUnderColor, maskIntensity);
		}
	}
	float2 texCoord = vsOutput.TexCoord + sin(frequency*time/1000.0f)/10;
	texCoord.x += sin((frequency+time)/2000.0f)/10;

	return lerp(txDiffuse.Sample(samLinear, texCoord), overlay, overlay.w);
}