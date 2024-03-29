#include "../Header/Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	BuildShadersAndInputLayout();
	BuildRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice);

	return true;
}

void Shader::ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	pd3dCommandList->SetPipelineState(m_PSO.Get());
	g_curShader = m_Type;
}

bool Shader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "defaultVS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "defaultPS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool Shader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL);


	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

bool Shader::BuildPSO(ID3D12Device* pd3dDevice)
{
	D3D12_RENDER_TARGET_BLEND_DESC transparancyblendDesc;
	transparancyblendDesc.BlendEnable = true;
	transparancyblendDesc.LogicOpEnable = false;
	transparancyblendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparancyblendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparancyblendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparancyblendDesc.SrcBlendAlpha = D3D12_BLEND_ZERO;
	transparancyblendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparancyblendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparancyblendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparancyblendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.BlendState.RenderTarget[0] = transparancyblendDesc;
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


std::array<const CD3DX12_STATIC_SAMPLER_DESC, SAMPLER_CNT> Shader::GetStaticSampler()
{
	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		0, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		1, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC shadow(
		2, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	return {
		anisotropicWrap, anisotropicClamp, shadow };
}

//////////////////////////////////////////////////////////

TextureMeshShader::TextureMeshShader()
{
	m_Type = ShaderType::Shader_TextureMesh;
}

TextureMeshShader::~TextureMeshShader()
{
}

bool TextureMeshShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "TextureVS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\default.hlsl", nullptr, "TexturePS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool TextureMeshShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[6];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 2, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameter[5].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_ALL);

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

// 은면 셰이더
BackfaceShader::BackfaceShader()
{
	m_Type = ShaderType::Shader_BackFace;
}

BackfaceShader::~BackfaceShader()
{
}

bool BackfaceShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\Backface.hlsl", nullptr, "VS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\Backface.hlsl", nullptr, "PS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool BackfaceShader::BuildPSO(ID3D12Device* pd3dDevice)
{
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// 은면을 그린다.
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
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

bool BackfaceShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[6];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 2, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameter[5].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_ALL);

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

//////////////////////////////////////////////////////////

SkinnedMeshShader::SkinnedMeshShader()
{
	m_Type = ShaderType::Shader_Skinned;
}

SkinnedMeshShader::~SkinnedMeshShader()
{
}

bool SkinnedMeshShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\SkinnedMesh.hlsl", nullptr, "VSSkinnedMesh", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\SkinnedMesh.hlsl", nullptr, "PSSkinnedMesh", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool SkinnedMeshShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[8];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 2, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameter[5].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameter[6].InitAsConstantBufferView(1);	// BoneOffsets 상수 버퍼 
	slotRootParameter[7].InitAsConstantBufferView(2);	// BoneTransforms 상수 버퍼 

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

//////////////////////////////////////////////////////////

ImageObjectShader::ImageObjectShader()
{
	m_Type = ShaderType::Shader_Image;
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

bool ImageObjectShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	slotRootParameter[0].InitAsConstants(32, 0); // 변환 행렬 상수 
	slotRootParameter[1].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL);
	//slotRootParameter[2].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_ALL);

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));

	return true;
}

bool ImageObjectShader::BuildPSO(ID3D12Device* pd3dDevice)
{
	D3D12_RENDER_TARGET_BLEND_DESC transparancyblendDesc;
	transparancyblendDesc.BlendEnable = true;
	transparancyblendDesc.LogicOpEnable = false;
	transparancyblendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparancyblendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparancyblendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparancyblendDesc.SrcBlendAlpha = D3D12_BLEND_ZERO;
	transparancyblendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparancyblendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparancyblendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparancyblendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.BlendState.RenderTarget[0] = transparancyblendDesc;
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

void ImageObjectShader::OnResize(float aspectRatio)
{
	//if (m_vpImgObjects.size() > 0)
	//{
	//	for (int i = 0; i < m_vpImgObjects.size(); ++i)
	//	{
	//		if (m_vpImgObjects[i])
	//		{
	//			m_vpImgObjects[i]->OnResize();
	//		}
	//	}
	//}
}

//void ImageObjectShader::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	if (m_vpImgObjects.size() > 0)
//	{
//		OnPrepareRender(pd3dCommandList);
//
//		for (int i = 0; i < m_vpImgObjects.size(); ++i)
//		{
//			if (m_vpImgObjects[i])
//			{
//				m_vpImgObjects[i]->Render(gt, pd3dCommandList);
//			}
//		}
//	}
//}
//
//bool ImageObjectShader::CreateImgObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nScreenWidth, int nScreenHeight, const wchar_t* pstrTextureFileName, int nBitmapWidth, int nBitmapHeight)
//{
//	bool bResult;
//
//	m_vpImgObjects.emplace_back(std::make_shared<ImgObject>());
//	bResult = m_vpImgObjects.back()->Initialize(pd3dDevice, pd3dCommandList, 
//		nScreenWidth, nScreenHeight, pstrTextureFileName, nBitmapWidth, nBitmapHeight);
//	if (!bResult)
//		return false;
//
//	return true;
//}

//////////////////////////////////////////////////////////

WireFrameShader::WireFrameShader()
{
	m_Type = ShaderType::Shader_WireFrame;
}

WireFrameShader::~WireFrameShader()
{
}

bool WireFrameShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\WireFrameShader.hlsl", nullptr, "VS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\WireFrameShader.hlsl", nullptr, "PS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool WireFrameShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	//CD3DX12_DESCRIPTOR_RANGE texTable;
	//texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstants(1,4);	// 재질 버퍼


	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

bool WireFrameShader::BuildPSO(ID3D12Device* pd3dDevice)
{
	// 와이어 프레임으로 렌더링 이외는 모두 동일

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));

	return true;
}

