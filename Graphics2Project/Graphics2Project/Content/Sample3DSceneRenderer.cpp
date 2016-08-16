#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace Graphics2Project;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	static const XMVECTORF32 eye = { 0.0f, 0.0f, -1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&camera, XMMatrixInverse(0, XMMatrixLookAtLH(eye, at, up)));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	m_constantBufferDirlightData.directional_dir = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_constantBufferDirlightData.directional_color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	m_constantBufferPointlightData.point_color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_constantBufferPointlightData.point_pos = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_constantBufferPointlightData.point_radius = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);

	m_constantBufferSpotlightData.Spot_color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	m_constantBufferSpotlightData.Spot_dir = XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	m_constantBufferSpotlightData.Spot_pos = XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f);
	m_constantBufferSpotlightData.Spot_radius = XMFLOAT4(20.0f, 0.85f, 0.8, 0.0f);
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.

}

using namespace Windows::UI::Core;
extern CoreWindow^ gwindow;
#include <atomic>
extern bool mouse_move;
extern float diffx;
extern float diffy;
extern bool w_down;
extern bool a_down;
extern bool s_down;
extern bool d_down;
extern bool left_click;

extern char buttons[256];
// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{

	//matrix = XMMatrixTranslation(1, 1, 1);


	XMMATRIX potato = XMMatrixIdentity();
	skybox[0].WM = potato;
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	XMMATRIX newcamera = XMLoadFloat4x4(&camera);

	if (buttons['W'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * timer.GetElapsedSeconds() * 5.0;
	}

	if (a_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * -timer.GetElapsedSeconds() *5.0;
	}

	if (s_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * -timer.GetElapsedSeconds() * 5.0;
	}

	if (d_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * timer.GetElapsedSeconds() * 5.0;
	}

	Windows::UI::Input::PointerPoint^ point = nullptr;

	//if(mouse_move)/*This crashes unless a mouse event actually happened*/
	//point = Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerID);
	//XMStoreFloat4x4(&m_constantBufferData.model, potato);
	//potato = newcamera;
	if (mouse_move)
	{
		// Updates the application state once per frame.
		if (left_click)
		{
			XMVECTOR pos = newcamera.r[3];
			newcamera.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
			newcamera = XMMatrixRotationX(-diffy*0.01f) * newcamera * XMMatrixRotationY(-diffx*0.01f);
			newcamera.r[3] = pos;
		}
	}
	skybox[0].WM.r[3] = newcamera.r[3];
	XMStoreFloat4x4(&camera, newcamera);

	/*Be sure to inverse the camera & Transpose because they don't use pragma pack row major in shaders*/
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));

	mouse_move = false;/*Reset*/
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	XMMATRIX potato3 = XMMatrixTranspose(skybox[0].WM);
	XMStoreFloat4x4(&m_constantBufferData.model, potato3);
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	context->UpdateSubresource1(
		m_constantBufferDirlight.Get(),
		0,
		NULL,
		&m_constantBufferDirlightData,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_constantBufferPointlight.Get(),
		0,
		NULL,
		&m_constantBufferPointlightData,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_constantBufferSpotlight.Get(),
		0,
		NULL,
		&m_constantBufferSpotlightData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;
	//context->IASetVertexBuffers(
	//	0,
	//	1,
	//	m_vertexBufferobj.GetAddressOf(),
	//	&stride,
	//	&offset
	//);

	//context->IASetIndexBuffer(
	//	m_indexBufferobj.Get(),
	//	DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
	//	0
	//);
	skybox[0].SetBuffer(context);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayoutfloor.Get());

	context->PSSetShaderResources(0, 1, skybox[0].m_SRV.GetAddressOf());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);


	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->PSSetConstantBuffers1(
		0,
		1,
		m_constantBufferDirlight.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->PSSetConstantBuffers1(
		1,
		1,
		m_constantBufferPointlight.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->PSSetConstantBuffers1(
		2,
		1,
		m_constantBufferSpotlight.GetAddressOf(),
		nullptr,
		nullptr
	);
	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetSamplers(
		0,
		1,
		m_state.GetAddressOf()
	);

	// Draw the objects.
	context->RSSetState(m_counterclockwise.Get());
	skybox[0].Drawindex(context);
	context->RSSetState(m_clockwise.Get());

	// clear depth stencil view here
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->VSSetShader(
		m_vertexShaderfloor.Get(),
		nullptr,
		0
	);
	context->PSSetShader(
		m_pixelShaderDirlight.Get(),
		nullptr,
		0
	);
	for (int i = 0; i < model.size(); i++)
	{
		XMMATRIX potato2 = XMMatrixTranspose(model[i].WM);
		XMStoreFloat4x4(&m_constantBufferData.model, potato2);
		context->UpdateSubresource1(
			m_constantBuffer.Get(),
			0,
			NULL,
			&m_constantBufferData,
			0,
			0,
			0
		);
		model[i].SetBuffer(context);
		context->PSSetShaderResources(0, 1, model[i].m_SRV.GetAddressOf());
		context->VSSetConstantBuffers1(
			0,
			1,
			m_constantBuffer.GetAddressOf(),
			nullptr,
			nullptr
		);
		model[i].Drawindex(context);
	}

	/*XMMATRIX potato1 = XMMatrixTranspose(model[0].WM);
	XMStoreFloat4x4(&m_constantBufferData.model, potato1);
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);
	model[0].SetBuffer(context);
	context->PSSetShaderResources(0, 1, model[0].m_SRV.GetAddressOf());
	context->VSSetShader(
		m_vertexShaderfloor.Get(),
		nullptr,
		0
	);
	context->PSSetShader(
		m_pixelShaderDirlight.Get(),
		nullptr,
		0
	);
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	model[1].Drawindex(context);



	XMMATRIX potato3 = XMMatrixTranspose(model[2].WM);
	XMStoreFloat4x4(&m_constantBufferData.model, potato3);
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);
	model[2].SetBuffer(context);
	context->PSSetShaderResources(0, 1, model[2].m_SRV.GetAddressOf());
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	model[2].Drawindex(context);*/

	////Start floor
	//UINT stride = sizeof(Model::Vertex);
	//UINT offset = 0;
	//context->IASetVertexBuffers(
	//	0,
	//	1,
	//	m_vertexBufferfloor.GetAddressOf(),
	//	&stride,
	//	&offset
	//);
	////CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"checkers.dds", NULL, &m_SRV);
	//context->PSSetShaderResources(0, 1, model[0].m_SRV.GetAddressOf());


	//context->IASetIndexBuffer(
	//	m_indexBufferfloor.Get(),
	//	DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
	//	0
	//);
	////context->PSSetShaderResources(0, 1, 0);

	//context->DrawIndexed(
	//	m_indexCount,
	//	0,
	//	0
	//);
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{

	//objload(IL_verts, IL_index, "Goomba.obj");
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"VertexShaderSkybox.cso");
	auto loadPSTask = DX::ReadDataAsync(L"PixelShaderSkybox.cso");
	auto loadVSTaskFloor = DX::ReadDataAsync(L"VertexShaderFloor.cso");
	auto loadPSTaskLight = DX::ReadDataAsync(L"PixelShaderDirectional.cso");
	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);
		static const D3D11_INPUT_ELEMENT_DESC vertexDescfloor[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDescfloor,
				ARRAYSIZE(vertexDescfloor),
				&fileData[0],
				fileData.size(),
				&m_inputLayoutfloor
			)
		);
	});
	auto createVSTaskFloor = loadVSTaskFloor.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShaderfloor
			)
		);
		static const D3D11_INPUT_ELEMENT_DESC vertexDescfloor[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDescfloor,
				ARRAYSIZE(vertexDescfloor),
				&fileData[0],
				fileData.size(),
				&m_inputLayoutfloor
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTaskLight.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShaderDirlight
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
			)
		);
		CD3D11_BUFFER_DESC constantBufferDirlightDesc(sizeof(DirectionalLightBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDirlightDesc,
				nullptr,
				m_constantBufferDirlight.GetAddressOf()
			)
		);
		CD3D11_BUFFER_DESC constantBufferPointlightDesc(sizeof(PointLightBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferPointlightDesc,
				nullptr,
				m_constantBufferPointlight.GetAddressOf()
			)
		);
		CD3D11_BUFFER_DESC constantBufferSpotlightDesc(sizeof(SpotLightBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferSpotlightDesc,
				nullptr,
				m_constantBufferSpotlight.GetAddressOf()
			)
		);
		CD3D11_SAMPLER_DESC sampler = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
		m_deviceResources->GetD3DDevice()->CreateSamplerState(&sampler, m_state.GetAddressOf());

	});

	auto createPSTaskskybox = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);
		CD3D11_RASTERIZER_DESC counterclockwise = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		counterclockwise.FrontCounterClockwise = true;
		m_deviceResources->GetD3DDevice()->CreateRasterizerState(&counterclockwise, m_counterclockwise.GetAddressOf());

		CD3D11_RASTERIZER_DESC clockwise = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		clockwise.FrontCounterClockwise = false;
		m_deviceResources->GetD3DDevice()->CreateRasterizerState(&clockwise, m_clockwise.GetAddressOf());
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTaskFloor).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		//static const VertexPositionColor cubeVertices[] =
		//{
		//	{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
		//	{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		//	{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		//	{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		//	{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		//	{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
		//	{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
		//	{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
		//};

		//D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		//vertexBufferData.pSysMem = cubeVertices;
		//vertexBufferData.SysMemPitch = 0;
		//vertexBufferData.SysMemSlicePitch = 0;
		//CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		//DX::ThrowIfFailed(
		//	m_deviceResources->GetD3DDevice()->CreateBuffer(
		//		&vertexBufferDesc,
		//		&vertexBufferData,
		//		&m_vertexBuffer
		//	)

		//);

		//// Load mesh indices. Each trio of indices represents
		//// a triangle to be rendered on the screen.
		//// For example: 0,2,1 means that the vertices with indexes
		//// 0, 2 and 1 from the vertex buffer compose the 
		//// first triangle of this mesh.
		//static const unsigned short cubeIndices[] =
		//{
		//	0,2,1, // -x
		//	1,2,3,

		//	4,5,6, // +x
		//	5,7,6,

		//	0,1,5, // -y
		//	0,5,4,

		//	2,6,7, // +y
		//	2,7,3,

		//	0,4,6, // -z
		//	0,6,2,

		//	1,3,7, // +z
		//	1,7,5,
		//};

	/*	m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);*/
		////////////////////////////////////////////////////////////////////////////////////////////
		/*char* modelname = "Cube.obj";
		wchar_t* modeltext = L"SkyboxOcean.dds";
		Model cube(modelname, modeltext, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		model.push_back(cube);*/
		/*char*modelname1 = "Goomba.obj";
		wchar_t*modeltext1 = L"Diffuse_Fuzzy_Corrupt.dds";
		Model goomba(modelname1, modeltext1, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		model.push_back(goomba);*/
		/*char*modelname2 = "Pawn.obj";
		wchar_t*modeltext2 = L"PawnSurface_Color.dds";
		Model chess(modelname2, modeltext2, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		model.push_back(chess);
		char*modelname3 = "Cube.obj";
		wchar_t*modeltext3 = L"Checkers.dds";
		Model floor(modelname3, modeltext3, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		model.push_back(floor);*/


		//static const Model::Vertex floorVertices[] =
		//{
		//	{ XMFLOAT3(-3.0f, -1.0f, 3.0f),XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), },
		//	{ XMFLOAT3(3.0, -1.0f,  3.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), },
		//	{ XMFLOAT3(-3.0f,  -1.0f, -3.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		//	{ XMFLOAT3(3.0f,  -1.0f,  -3.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		//	//{ XMFLOAT3(-0.8f,  -0.8f,  0.8f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		//	//{ XMFLOAT3(-0.8f,  -0.8f,  -0.8f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		//};

		//D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		//vertexBufferData.pSysMem = floorVertices;
		//vertexBufferData.SysMemPitch = 0;
		//vertexBufferData.SysMemSlicePitch = 0;
		//CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(floorVertices), D3D11_BIND_VERTEX_BUFFER);
		//DX::ThrowIfFailed(
		//	m_deviceResources->GetD3DDevice()->CreateBuffer(
		//		&vertexBufferDesc,
		//		&vertexBufferData,
		//		&m_vertexBufferfloor
		//	)
		//);

		//static const unsigned short floorIndices[] =
		//{
		//	0,1,2,
		//	1,3,2

		//};

		//m_indexCount = ARRAYSIZE(floorIndices);

		//D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		//indexBufferData.pSysMem = floorIndices;
		//indexBufferData.SysMemPitch = 0;
		//indexBufferData.SysMemSlicePitch = 0;
		//CD3D11_BUFFER_DESC indexBufferDesc(sizeof(floorIndices), D3D11_BIND_INDEX_BUFFER);
		//DX::ThrowIfFailed(
		//	m_deviceResources->GetD3DDevice()->CreateBuffer(
		//		&indexBufferDesc,
		//		&indexBufferData,
		//		&m_indexBufferfloor
		//	)
		//);
		//////////////////////////////////////////////////////////////////////////////////////////
	});
	auto createSkyTask = (createPSTaskskybox && createVSTask).then([this]() {
		char* modelname = "Cube.obj";
		wchar_t* modeltext = L"SkyboxOcean.dds";
		Model cube(modelname, modeltext, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		skybox.push_back(cube);

	});
	auto createGoombaTask = (createPSTask && createVSTaskFloor).then([this]() {
		char*modelname1 = "Goomba.obj";
		wchar_t*modeltext1 = L"Diffuse_Fuzzy_Corrupt.dds";
		Model goomba(modelname1, modeltext1, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		XMMATRIX matrix;
		matrix = XMMatrixIdentity();
		goomba.WM = matrix;
		
		model.push_back(goomba);
	});
	auto createFloorTask = (createPSTask && createVSTaskFloor).then([this]() {
		char*modelname3 = "Cube.obj";
		wchar_t*modeltext3 = L"Checkers.dds";
		Model floor(modelname3, modeltext3, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		XMMATRIX matrix;
		matrix = XMMatrixIdentity();
		matrix = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
		matrix = XMMatrixMultiply(XMMatrixScaling(5.0f, 0.1, 5.0f), matrix);
		floor.WM = matrix;
		model.push_back(floor);	
	});
	auto createPawnTask = (createPSTask && createVSTaskFloor).then([this]() {
		char*modelname2 = "Pawn.obj";
		wchar_t*modeltext2 = L"PawnSurface_Color.dds";
		Model chess(modelname2, modeltext2, m_deviceResources->GetD3DDevice(), XMFLOAT3(0, 0, 0));
		XMMATRIX matrix;
		matrix = XMMatrixIdentity();
		matrix = XMMatrixTranslation(-10.0f, -0.8f, 0.0f);
		chess.WM = matrix;
		model.push_back(chess);
	});
	// Once the cube is loaded, the object is ready to be rendered.
	(createCubeTask && createFloorTask && createGoombaTask && createSkyTask && createPawnTask).then([this]() {
		m_loadingComplete = true;
	});

}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_constantBufferPointlight.Reset();
	m_constantBufferDirlight.Reset();
	m_constantBufferSpotlight.Reset();
	m_indexBufferfloor.Reset();
	m_indexBufferobj.Reset();
	m_pixelShaderDirlight.Reset();
	m_inputLayoutfloor.Reset();
	m_vertexShaderfloor.Reset();
	m_pixelShaderDirlight.Reset();
	m_state.Reset();
	m_clockwise.Reset();
	m_counterclockwise.Reset();
}
/*	void Sample3DSceneRenderer::objload(std::vector<Vertex>& _verts, std::vector<unsigned>& _index, const char* filename)
	{
		std::vector<XMFLOAT3> tempverts;
		std::vector<XMFLOAT2> tempuv;
		std::vector<XMFLOAT3> tempnorms;
		std::vector<unsigned int> vert_ind, uv_ind, norm_ind;

		FILE* file;
		fopen_s(&file, filename, "r");
		if( file == NULL)
		{
			printf("Impossible to open the file!");
			return;
		}
		while (1)
		{
			char lineHeader[128];
			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break;
			if (strcmp(lineHeader, "v") == 0)
			{
				XMFLOAT3 vert;
				fscanf_s(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
				tempverts.push_back(vert);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				XMFLOAT2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				uv.y = 1 - uv.y;
				tempuv.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				tempnorms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9)
				{
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
				}
				vert_ind.push_back(vertexIndex[0]);
				vert_ind.push_back(vertexIndex[1]);
				vert_ind.push_back(vertexIndex[2]);
				uv_ind.push_back(uvIndex[0]);
				uv_ind.push_back(uvIndex[1]);
				uv_ind.push_back(uvIndex[2]);
				norm_ind.push_back(normalIndex[0]);
				norm_ind.push_back(normalIndex[1]);
				norm_ind.push_back(normalIndex[2]);
			}
		}
		for (unsigned int i = 0; i < vert_ind.size(); ++i)
		{
			Vertex temp;
			unsigned int vertexIndex = vert_ind[i];
			XMFLOAT3 vertex = tempverts[vertexIndex - 1];
			temp.pos = vertex;
			unsigned int uvIndex = uv_ind[i];
			XMFLOAT2 uv = tempuv[uvIndex - 1];
			temp.uv = uv;
			unsigned int normIndex = norm_ind[i];
			XMFLOAT3 normal = tempnorms[normIndex - 1];
			temp.norm = normal;
			_verts.push_back(temp);
			_index.push_back(i);
		}

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = _verts.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex)*_verts.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				m_vertexBufferobj.GetAddressOf()
			)

		);
		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = _index.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int)*_index.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				m_indexBufferobj.GetAddressOf()
			)
		);
		HRESULT h = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Diffuse_Fuzzy_Corrupt.dds", NULL, &m_SRV);
	}*/
