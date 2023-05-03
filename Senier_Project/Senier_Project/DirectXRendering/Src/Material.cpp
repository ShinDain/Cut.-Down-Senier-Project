#include "../Header/Shader.h"
#include "../Header/Object.h"
#include "../Header/Material.h"

Material::Material()
{
}

Material::~Material()
{
}

bool Material::BuildDescriptorHeap(ID3D12Device* pd3dDevice)
{
	if (m_strTextureName.size() < 1)
		return false;

	for (int i = 0; i < m_strTextureName.size(); ++i)
	{
		m_vpTextures.emplace_back(FindReplicatedTexture(m_strTextureName[i].c_str()));
	}

	if (m_vpTextures.size() < 1)
		return false;

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

void Material::MaterialSet(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if(m_pShader)m_pShader->ChangeShader(pd3dCommandList);

	if (m_DescriptorHeap == NULL)
		return;

	ID3D12DescriptorHeap* descriptorHeap[] = { m_DescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE matHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, matHandle);

}

void Material::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, Object* pRootObject)
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
		strcat_s(pstrFilePath, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcat_s(pstrFilePath, ".dds");

		size_t nConverted = 0;
		wchar_t conStr[64];
		mbstowcs_s(&nConverted, conStr, 64, pstrFilePath, _TRUNCATE);

		m_strTextureName.push_back(conStr);

		if (!bDuplicated)
		{
			LoadTexture(pd3dDevice, pd3dCommandList, conStr);
		}
	}
}