//////////////////////////////////////////////////////////
// static 메시라 가정

DepthMapShader::DepthMapShader()
{
	m_Type = ShaderType::Shader_DepthMap;
}

DepthMapShader::~DepthMapShader()
{
}

bool DepthMapShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\Shadow.hlsl", nullptr, "VS", "vs_5_1");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\Shadow.hlsl", nullptr, "PS", "ps_5_1");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool DepthMapShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	//CD3DX12_DESCRIPTOR_RANGE texTable;
	//texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	//slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_ALL);

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

bool DepthMapShader::BuildPSO(ID3D12Device* pd3dDevice)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
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
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	psoDesc.RasterizerState.DepthBias = 100000;
	psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	psoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	psoDesc.NumRenderTargets = 0;
	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));

	return true;
}

// 잘린 오브젝트 
// 기존 셰이더에서 변경점
// 은면 렌더링, 알파 블랜딩
// 평면 법선 벡터, 평면 위치, 렌더링 위치 추가 전달

// 스테틱 메시 렌더링

CuttedStaticMeshShader::CuttedStaticMeshShader()
{
	m_Type = ShaderType::Shader_CuttedStatic;
}

CuttedStaticMeshShader::~CuttedStaticMeshShader()
{
}

bool CuttedStaticMeshShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\CuttedStaticMesh.hlsl", nullptr, "VS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\CuttedStaticMesh.hlsl", nullptr, "PS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool CuttedStaticMeshShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[6];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL); // 그림자 맵, Dissolve 텍스쳐
	slotRootParameter[5].InitAsConstantBufferView(6);	// 절단 버퍼

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

bool CuttedStaticMeshShader::BuildPSO(ID3D12Device* pd3dDevice)
{
	D3D12_RENDER_TARGET_BLEND_DESC transparancyblendDesc;
	transparancyblendDesc.BlendEnable = true;	
	transparancyblendDesc.LogicOpEnable = false;
	transparancyblendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparancyblendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparancyblendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparancyblendDesc.SrcBlendAlpha = D3D12_BLEND_ZERO;
	transparancyblendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparancyblendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparancyblendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparancyblendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.BlendState.RenderTarget[0] = transparancyblendDesc;
	
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

// 텍스쳐 메시 렌더링

CuttedTextureMeshShader::CuttedTextureMeshShader()
{
	m_Type = ShaderType::Shader_CuttedTextureMesh;
}

CuttedTextureMeshShader::~CuttedTextureMeshShader()
{
}

bool CuttedTextureMeshShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\CuttedStaticMesh.hlsl", nullptr, "TextureVS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\CuttedStaticMesh.hlsl", nullptr, "TexturePS", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool CuttedTextureMeshShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[7];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 2, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL);	// 그림자 맵
	slotRootParameter[5].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_ALL); // 메시 텍스쳐
	slotRootParameter[6].InitAsConstantBufferView(6);	// 절단 버퍼

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

// 스키닝 메시 셰이더

CuttedSkinnedMeshShader::CuttedSkinnedMeshShader()
{
	m_Type = ShaderType::Shader_CuttedSkinned;
}

CuttedSkinnedMeshShader::~CuttedSkinnedMeshShader()
{
}

bool CuttedSkinnedMeshShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\CuttedSkinnedMesh.hlsl", nullptr, "VSSkinnedMesh", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\CuttedSkinnedMesh.hlsl", nullptr, "PSSkinnedMesh", "ps_5_0");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool CuttedSkinnedMeshShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[9];

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 2, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameter[5].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameter[6].InitAsConstantBufferView(1);	// BoneOffsets 상수 버퍼 
	slotRootParameter[7].InitAsConstantBufferView(2);	// BoneTransforms 상수 버퍼 
	slotRootParameter[8].InitAsConstantBufferView(6);	// 절단 버퍼

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

CuttedDepthMapShader::CuttedDepthMapShader()
{
	m_Type = ShaderType::Shader_CuttedDepthMap;
}

CuttedDepthMapShader::~CuttedDepthMapShader()
{
}

bool CuttedDepthMapShader::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shader\\Shadow_Cutted.hlsl", nullptr, "VS", "vs_5_1");
	m_psByteCode = d3dUtil::CompileShader(L"Shader\\Shadow_Cutted.hlsl", nullptr, "PS", "ps_5_1");

	m_vInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	return true;
}

bool CuttedDepthMapShader::BuildRootSignature(ID3D12Device* pd3dDevice)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	//CD3DX12_DESCRIPTOR_RANGE texTable;
	//texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0);

	slotRootParameter[0].InitAsConstants(32, 0);		// 월드 변환 행렬	// 오브젝트 상수 버퍼 
	slotRootParameter[1].InitAsConstantBufferView(3);	// 패스 버퍼
	slotRootParameter[2].InitAsConstantBufferView(4);	// 재질 버퍼
	slotRootParameter[3].InitAsConstantBufferView(5);	// 오브젝트 버퍼
	slotRootParameter[4].InitAsConstantBufferView(6);	// 절단 버퍼
	//slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_ALL);

	// 샘플러
	auto staticSamplers = GetStaticSampler();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
	return true;
}

bool CuttedDepthMapShader::BuildPSO(ID3D12Device* pd3dDevice)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_vInputLayout.data(), (UINT)m_vInputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
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
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	psoDesc.RasterizerState.DepthBias = 100000;
	psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	psoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	psoDesc.NumRenderTargets = 0;
	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));

	return true;
}
