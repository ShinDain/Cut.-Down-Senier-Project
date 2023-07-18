#include "../Header/Mesh.h"
#include "../Header/Object.h"
#include "../Header/Shader.h"

using namespace DirectX;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	 m_PositionBufferGPU = nullptr;
	 m_PositionBufferUploader = nullptr;

	 m_ColorBufferGPU = nullptr;
	 m_ColorBufferUploader = nullptr;

	 m_TexC0BufferGPU = nullptr;
	 m_TexC0BufferUploader = nullptr;

	 m_TexC1BufferGPU = nullptr;
	 m_TexC1BufferUploader = nullptr;

	 m_NormalBufferGPU = nullptr;
	 m_NormalBufferUploader = nullptr;

	 m_TangentBufferGPU = nullptr;
	 m_TangentBufferUploader = nullptr;

	 m_BiTangentBufferGPU = nullptr;
	 m_BiTangentBufferUploader = nullptr;

	 for (int i = 0; i < m_vIndexBufferGPU.size(); ++i)
	 {
		 m_vIndexBufferGPU[i].Reset();
	 }

	 for (int i = 0; i < m_vIndexBufferUploader.size(); ++i)
	 {
		 m_vIndexBufferUploader[i].Reset();
	 }
}

void Mesh::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[5] = { m_PositionBufferView, m_NormalBufferView , m_TexC0BufferView,
		m_TangentBufferView, m_BiTangentBufferView };
    pd3dCommandList->IASetVertexBuffers(0, 5, pVertexBufferView);
    pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void Mesh::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);

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

	std::vector<XMFLOAT3> vPosition;
	std::vector<XMFLOAT4> vColor;
	std::vector<XMFLOAT2> vTextureC0;
	std::vector<XMFLOAT2> vTextureC1;
	std::vector<XMFLOAT3> vNormal;
	std::vector<XMFLOAT3> vTangent;
	std::vector<XMFLOAT3> vBiTangent;
	std::vector<std::vector<UINT>> vvIndices;

	for (; ; )
	{
		nReads = ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)fread(&m_xmf3Center, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)fread(&m_xmf3Extents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			int nPosition = 0;
			nPosition = ReadintegerFromFile(pInFile);
			if (nPosition)
			{
				m_nType |= VERTEXT_POSITION;

				vPosition.resize(nPosition);

				nReads = (UINT)fread(&vPosition[0], sizeof(XMFLOAT3), nPosition, pInFile);

				UINT positionBufferByteSize = sizeof(XMFLOAT3) * nPosition;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_PositionBufferGPU, &m_PositionBufferUploader,
					positionBufferByteSize, sizeof(XMFLOAT3),
					&m_PositionBufferView, vPosition.data());
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			int nColor = 0;
			nColor = ReadintegerFromFile(pInFile);
			if (nColor)
			{
				m_nType |= VERTEXT_COLOR;

				vColor.resize(nColor);
				nReads = (UINT)fread(&vColor[0], sizeof(XMFLOAT4), nColor, pInFile);

				UINT colorBufferByteSize = sizeof(XMFLOAT4) * nColor;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_ColorBufferGPU, &m_ColorBufferUploader,
					colorBufferByteSize, sizeof(XMFLOAT4),
					&m_ColorBufferView, vColor.data());
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			int nTexC0 = 0;
			nTexC0 = ReadintegerFromFile(pInFile);
			if (nTexC0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;

				vTextureC0.resize(nTexC0);
				nReads = (UINT)fread(&vTextureC0[0], sizeof(XMFLOAT2), nTexC0, pInFile);

				UINT TextureC0BufferByteSize = sizeof(XMFLOAT2) * nTexC0;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TexC0BufferGPU, &m_TexC0BufferUploader,
					TextureC0BufferByteSize, sizeof(XMFLOAT2),
					&m_TexC0BufferView, vTextureC0.data());
			}
			else
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;

				nTexC0 = m_nVertices;
				vTextureC0.resize(nTexC0);
				for (int i = 0; i < nTexC0; ++i)
					vTextureC0.push_back(XMFLOAT2(0, 0));

				UINT TextureC0BufferByteSize = sizeof(XMFLOAT2) * nTexC0;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TexC0BufferGPU, &m_TexC0BufferUploader,
					TextureC0BufferByteSize, sizeof(XMFLOAT2),
					&m_TexC0BufferView, vTextureC0.data());
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			int nTexC1 = 0;
			nTexC1 = ReadintegerFromFile(pInFile);
			if (nTexC1)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;

				vTextureC1.resize(nTexC1);
				nReads = (UINT)fread(&vTextureC1[0], sizeof(XMFLOAT2), nTexC1, pInFile);

				UINT TextureC1BufferByteSize = sizeof(XMFLOAT2) * nTexC1;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TexC1BufferGPU, &m_TexC1BufferUploader,
					TextureC1BufferByteSize, sizeof(XMFLOAT2),
					&m_TexC1BufferView, vTextureC1.data());
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			int nNormal = 0;
			nNormal = ReadintegerFromFile(pInFile);
			if (nNormal)
			{
				m_nType |= VERTEXT_NORMAL;

				vNormal.resize(nNormal);
				nReads = (UINT)fread(&vNormal[0], sizeof(XMFLOAT3), nNormal, pInFile);

				UINT normalBufferByteSize = sizeof(XMFLOAT3) * nNormal;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_NormalBufferGPU, &m_NormalBufferUploader,
					normalBufferByteSize, sizeof(XMFLOAT3),
					&m_NormalBufferView, vNormal.data());
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			int nTangent = 0;
			nTangent = ReadintegerFromFile(pInFile);
			if (nTangent)
			{
				m_nType |= VERTEXT_TANGENT;

				vTangent.resize(nTangent);
				nReads = (UINT)fread(&vTangent[0], sizeof(XMFLOAT3), nTangent, pInFile);

				UINT tangentBufferByteSize = sizeof(XMFLOAT3) * nTangent;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_TangentBufferGPU, &m_TangentBufferUploader,
					tangentBufferByteSize, sizeof(XMFLOAT3),
					&m_TangentBufferView, vTangent.data());
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			int nBiTangent = 0;
			nBiTangent = ReadintegerFromFile(pInFile);
			if (nBiTangent)
			{
				vBiTangent.resize(nBiTangent);
				nReads = (UINT)fread(&vBiTangent[0], sizeof(XMFLOAT3), nBiTangent, pInFile);

				UINT bitangentBufferByteSize = sizeof(XMFLOAT3) * nBiTangent;

				CreateVertexBuffer(pd3dDevice, pd3dCommandList,
					&m_BiTangentBufferGPU, &m_BiTangentBufferUploader,
					bitangentBufferByteSize, sizeof(XMFLOAT3),
					&m_BiTangentBufferView, vBiTangent.data());
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			int nSubMeshes = 0;
			nSubMeshes = ReadintegerFromFile(pInFile);

			if (nSubMeshes > 0)
			{
				vvIndices.resize(nSubMeshes);
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
							vvIndices[nIndex].resize(nIdxCnt);
							nReads = (UINT)fread(&vvIndices[nIndex][0], sizeof(UINT), nIdxCnt, pInFile);

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
								indexBufferByteSize, m_IndexFormat,
								&m_vIndexBufferView[nIndex], vvIndices[nIndex].data());
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

////////////////////////////////////////////////////////////////////

SkinnedMesh::SkinnedMesh()
{
	m_BoneIndexBufferView.BufferLocation = NULL;
	m_BoneIndexBufferView.SizeInBytes = 0;
	m_BoneIndexBufferView.StrideInBytes = 0;

	m_BoneWeightBufferView.BufferLocation = NULL;
	m_BoneWeightBufferView.SizeInBytes = 0;
	m_BoneWeightBufferView.StrideInBytes = 0;
}

SkinnedMesh::~SkinnedMesh()
{
	m_BoneIndexBufferGPU = nullptr;
	m_BoneIndexBufferUploader = nullptr;

	m_BoneWeightBufferGPU = nullptr;
	m_BoneWeightBufferUploader = nullptr;

	for (int i = 0; i < m_vpSkinningBoneFrameCaches.size(); ++i)
	{
		m_vpSkinningBoneFrameCaches[i].reset();
		m_vpSkinningBoneFrameCaches.erase(m_vpSkinningBoneFrameCaches.begin() + i);
	}

	m_SkinningBoneTransformCB.reset();
	m_BindPoseBoneOffsetCB.reset();
}

void SkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[128] = { '\0' };
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
			nReads = (UINT)fread(&m_xmf3Center, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)fread(&m_xmf3Extents, sizeof(XMFLOAT3), 1, pInFile);
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

				m_BindPoseBoneOffsetCB = std::make_unique<UploadBuffer<BoneBindPoseOffsetConstant>>(pd3dDevice, m_nSkinningBones, true);

				BoneBindPoseOffsetConstant* tmpBoneOffsetConstant = new BoneBindPoseOffsetConstant;
				for (int i = 0; i < m_nSkinningBones; ++i)
				{
					XMStoreFloat4x4(&tmpBoneOffsetConstant->BoneOffset[i], XMMatrixTranspose(XMLoadFloat4x4(&m_vxmf4x4BindPoseBoneOffsets[i])));
				}
				m_BindPoseBoneOffsetCB->CopyData(0, *tmpBoneOffsetConstant);
				delete tmpBoneOffsetConstant;
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

				m_vxmn4BoneIndices.clear();
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

void SkinnedMesh::UpdateBoneTransformBuffer(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// BindPoseBoneOffset 상수 버퍼 연결
	if (m_BindPoseBoneOffsetCB != nullptr)
	{
		D3D12_GPU_VIRTUAL_ADDRESS BoneOffsetsGpuVirtualAddress = m_BindPoseBoneOffsetCB->Resource()->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(m_nBoneOffsetParameterIdx, BoneOffsetsGpuVirtualAddress);
	}

	// SkinningBoneTransform 상수 버퍼 연결
	if (m_SkinningBoneTransformCB != nullptr)
	{
		D3D12_GPU_VIRTUAL_ADDRESS SkinnginBoneTransformGpuVirtualAddress = m_SkinningBoneTransformCB->Resource()->GetGPUVirtualAddress();
		// 루트 서명에 상수 버퍼 연결
		pd3dCommandList->SetGraphicsRootConstantBufferView(m_nBoneTransformParameterIdx, SkinnginBoneTransformGpuVirtualAddress);

		SkinningBoneTransformConstant* tmpBoneTransformConstant = new SkinningBoneTransformConstant;
		for (int i = 0; i < m_nSkinningBones; ++i)
		{
			XMStoreFloat4x4(&tmpBoneTransformConstant->BoneTransform[i], XMMatrixTranspose(XMLoadFloat4x4(&m_vpSkinningBoneFrameCaches[i]->GetWorld())));
		}
		if (m_SkinningBoneTransformCB) m_SkinningBoneTransformCB->CopyData(0, *tmpBoneTransformConstant);
		delete tmpBoneTransformConstant;
	}
}

void SkinnedMesh::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UpdateBoneTransformBuffer(pd3dCommandList);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[7] =
	{ m_PositionBufferView, m_NormalBufferView,m_TexC0BufferView, m_TangentBufferView,
		m_BiTangentBufferView, m_BoneIndexBufferView, m_BoneWeightBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 7, pVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

}