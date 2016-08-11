#include "pch.h"
#include "Model.h"


Model::Model(char* _file, wchar_t* _texture, ID3D11Device* _dev, DirectX::XMFLOAT3 _offset)
{
	loadobj(_file, _texture, _dev, _offset);
}
Model::~Model()
{

}

void Model::loadobj(char* _file, wchar_t* _texture, ID3D11Device* _dev, DirectX::XMFLOAT3 _offset)
{
	std::vector<DirectX::XMFLOAT3> tempverts;
	std::vector<DirectX::XMFLOAT2> tempuv;
	std::vector<DirectX::XMFLOAT3> tempnorms;
	std::vector<unsigned int> vert_ind, uv_ind, norm_ind;

	FILE* file;
	fopen_s(&file, _file, "r");
	if (file == NULL)
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
			DirectX::XMFLOAT3 vert;
			fscanf_s(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
			tempverts.push_back(vert);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			DirectX::XMFLOAT2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1 - uv.y;
			tempuv.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			DirectX::XMFLOAT3 normal;
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
		DirectX::XMFLOAT3 vertex = tempverts[vertexIndex - 1];
		temp.pos = vertex;
		unsigned int uvIndex = uv_ind[i];
		DirectX::XMFLOAT2 uv = tempuv[uvIndex - 1];
		temp.uv = uv;
		unsigned int normIndex = norm_ind[i];
		DirectX::XMFLOAT3 normal = tempnorms[normIndex - 1];
		temp.norm = normal;
		IL_verts.push_back(temp);
		IL_index.push_back(i);
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = IL_verts.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex)*IL_verts.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		_dev->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			m_VB.GetAddressOf()
		)

	);
	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = IL_index.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int)*IL_index.size(), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		_dev->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			m_IB.GetAddressOf()
		)
	);
	HRESULT h = CreateDDSTextureFromFile(_dev, _texture, NULL, &m_SRV);
}
void Model::SetBuffer(ID3D11DeviceContext3* _con)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_con->IASetVertexBuffers(
		0,
		1,
		m_VB.GetAddressOf(),
		&stride,
		&offset
	);

	_con->IASetIndexBuffer(
		m_IB.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);
}
void Model::Drawindex(ID3D11DeviceContext3* _con)
{
	_con->DrawIndexed(
		IL_index.size(),
		0,
		0
	);
}