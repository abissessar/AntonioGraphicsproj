#pragma once

namespace Graphics2Project
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};
	struct VertexPositionColorNormal
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 normal;
	};
	struct DirectionalLightBuffer
	{
		DirectX::XMFLOAT4 directional_color;
		DirectX::XMFLOAT4 directional_dir;
	};
	struct PointLightBuffer
	{
		DirectX::XMFLOAT4 point_color;
		DirectX::XMFLOAT4 point_pos;
		DirectX::XMFLOAT4 point_radius;
	};
	struct SpotLightBuffer
	{
		DirectX::XMFLOAT4 Spot_color;
		DirectX::XMFLOAT4 Spot_pos;
		DirectX::XMFLOAT4 Spot_dir;
		DirectX::XMFLOAT4 Spot_radius;
	};
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		//DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 norm;
	};
}