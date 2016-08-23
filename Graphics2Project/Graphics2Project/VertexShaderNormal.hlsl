// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	//float3 color : COLOR0;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float3 wpos : WPOSITION;
	float3 color : COLOR0;
	float4 tan : TAN0;
	float4 biTan : BITAN0;

};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	float3 norm = input.normal;
	float4 tan = float4(input.tangent.xyz, 0.0f);
	float4 bitan;
	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	output.wpos = pos.xyz;
	tan = mul(tan, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	bitan.xyz = cross(norm.xyz, tan.xyz);
	bitan.w = 0.0f;
	output.pos = pos;
	output.normal = mul(norm, model);
	output.tan = tan;
	output.biTan = bitan;
	// Pass the color through without modification.
	//output.color = input.color;
	//output.normal = normalize(.normal);
	output.uv = input.uv;

	return output;
}