#include "pch.h"
#include "Model.h"


Model::Model(char* _file, wchar_t* _texture, ID3D11Device* _dev, DirectX::XMFLOAT3 _offset, wchar_t*normalmap)
{
	loadobj(_file, _texture, _dev, _offset, normalmap);
}
Model::~Model()
{

}

void Model::loadobj(char* _file, wchar_t* _texture, ID3D11Device* _dev, DirectX::XMFLOAT3 _offset, wchar_t*normalmap)
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
		bool unique = true;
		unsigned int index;
		for (int j = 0; j < IL_verts.size(); ++j)
		{
			if (temp.pos.x == IL_verts[j].pos.x 
				&& temp.pos.y == IL_verts[j].pos.y 
				&&temp.pos.z == IL_verts[j].pos.z
				&&temp.norm.x == IL_verts[j].norm.x
				&& temp.norm.y == IL_verts[j].norm.y
				&&temp.norm.z == IL_verts[j].norm.z
				&&temp.uv.x == IL_verts[j].uv.x
				&& temp.uv.y == IL_verts[j].uv.y)
			{
				unique = false;
				index = j;
				break;
			}
		}
		if (unique)
		{
			IL_verts.push_back(temp);
			IL_index.push_back(IL_verts.size() - 1);
		}
		else
		{
			IL_index.push_back(index);
		}
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
	if (normalmap != nullptr)
	{
		Normalstuff();
		HRESULT h = CreateDDSTextureFromFile(_dev, normalmap, NULL, &m_SRVnorm);

	}
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
void Model::Normalstuff()
{
	DirectX::XMFLOAT3 vert0, vert1, vert2;
	DirectX::XMFLOAT3 vertEdge0, vertEdge1;
	DirectX::XMFLOAT2 tex0, tex1, tex2;
	DirectX::XMFLOAT2 texEdge0, texEdge1;
	float ratio;
	for  (int i = 0; i < IL_index.size(); i+=3)
	{
		vert0 = IL_verts[IL_index[i]].pos;
		vert1 = IL_verts[IL_index[i + 1]].pos;
		vert2 = IL_verts[IL_index[i + 2]].pos;
		DirectX::XMStoreFloat3(&vertEdge0, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vert1), DirectX::XMLoadFloat3(&vert0)));
		DirectX::XMStoreFloat3(&vertEdge1, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vert2), DirectX::XMLoadFloat3(&vert0)));
		tex0 = IL_verts[IL_index[i]].uv;
		tex1 = IL_verts[IL_index[i + 1]].uv;
		tex2 = IL_verts[IL_index[i + 2]].uv;
		DirectX::XMStoreFloat2(&texEdge0, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&tex1), DirectX::XMLoadFloat2(&tex0)));
		DirectX::XMStoreFloat2(&texEdge1, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&tex2), DirectX::XMLoadFloat2(&tex0)));
		ratio = 1.0f / (texEdge0.x * texEdge1.y - texEdge1.x*texEdge0.y);
		DirectX::XMFLOAT3 uDirection = DirectX::XMFLOAT3(
										(texEdge1.y * vertEdge0.x) - (texEdge0.y * vertEdge1.x) * ratio, 
										(texEdge1.y * vertEdge0.y) - (texEdge0.y * vertEdge1.y) * ratio, 
										(texEdge1.y * vertEdge0.z) - (texEdge0.y * vertEdge1.z) * ratio);
		DirectX::XMFLOAT3 vDirection = DirectX::XMFLOAT3(
			(texEdge0.x * vertEdge1.x) - (texEdge1.x * vertEdge0.x) * ratio,
			(texEdge0.x * vertEdge1.y) - (texEdge1.x * vertEdge0.y) * ratio,
			(texEdge0.x * vertEdge1.z) - (texEdge1.x * vertEdge0.z) * ratio);

		for (int j = 0; j < 3; ++j)
		{
			DirectX::XMVECTOR unorm = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&uDirection));
			DirectX::XMVECTOR dot = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&IL_verts[IL_index[i + j]].norm), unorm);
			DirectX::XMVECTOR tan = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&uDirection), DirectX::XMVectorScale(unorm ,dot.m128_f32[0]));
			tan = DirectX::XMVector3Normalize(tan);
			DirectX::XMVECTOR vnorm = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vDirection));
			DirectX::XMVECTOR cross = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&IL_verts[IL_index[i + j]].norm), unorm);
			DirectX::XMVECTOR handedness = DirectX::XMLoadFloat3(&vDirection);
			dot = DirectX::XMVector3Dot(cross, handedness);
			tan.m128_f32[3] = (dot.m128_f32[0] < 0.0f) ? -1.0f : 1.0f;
			IL_verts[IL_index[i + j]].tangent.x = tan.m128_f32[0];
			IL_verts[IL_index[i + j]].tangent.y = tan.m128_f32[1];
			IL_verts[IL_index[i + j]].tangent.z = tan.m128_f32[2];
		}
	}
}