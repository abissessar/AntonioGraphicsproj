#pragma once
#include "Content\ShaderStructures.h"
#include "Common\DirectXHelper.h"
class Model 
{
private:
	void loadobj(char* _file, wchar_t* _texture, ID3D11Device* _dev, DirectX::XMFLOAT3 _offset);
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 norm;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
	DirectX::XMMATRIX WM;

	Model(char* _file, wchar_t* _texture, ID3D11Device* _dev, DirectX::XMFLOAT3 _offset);
	~Model();

	std::vector<Vertex> IL_verts;
	std::vector<unsigned int> IL_index;

	void SetBuffer(ID3D11DeviceContext3* _con);

	void Drawindex(ID3D11DeviceContext3* _con);
};

