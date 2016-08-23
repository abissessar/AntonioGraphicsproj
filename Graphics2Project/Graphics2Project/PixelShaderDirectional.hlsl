Texture2D shadertexture : register(t0);
SamplerState SampleType : register(S0);

cbuffer LightBuffer : register(b0)
{
	float4 DirectionalColor;
	float3 lightDirection;
};
cbuffer PointBuffer : register(b1)
{
	float4 PointColor;
	float4 Point_pos;
	float4 Point_radius;
};
cbuffer SpotBuffer : register(b2)
{
	float4 SpotColor;
	float4 Spot_pos;
	float4 Spot_Dir;
	float4 Spot_radius;
}

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float3 wpos : WPOSITION;
	float3 color : COLOR0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 tex_color = shadertexture.Sample(SampleType, input.uv);
	
	float r = saturate(dot(normalize(-lightDirection.xyz), input.normal));
	float4 color1 = r*DirectionalColor;

	float3 dir = normalize(Point_pos.xyz - input.wpos.xyz);
	//r = saturate(dot(dir.xyz, input.normal));
	float attenuationFO = 1.0f - saturate(length(Point_pos - input.wpos.xyz) / Point_radius.x);
	float4 color2 = attenuationFO*PointColor;


	dir = normalize(Spot_pos.xyz - input.wpos.xyz);
	r = saturate(dot(-dir.xyz, normalize(Spot_Dir.xyz)));
	//float spotfact = (r > Spot_radius.z) ? 1 : 0;
	attenuationFO = 1.0f - saturate(length(Spot_pos - input.wpos.xyz) / Spot_radius.x);
	float attenuationCE = 1.0 - saturate((Spot_radius.y - r) / (Spot_radius.y - Spot_radius.z));
	float FinalAtten = attenuationFO*attenuationCE;
	float Lightr = saturate(dot(dir.xyz, input.normal));
	float4 color3 = FinalAtten*Lightr*SpotColor;


	float4 color = color1 + color2 + color3;

	color = saturate(color);
	color = color *tex_color;
	color.w = tex_color.w;
return float4(color);
}