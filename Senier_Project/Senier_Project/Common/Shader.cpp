#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, void* pContext)
{
	BuildShadersAndInputLayout();
	BuildPSO(pd3dDevice, pd3dRootSignature);

	return true;
}

void Shader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetPipelineState(m_PSO.Get());
}

void Shader::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);
}

bool Shader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "VS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "PS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool Shader::BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = pd3dRootSignature;
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));

	return true;
}

//////////////////////////////////////////////////////////

SkinnedMeshShader::SkinnedMeshShader()
{
}

SkinnedMeshShader::~SkinnedMeshShader()
{
}

bool SkinnedMeshShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "VSSkinnedMesh", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "PSSkinnedMesh", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

//////////////////////////////////////////////////////////

ImageObjectShader::ImageObjectShader()
{
}

ImageObjectShader::~ImageObjectShader()
{
}

bool ImageObjectShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\ImageObjShader.hlsl", nullptr, "VS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\ImageObjShader.hlsl", nullptr, "PS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	return true;
}

bool ImageObjectShader::BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
	// 깊이 검사 하지 않는 것 이외는 동일

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = pd3dRootSignature;
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));

	return true;
}

void ImageObjectShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{

	pd3dCommandList->SetPipelineState(m_PSO.Get());
}

void ImageObjectShader::OnResize(float aspectRatio)
{
	if (m_vpImgObjects.size() > 0)
	{
		for (int i = 0; i < m_vpImgObjects.size(); ++i)
		{
			if (m_vpImgObjects[i])
			{
				m_vpImgObjects[i]->OnResize();
			}
		}
	}
}

void ImageObjectShader::Update(const GameTimer& gt)
{
}

void ImageObjectShader::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_vpImgObjects.size() > 0)
	{
		OnPrepareRender(pd3dCommandList);

		for (int i = 0; i < m_vpImgObjects.size(); ++i)
		{
			if (m_vpImgObjects[i])
			{
				m_vpImgObjects[i]->Render(gt, pd3dCommandList);
			}
		}
	}
}

bool ImageObjectShader::CreateImgObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nScreenWidth, int nScreenHeight, const wchar_t* pstrTextureFileName, int nBitmapWidth, int nBitmapHeight)
{
	bool bResult;

	m_vpImgObjects.emplace_back(std::make_shared<ImgObject>());
	bResult = m_vpImgObjects.back()->Initialize(pd3dDevice, pd3dCommandList, 
		nScreenWidth, nScreenHeight, pstrTextureFileName, nBitmapWidth, nBitmapHeight);
	if (!bResult)
		return false;

	return true;
}
