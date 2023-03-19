#include "Mesh.h"

using namespace DirectX;


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    std::vector<XMFLOAT3> Positions;
    std::vector<XMFLOAT2> TexC0;
    std::vector<std::uint32_t> Indices;

	Positions.resize(24);
	TexC0.resize(24);

    float w = 10.0f / 2;
    float h = 10.0f / 2;
    float d = 10.0f / 2;

	Positions =
	{
		XMFLOAT3(-w, -h, -d),
		XMFLOAT3(-w, +h, -d),
		XMFLOAT3(+w, +h, -d),
		XMFLOAT3(+w, -h, -d),

		XMFLOAT3(-w, -h, +d),
		XMFLOAT3(+w, -h, +d),
		XMFLOAT3(+w, +h, +d),
		XMFLOAT3(-w, +h, +d),

		XMFLOAT3(-w, +h, -d),
		XMFLOAT3(-w, +h, +d),
		XMFLOAT3(+w, +h, +d),
		XMFLOAT3(+w, +h, -d),

		XMFLOAT3(-w, -h, -d),
		XMFLOAT3(+w, -h, -d),
		XMFLOAT3(+w, -h, +d),
		XMFLOAT3(-w, -h, +d),

		XMFLOAT3(-w, -h, +d),
		XMFLOAT3(-w, +h, +d),
		XMFLOAT3(-w, +h, -d),
		XMFLOAT3(-w, -h, -d),

		XMFLOAT3(+w, -h, -d),
		XMFLOAT3(+w, +h, -d),
		XMFLOAT3(+w, +h, +d),
		XMFLOAT3(+w, -h, +d)
	};
	TexC0 =
	{
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};

	Indices.resize(36);

	Indices =
	{
		// ¾Õ¸é
		0,1,2,
		0,2,3,

		// µÞ¸é
		4,5,6,
		4,6,7,

		// ¿ÞÂÊ ¸é
		8,9,10,
		8,10,11,

		// ¿À¸¥ÂÊ ¸é
		12,13,14,
		12,14,15,

		// À­¸é
		16,17,18,
		16,18,19,

		// ¾Æ·§¸é
		20,21,22,
		20,22,23
	};

	const UINT positionBufferByteSize = (UINT)Positions.size() * sizeof(XMFLOAT3);
	const UINT texC0BufferByteSize = (UINT)TexC0.size() * sizeof(XMFLOAT2);
	const UINT indexBufferByteSize = (UINT)Indices.size() * sizeof(std::uint_fast32_t);

	const char* tmpStr = "boxGeo";
	SetMeshName(tmpStr);

	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_PositionBufferGPU, &m_PositionBufferUploader,
		positionBufferByteSize, sizeof(XMFLOAT3),
		&m_PositionBufferView, Positions.data());

	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_TexC0BufferGPU, &m_TexC0BufferUploader,
		texC0BufferByteSize, sizeof(XMFLOAT2),
		&m_TexC0BufferView, TexC0.data());

	CreateIndexBuffer(pd3dDevice, pd3dCommandList,
		&m_IndexBufferGPU, &m_IndexBufferUploader,
		indexBufferByteSize,
		&m_IndexBufferView, Indices.data());

	SubmeshGeometry subMesh;
	subMesh.IndexCount = (UINT)Indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;
	// subMesh.Bounds = BoundingBox()

	m_DrawArgs.emplace_back(subMesh);
}

void Mesh::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[2] = { m_PositionBufferView, m_TexC0BufferView };
    pd3dCommandList->IASetVertexBuffers(0, 2, pVertexBufferView);
    pd3dCommandList->IASetIndexBuffer(&m_IndexBufferView);
    pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void Mesh::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
    for (auto begin = m_DrawArgs.begin(); begin != m_DrawArgs.end(); ++begin)
    {
        pd3dCommandList->DrawIndexedInstanced(
            begin->IndexCount, 1, begin->StartIndexLocation, begin->BaseVertexLocation, 0);
    }
}

void Mesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };

	UINT nReads;

	m_nVertices = ReadintegerFromFile(pInFile);
	nReads = ReadStringFromFile(pInFile, pstrToken);
	SetMeshName(pstrToken);

	for (; ; )
	{
		nReads = ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)fread(&m_AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)fread(&m_AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			int nPosition = 0;
			nPosition = ReadintegerFromFile(pInFile);
			if (nPosition)
			{
				m_Positions.resize(nPosition);
				nReads = (UINT)fread(&m_Positions[0], sizeof(XMFLOAT3), nPosition, pInFile);

				UINT positionBufferByteSize = sizeof(XMFLOAT3) * nPosition;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_PositionBufferGPU, &m_PositionBufferUploader,
					positionBufferByteSize, sizeof(XMFLOAT3),
					&m_PositionBufferView, m_Positions.data());
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			int nColor = 0;
			nColor = ReadintegerFromFile(pInFile);
			if (nColor)
			{
				m_Colors.resize(nColor);
				nReads = (UINT)fread(&m_Colors[0], sizeof(XMFLOAT3), nColor, pInFile);

				UINT colorBufferByteSize = sizeof(XMFLOAT3) * nColor;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_ColorBufferGPU, &m_ColorBufferUploader,
					colorBufferByteSize, sizeof(XMFLOAT3),
					&m_ColorBufferView, m_Colors.data());
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			int nTexC0 = 0;
			nTexC0 = ReadintegerFromFile(pInFile);
			if (nTexC0)
			{
				m_TextureC0.resize(nTexC0);
				nReads = (UINT)fread(&m_TextureC0[0], sizeof(XMFLOAT2), nTexC0, pInFile);

				UINT TextureC0BufferByteSize = sizeof(XMFLOAT2) * nTexC0;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TexC0BufferGPU, &m_TexC0BufferUploader,
					TextureC0BufferByteSize, sizeof(XMFLOAT2),
					&m_TexC0BufferView, m_TextureC0.data());
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			int nTexC1 = 0;
			nTexC1 = ReadintegerFromFile(pInFile);
			if (nTexC1)
			{
				m_TextureC1.resize(nTexC1);
				nReads = (UINT)fread(&m_TextureC1[0], sizeof(XMFLOAT2), nTexC1, pInFile);

				UINT TextureC1BufferByteSize = sizeof(XMFLOAT2) * nTexC1;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TexC1BufferGPU, &m_TexC1BufferUploader,
					TextureC1BufferByteSize, sizeof(XMFLOAT2),
					&m_TexC1BufferView, m_TextureC1.data());
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			int nNormal = 0;
			nNormal = ReadintegerFromFile(pInFile);
			if (nNormal)
			{
				m_Normals.resize(nNormal);
				nReads = (UINT)fread(&m_Normals[0], sizeof(XMFLOAT3), nNormal, pInFile);

				UINT normalBufferByteSize = sizeof(XMFLOAT3) * nNormal;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_NormalBufferGPU, &m_NormalBufferUploader,
					normalBufferByteSize, sizeof(XMFLOAT3),
					&m_NormalBufferView, m_Normals.data());
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			int nTangent = 0;
			nTangent = ReadintegerFromFile(pInFile);
			if (nTangent)
			{
				m_Tangents.resize(nTangent);
				nReads = (UINT)fread(&m_Tangents[0], sizeof(XMFLOAT3), nTangent, pInFile);

				UINT tangentBufferByteSize = sizeof(XMFLOAT3) * nTangent;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TangentBufferGPU, &m_TangentBufferUploader,
					tangentBufferByteSize, sizeof(XMFLOAT3),
					&m_TangentBufferView, m_Tangents.data());
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			int nBiTangent = 0;
			nBiTangent = ReadintegerFromFile(pInFile);
			if (nBiTangent)
			{
				m_BiTangents.resize(nBiTangent);
				nReads = (UINT)fread(&m_BiTangents[0], sizeof(XMFLOAT3), nBiTangent, pInFile);

				UINT bitangentBufferByteSize = sizeof(XMFLOAT3) * nBiTangent;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_BiTangentBufferGPU, &m_BiTangentBufferUploader,
					bitangentBufferByteSize, sizeof(XMFLOAT3),
					&m_BiTangentBufferView, m_BiTangents.data());
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			int nSubMeshes = 0;
			nSubMeshes = ReadintegerFromFile(pInFile);

			if (nSubMeshes > 0)
			{
				int nIdxStack = 0;

				for (int i = 0; i < nSubMeshes; ++i)
				{
					nReads = ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						int nIdxCnt = 0;
						nIndex = ReadintegerFromFile(pInFile);
						nIdxCnt = ReadintegerFromFile(pInFile);

						if (nIdxCnt > 0)
						{
							m_Indices.resize(nIdxCnt + nIdxStack);
							nReads = (UINT)fread(&m_Indices[nIdxStack], sizeof(UINT), nIdxCnt, pInFile);

							SubmeshGeometry subMesh;
							subMesh.IndexCount = nIdxCnt;
							subMesh.StartIndexLocation = nIdxStack;
							subMesh.BaseVertexLocation = 0;

							nIdxStack += nIdxCnt;

							m_DrawArgs.emplace_back(subMesh);

							UINT indexBufferByteSize = sizeof(UINT) * nIdxCnt;

							CreateIndexBuffer(pd3dDevice, pd3dCommandList,
								&m_IndexBufferGPU, &m_IndexBufferUploader,
								indexBufferByteSize,
								&m_IndexBufferView, m_Indices.data());
						}
					}
				}

				
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}


}

void Mesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
	UINT bufferByteSize,
	UINT strideInBytes,
	D3D12_VERTEX_BUFFER_VIEW* pVertexBufferView, void* pData)
{
	*pBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		pData, bufferByteSize,
		*pBufferUploader);

	pVertexBufferView->BufferLocation = (*pBufferGPU)->GetGPUVirtualAddress();
	pVertexBufferView->StrideInBytes = strideInBytes;
	pVertexBufferView->SizeInBytes = bufferByteSize;
}

void Mesh::CreateIndexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU, 
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
	UINT bufferByteSize,
	D3D12_INDEX_BUFFER_VIEW* pIndexBufferView, void* pData)
{
	*pBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		pData, bufferByteSize,
		*pBufferUploader);

	pIndexBufferView->BufferLocation = (*pBufferGPU)->GetGPUVirtualAddress();
	pIndexBufferView->Format = m_IndexFormat;
	pIndexBufferView->SizeInBytes = bufferByteSize;
}

void Mesh::DisposeUploaders()
{
    m_PositionBufferUploader = nullptr;
    m_ColorBufferUploader = nullptr;
    m_IndexBufferUploader = nullptr;
}

////////////////////////////////////////////////////////////////////

SkinnedMesh::SkinnedMesh()
{
}

SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pFile)
{
}

void SkinnedMesh::PrepareSkinning(Object* pModelRootObject)
{
}

void SkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void SkinnedMesh::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
}
