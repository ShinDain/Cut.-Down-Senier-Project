#include "../Header/DepthMap.h"

DepthMap::DepthMap(ID3D12Device* pd3dDevice, UINT width, UINT height)
{
	m_d3dDevice = pd3dDevice;

	m_Width = width;
	m_Height = height;

	m_Viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	m_ScissorRect = { 0,0,(int)width, (int)height };

	BuildResource();
}

UINT DepthMap::Width() const
{
	return m_Width;
}

UINT DepthMap::Height() const
{
	return m_Height;
}

ID3D12Resource* DepthMap::Resource()
{
	return m_DepthMap.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DepthMap::CpuSrv() const
{
	return m_hCpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DepthMap::Srv() const
{
	return m_hGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DepthMap::Dsv() const
{
	return m_hCpuDsv;
}

D3D12_VIEWPORT DepthMap::Viewport() const
{
	return m_Viewport;
}

D3D12_RECT DepthMap::ScissorRect() const
{
	return m_ScissorRect;
}

void DepthMap::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv)
{
	m_hCpuSrv = hCpuSrv;
	m_hGpuSrv = hGpuSrv;
	m_hCpuDsv = hCpuDsv;

	BuildDescriptors();
}

void DepthMap::OnResize(UINT newWidth, UINT newHeight)
{
	if ((m_Width != newWidth) || (m_Height != newHeight))
	{
		m_Width = newWidth;
		m_Height = newHeight;

		BuildResource();

		BuildDescriptors();
	}

}

void DepthMap::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	m_d3dDevice->CreateShaderResourceView(m_DepthMap.Get(), &srvDesc, m_hCpuSrv);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3dDevice->CreateDepthStencilView(m_DepthMap.Get(), &dsvDesc, m_hCpuDsv);

}

void DepthMap::BuildResource()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));

	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = m_Width;
	texDesc.Height = m_Height;
	texDesc.Format = m_Format;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES defalutHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&defalutHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&m_DepthMap)));
}
