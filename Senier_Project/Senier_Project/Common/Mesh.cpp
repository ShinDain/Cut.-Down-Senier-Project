#include "Mesh.h"
#include "Object.h"

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
		// 앞면
		0,1,2,
		0,2,3,

		// 뒷면
		4,5,6,
		4,6,7,

		// 왼쪽 면
		8,9,10,
		8,10,11,

		// 오른쪽 면
		12,13,14,
		12,14,15,

		// 윗면
		16,17,18,
		16,18,19,

		// 아랫면
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

	m_vIndexBufferGPU.emplace_back(Microsoft::WRL::ComPtr<ID3D12Resource>());
	m_vIndexBufferUploader.emplace_back(Microsoft::WRL::ComPtr<ID3D12Resource>());
	m_vIndexBufferView.emplace_back(D3D12_INDEX_BUFFER_VIEW());
	
	CreateIndexBuffer(pd3dDevice, pd3dCommandList,
		&m_vIndexBufferGPU[0], &m_vIndexBufferUploader[0],
		indexBufferByteSize,
		&m_vIndexBufferView[0], Indices.data());

	SubmeshGeometry subMesh;
	subMesh.IndexCount = (UINT)Indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;
	// subMesh.Bounds = BoundingBox()

	m_vDrawArgs.emplace_back(subMesh);
}

