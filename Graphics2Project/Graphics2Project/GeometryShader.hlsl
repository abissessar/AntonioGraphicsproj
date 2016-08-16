
struct GSOutput
{
	float4 color : COLOR;
	float4 posH : SV_POSITION;
};

[maxvertexcount(3)] // max vertex data to be output (limit 1024 total scalars)
void main(line float4 input[2] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	//// red green and blue vertex
	//GSOutput verts[3] =
	//{
	//	float4(1,0,0,1), float4(0,0,0,1),
	//	float4(0,1,0,1), float4(0,0,0,1),
	//	float4(0,0,1,1), float4(0,0,0,1)
	//};
	//// bottom left
	//verts[0].posH.xyz = input[0].xyz;
	//verts[0].posH.x -= 0.5f;
	//// bottom right
	//verts[2].posH = verts[0].posH;
	//verts[2].posH.x += 1.0f;
	//// top center
	//verts[1].posH.xyz = input[1].xyz;
	//// prep triangle for rasterization
	//float4x4 mVP = mul(mView, mProj);
	//for (uint i = 0; i < 3; ++i)
	//	verts[i].posH = mul(verts[i].posH, mVP);
	//// send verts to the rasterizer
	//output.Append(verts[0]);
	//output.Append(verts[1]);
	//output.Append(verts[2]);
	//// do not connect to other triangles
	//output.RestartStrip();
}
