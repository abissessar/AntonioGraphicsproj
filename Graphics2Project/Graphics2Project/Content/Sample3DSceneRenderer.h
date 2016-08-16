#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <DirectXMath.h>
#include <vector>
#include "Model.h"
using namespace DirectX;

namespace Graphics2Project
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		bool	m_floorloaded;

		XMFLOAT4X4 world, camera, proj;

		//////////////////////////////////
		

		//void objload(std::vector<Vertex>& _verts, std::vector<unsigned>& _index, const char* filename);
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBufferfloor;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBufferfloor;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayoutfloor;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShaderfloor;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBufferDirlight;
		DirectionalLightBuffer						m_constantBufferDirlightData;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShaderDirlight;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBufferPointlight;
		PointLightBuffer						m_constantBufferPointlightData;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBufferSpotlight;
		SpotLightBuffer						m_constantBufferSpotlightData;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBufferobj;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBufferobj;

		std::vector<Model> model;
		std::vector<Model> skybox;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_state;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_clockwise;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_counterclockwise;

		//////////////////////////////////
	};
}