void Mesh::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[2] = { m_PositionBufferView, m_TexC0BufferView };
    pd3dCommandList->IASetVertexBuffers(0, 2, pVertexBufferView);
    pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void Mesh::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
    for (int i = 0 ; i< m_vDrawArgs.size(); ++i)
    {
		pd3dCommandList->IASetIndexBuffer(&m_vIndexBufferView[i]);
        pd3dCommandList->DrawIndexedInstanced(
            m_vDrawArgs[i].IndexCount, 1, m_vDrawArgs[i].StartIndexLocation, m_vDrawArgs[i].BaseVertexLocation, 0);
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
			nReads = (UINT)fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			m_nType |= VERTEXT_POSITION;

			int nPosition = 0;
			nPosition = ReadintegerFromFile(pInFile);
			if (nPosition)
			{
				m_vPositions.resize(nPosition);
				nReads = (UINT)fread(&m_vPositions[0], sizeof(XMFLOAT3), nPosition, pInFile);

				UINT positionBufferByteSize = sizeof(XMFLOAT3) * nPosition;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_PositionBufferGPU, &m_PositionBufferUploader,
					positionBufferByteSize, sizeof(XMFLOAT3),
					&m_PositionBufferView, m_vPositions.data());
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			int nColor = 0;
			nColor = ReadintegerFromFile(pInFile);
			if (nColor)
			{
				m_nType |= VERTEXT_COLOR;

				m_vColors.resize(nColor);
				nReads = (UINT)fread(&m_vColors[0], sizeof(XMFLOAT3), nColor, pInFile);

				UINT colorBufferByteSize = sizeof(XMFLOAT3) * nColor;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_ColorBufferGPU, &m_ColorBufferUploader,
					colorBufferByteSize, sizeof(XMFLOAT3),
					&m_ColorBufferView, m_vColors.data());
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			int nTexC0 = 0;
			nTexC0 = ReadintegerFromFile(pInFile);
			if (nTexC0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;

				m_vTextureC0.resize(nTexC0);
				nReads = (UINT)fread(&m_vTextureC0[0], sizeof(XMFLOAT2), nTexC0, pInFile);

				UINT TextureC0BufferByteSize = sizeof(XMFLOAT2) * nTexC0;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TexC0BufferGPU, &m_TexC0BufferUploader,
					TextureC0BufferByteSize, sizeof(XMFLOAT2),
					&m_TexC0BufferView, m_vTextureC0.data());
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			int nTexC1 = 0;
			nTexC1 = ReadintegerFromFile(pInFile);
			if (nTexC1)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;

				m_vTextureC1.resize(nTexC1);
				nReads = (UINT)fread(&m_vTextureC1[0], sizeof(XMFLOAT2), nTexC1, pInFile);

				UINT TextureC1BufferByteSize = sizeof(XMFLOAT2) * nTexC1;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TexC1BufferGPU, &m_TexC1BufferUploader,
					TextureC1BufferByteSize, sizeof(XMFLOAT2),
					&m_TexC1BufferView, m_vTextureC1.data());
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			int nNormal = 0;
			nNormal = ReadintegerFromFile(pInFile);
			if (nNormal)
			{
				m_nType |= VERTEXT_NORMAL;

				m_vNormals.resize(nNormal);
				nReads = (UINT)fread(&m_vNormals[0], sizeof(XMFLOAT3), nNormal, pInFile);

				UINT normalBufferByteSize = sizeof(XMFLOAT3) * nNormal;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_NormalBufferGPU, &m_NormalBufferUploader,
					normalBufferByteSize, sizeof(XMFLOAT3),
					&m_NormalBufferView, m_vNormals.data());
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			int nTangent = 0;
			nTangent = ReadintegerFromFile(pInFile);
			if (nTangent)
			{
				m_nType |= VERTEXT_TANGENT;

				m_vTangents.resize(nTangent);
				nReads = (UINT)fread(&m_vTangents[0], sizeof(XMFLOAT3), nTangent, pInFile);

				UINT tangentBufferByteSize = sizeof(XMFLOAT3) * nTangent;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TangentBufferGPU, &m_TangentBufferUploader,
					tangentBufferByteSize, sizeof(XMFLOAT3),
					&m_TangentBufferView, m_vTangents.data());
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			int nBiTangent = 0;
			nBiTangent = ReadintegerFromFile(pInFile);
			if (nBiTangent)
			{
				m_vBiTangents.resize(nBiTangent);
				nReads = (UINT)fread(&m_vBiTangents[0], sizeof(XMFLOAT3), nBiTangent, pInFile);

				UINT bitangentBufferByteSize = sizeof(XMFLOAT3) * nBiTangent;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_BiTangentBufferGPU, &m_BiTangentBufferUploader,
					bitangentBufferByteSize, sizeof(XMFLOAT3),
					&m_BiTangentBufferView, m_vBiTangents.data());
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			int nSubMeshes = 0;
			nSubMeshes = ReadintegerFromFile(pInFile);

			if (nSubMeshes > 0)
			{
				m_vvIndices.resize(nSubMeshes);
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
							m_vvIndices[nIndex].resize(nIdxCnt);
							nReads = (UINT)fread(&m_vvIndices[nIndex][0], sizeof(UINT), nIdxCnt, pInFile);

							SubmeshGeometry subMesh;
							subMesh.IndexCount = nIdxCnt;
							subMesh.StartIndexLocation = 0;
							subMesh.BaseVertexLocation = 0;

							m_vDrawArgs.emplace_back(subMesh);

							UINT indexBufferByteSize = sizeof(UINT_FAST32_MAX) * nIdxCnt;

							m_vIndexBufferGPU.emplace_back(Microsoft::WRL::ComPtr<ID3D12Resource>());
							m_vIndexBufferUploader.emplace_back(Microsoft::WRL::ComPtr<ID3D12Resource>());
							m_vIndexBufferView.emplace_back(D3D12_INDEX_BUFFER_VIEW());
							CreateIndexBuffer(pd3dDevice, pd3dCommandList,
								&m_vIndexBufferGPU[nIndex], &m_vIndexBufferUploader[nIndex],
								indexBufferByteSize,
								&m_vIndexBufferView[nIndex], m_vvIndices[nIndex].data());
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
	for(int i = 0 ; i< m_vIndexBufferUploader.size(); ++i)
		m_vIndexBufferUploader[i] = nullptr;
}

////////////////////////////////////////////////////////////////////

SkinnedMesh::SkinnedMesh()
{
}

SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	ReadStringFromFile(pInFile, pstrToken);

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			m_nBonesPerVertex = ReadintegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			m_nSkinningBones = ReadintegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_vstrSkinningBoneNames.resize(m_nSkinningBones);
				m_vpSkinningBoneFrameCaches.resize(m_nSkinningBones);
				m_vxmf4x4SkinningBoneTransforms.resize(m_nSkinningBones);
				for (int i = 0; i < m_nSkinningBones; ++i)
				{
					nReads = ReadStringFromFile(pInFile, pstrToken);
					m_vstrSkinningBoneNames[i] = pstrToken;
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			m_nSkinningBones = ReadintegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_vxmf4x4BindPoseBoneOffsets.resize(m_nSkinningBones);
				nReads = (UINT)fread(&m_vxmf4x4BindPoseBoneOffsets[0], sizeof(XMFLOAT4X4), m_nSkinningBones, pInFile);

				m_BindPoseBoneOffsetCB = std::make_unique<UploadBuffer<XMFLOAT4X4>>(pd3dDevice, m_nSkinningBones, true);
				m_nBindPoseBoneOffsetCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(XMFLOAT4X4) * m_nSkinningBones);

				for (int i = 0; i < m_nSkinningBones; ++i)
				{
					XMFLOAT4X4 transposeOffset;
					XMStoreFloat4x4(&transposeOffset, XMMatrixTranspose(XMLoadFloat4x4(&m_vxmf4x4BindPoseBoneOffsets[i])));

					m_BindPoseBoneOffsetCB->CopyData(i, transposeOffset);
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneIndices>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ReadintegerFromFile(pInFile);

			if (m_nVertices > 0)
			{
				m_vxmn4BoneIndices.resize(m_nVertices);
				nReads = (UINT)fread(&m_vxmn4BoneIndices[0], sizeof(XMINT4), m_nVertices, pInFile);

				UINT boneIndexBufferByteSize = sizeof(XMINT4) * m_nVertices;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_BoneIndexBufferGPU, &m_BoneIndexBufferUploader,
					boneIndexBufferByteSize, sizeof(XMINT4),
					&m_BoneIndexBufferView, m_vxmn4BoneIndices.data());
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ReadintegerFromFile(pInFile);

			if (m_nVertices > 0)
			{
				m_vxmf4BoneWeights.resize(m_nVertices);

				nReads = (UINT)fread(&m_vxmf4BoneWeights[0], sizeof(XMFLOAT4), m_nVertices, pInFile);

				UINT boneWeightBufferByteSize = sizeof(XMFLOAT4) * m_nVertices;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_BoneWeightBufferGPU, &m_BoneWeightBufferUploader,
					boneWeightBufferByteSize, sizeof(XMFLOAT4),
					&m_BoneWeightBufferView, m_vxmf4BoneWeights.data());
			}
		}
		else if (!strcmp(pstrToken, "</SkinningInfo>"))
		{
			break;
		}
	}
}

void SkinnedMesh::PrepareSkinning(Object* pModelRootObject)
{
	// BoneFrame 캐싱
	char pstrFrameName[64] = { '\0' };

	for (int i = 0; i < m_nSkinningBones; ++i)
	{
		strcpy_s(pstrFrameName, m_vstrSkinningBoneNames[i].c_str());
		m_vpSkinningBoneFrameCaches[i] = pModelRootObject->FindFrame(pstrFrameName);
	}
}

void SkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// BindPoseBoneOffset 상수 버퍼 연결
	if (m_BindPoseBoneOffsetCB != nullptr)
	{
		D3D12_GPU_VIRTUAL_ADDRESS BoneOffsetsGpuVirtualAddress = m_BindPoseBoneOffsetCB->Resource()->GetGPUVirtualAddress();
		// 루트 서명에 상수 버퍼 연결
		pd3dCommandList->SetGraphicsRootConstantBufferView(3, BoneOffsetsGpuVirtualAddress);
	}

	// SkinningBoneTransform 상수 버퍼 연결
	if (m_SkinningBoneTransformCB != nullptr)
	{
		D3D12_GPU_VIRTUAL_ADDRESS SkinnginBoneTransformGpuVirtualAddress = m_SkinningBoneTransformCB->Resource()->GetGPUVirtualAddress();
		// 루트 서명에 상수 버퍼 연결
		pd3dCommandList->SetGraphicsRootConstantBufferView(4, SkinnginBoneTransformGpuVirtualAddress);

		for (int i = 0; i < m_nSkinningBones; ++i)
		{
			XMStoreFloat4x4(&m_vxmf4x4SkinningBoneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&m_vpSkinningBoneFrameCaches[i]->GetWorld())));

			XMFLOAT4X4 transposeTransform;
			XMStoreFloat4x4(&transposeTransform, XMMatrixTranspose(XMLoadFloat4x4(&m_vxmf4x4SkinningBoneTransforms[i])));

			if (m_SkinningBoneTransformCB) m_SkinningBoneTransformCB->CopyData(i, transposeTransform);
		}
	}
}

void SkinnedMesh::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UpdateShaderVariables(pd3dCommandList);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[7] = 
	{ m_PositionBufferView, m_TexC0BufferView, m_NormalBufferView, m_TangentBufferView,
		m_BiTangentBufferView, m_BoneIndexBufferView, m_BoneWeightBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 7, pVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

}