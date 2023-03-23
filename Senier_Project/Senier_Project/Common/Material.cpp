#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

bool Material::BuildDescriptorHeap(ID3D12Device* pd3dDevice)
{
	if (m_vpTextures.size() < 1)
		return false;

	//mMatCB = std::make_unique<UploadBuffer<tmpMatConstant>>(pd3dDevice, 1, true);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_vpTextures.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	UINT CbvSrvUavDescriptorSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (size_t i = 0; i < m_vpTextures.size(); ++i)
	{
		auto tex = m_vpTextures[i]->Resource;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = tex->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = tex->GetDesc().MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		pd3dDevice->CreateShaderResourceView(tex.Get(), &srvDesc, hDescriptor);

		hDescriptor.Offset(1, CbvSrvUavDescriptorSize);
	}


	return true;
}

void Material::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_DescriptorHeap == NULL)
		return;

	ID3D12DescriptorHeap* descriptorHeap[] = { m_DescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE matHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, matHandle);

}

void Material::LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::wstring texFileName)
{
	auto texMap = std::make_unique<Texture>();
	texMap->FileName = texFileName;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		pd3dDevice, pd3dCommandList,
		texMap->FileName.c_str(),
		texMap->Resource, texMap->UploadHeap));

	m_vpTextures.emplace_back(std::move(texMap));
}

void Material::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	if (strcmp(pstrTextureName, "null"))
	{
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		wchar_t conStr[64];
		mbstowcs_s(&nConverted, conStr, 64, pstrFilePath, _TRUNCATE);

		LoadTexture(pd3dDevice, pd3dCommandList, conStr);


		// 이미 로드한 텍스쳐의 경우 부모 객체로부터 텍스쳐를 가져올 수 있도록

		/*if (!bDuplicated)
		{
			LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, m_ppstrTextureNames[nIndex], RESOURCE_TEXTURE2D, nIndex);
			pShader->CreateShaderResourceView(pd3dDevice, this, nIndex);
		}
		else
		{
			if (pParent)
			{
				CGameObject* pRootGameObject = pParent;
				while (pRootGameObject)
				{
					if (!pRootGameObject->m_pParent) break;
					pRootGameObject = pRootGameObject->m_pParent;
				}
				D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle;
				int nParameterIndex = pRootGameObject->FindReplicatedTexture(m_ppstrTextureNames[nIndex], &d3dSrvGpuDescriptorHandle);
				if (nParameterIndex >= 0)
				{
					m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
					m_pnRootParameterIndices[nIndex] = nParameterIndex;
				}
			}
		}*/
	}

}
