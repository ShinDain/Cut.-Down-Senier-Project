#include "../Header/Shader.h"
#include "../Header/Object.h"
#include "../Header/Material.h"

Material::Material()
{
}

Material::~Material()
{
	for(int i = 0 ; i < m_vpTextures.size(); ++i)
		m_vpTextures;

	m_DescriptorHeap = nullptr;
	m_pMatCB = nullptr;
	m_pShader = nullptr;
}

bool Material::BuildDescriptorHeap(ID3D12Device* pd3dDevice)
{
	m_pMatCB = std::make_unique<UploadBuffer<MatConstant>>(pd3dDevice, 1, true);
	MatConstant materialConstant;
	materialConstant.AlbedoColor = m_xmf4AlbedoColor;
	m_pMatCB->CopyData(0, materialConstant);

	if (m_bIsNonTextureMat)
	{
		SetShader(g_Shaders[ShaderType::Shader_Static]);
		return false;
	}
	

	for (int i = 0; i < m_strTextureName.size(); ++i)
	{
		if (wcscmp(m_strTextureName[i].c_str(), L"null"))
			m_vpTextures.emplace_back(FindReplicatedTexture(m_strTextureName[i].c_str()));
		else
			m_vpTextures.emplace_back(nullptr);
	}

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_vpTextures.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	UINT CbvSrvUavDescriptorSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	for (size_t i = 0; i < m_vpTextures.size(); ++i)
	{
		if (m_vpTextures[i])
		{
			auto tex = m_vpTextures[i]->Resource;

			srvDesc.Format = tex->GetDesc().Format;
			srvDesc.Texture2D.MipLevels = tex->GetDesc().MipLevels;

			pd3dDevice->CreateShaderResourceView(tex.Get(), &srvDesc, hDescriptor);
			hDescriptor.Offset(1, CbvSrvUavDescriptorSize);
		}
		else
		{
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.Texture2D.MipLevels = 1;
			pd3dDevice->CreateShaderResourceView(nullptr, &srvDesc, hDescriptor);
			hDescriptor.Offset(1, CbvSrvUavDescriptorSize);
		}

	}

	return true;
}

void Material::MaterialSet(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pShader && g_curShader != m_nShaderType && g_curShader < ShaderType::Shader_DepthMap)
		m_pShader->ChangeShader(pd3dCommandList);

	if(m_pMatCB) pd3dCommandList->SetGraphicsRootConstantBufferView(m_nMatCBParameterIdx, m_pMatCB->Resource()->GetGPUVirtualAddress());

	if (g_curShader == ShaderType::Shader_DepthMap)
		return;

	if (m_DescriptorHeap == nullptr)
	{
		return;
	}
	else
	{
		ID3D12DescriptorHeap* descriptorHeap[] = { m_DescriptorHeap.Get() };
		pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

		D3D12_GPU_DESCRIPTOR_HANDLE matHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		pd3dCommandList->SetGraphicsRootDescriptorTable(m_nDescTableParameterIdx, matHandle);
	}
}

bool Material::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	FILE* pInFile, Object* pRootObject, const char* pstrFileName, const char* pstrTexPath)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 128;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	if (strcmp(pstrTextureName, "null"))
	{
		char pstrFilePath[128] = { '\0' };
		strcpy_s(pstrFilePath, 128, "Model/Textures/");

		strcat_s(pstrFilePath, pstrTexPath);
		strcat_s(pstrFilePath, "/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcat_s(pstrFilePath, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcat_s(pstrFilePath, ".dds");

		size_t nConverted = 0;
		wchar_t conStr[128];
		mbstowcs_s(&nConverted, conStr, 128, pstrFilePath, _TRUNCATE);

		m_strTextureName.push_back(conStr);

		if (!bDuplicated)
		{
			LoadTexture(pd3dDevice, pd3dCommandList, conStr);
		}
		return true;
	}
	else
	{
		m_strTextureName.push_back(L"null");
		return false;
	}
}
