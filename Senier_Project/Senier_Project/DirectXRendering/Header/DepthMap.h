#ifndef DEPTHMAP_H

#define DEPTHMAP_H

#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"

class DepthMap
{
public:
	DepthMap(ID3D12Device* pd3dDevice, UINT width, UINT height);
	DepthMap(const DepthMap& rhs) = delete;
	DepthMap& operator=(const DepthMap* rhs) = delete;
	~DepthMap() = default;

	UINT Width()const;
	UINT Height() const;
	ID3D12Resource* Resource();
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuSrv() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Dsv() const;

	D3D12_VIEWPORT Viewport() const;
	D3D12_RECT ScissorRect() const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

	void OnResize(UINT newWidth, UINT newHeight);

private:
	void BuildDescriptors();
	void BuildResource();

private:
	ID3D12Device* m_d3dDevice = nullptr;

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

	UINT m_Width = 0;
	UINT m_Height = 0;
	DXGI_FORMAT m_Format = DXGI_FORMAT_R24G8_TYPELESS;

	CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE m_hGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuDsv;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthMap = nullptr;
};



#endif