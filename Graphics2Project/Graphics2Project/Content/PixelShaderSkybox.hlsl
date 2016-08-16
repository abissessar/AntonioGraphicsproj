// Per-pixel color data passed through the pixel shader.
TextureCube shadertexture : register(t0);
SamplerState SampleType : register(S0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 uvw: UVW;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 tex_color = shadertexture.Sample(SampleType, input.uvw);

	return tex_color;
}