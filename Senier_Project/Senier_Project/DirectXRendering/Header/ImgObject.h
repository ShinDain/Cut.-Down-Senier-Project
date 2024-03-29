#pragma once

// 화면에 2D로 그려지는 Object

#include "Global.h"
#include "../../Common/Header/MathHelper.h"

// position과 uv좌표만 
class ImgObject
{
public:
	ImgObject();
	ImgObject(const ImgObject& rhs) = delete;
	ImgObject operator=(const ImgObject& rhs) = delete;
	virtual ~ImgObject();

	bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nScreenWidth, int nScreenHeight,
		const wchar_t* pstrTextureFileName, int nBitmapWidth, int nBitmapHeight);

	void Update(float elapsedTime);
	void OnResize();
	void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void ChangePosition(int nPosX, int nPosY);
	virtual void ChangeSize(int nBitmapWidth, int nBitmapHeight);

	bool UpdateBuffer();

protected:
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);

	bool BuildBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool BuildDescriptorHeap(ID3D12Device* pd3dDevice);

private:
	std::unique_ptr<UploadBuffer<XMFLOAT3>> m_DynamicPositionBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DynamicPositionBufferGPU = nullptr;
	D3D12_VERTEX_BUFFER_VIEW			   m_DynamicPositionBufferView;

	std::unique_ptr<UploadBuffer<XMFLOAT2>> m_DynamicTexC0Buffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DynamicTexC0BufferGPU = nullptr;
	D3D12_VERTEX_BUFFER_VIEW			   m_DynamicTexC0BufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader = nullptr;
	D3D12_INDEX_BUFFER_VIEW				   m_IndexBufferView;

	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R16_UINT;


	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;
	std::wstring m_strTextureName;
	std::shared_ptr<Texture> m_pTexture = nullptr;
	UINT m_nDescTableParameterIdx = 1;


	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

protected:
	int m_nScreenWidth	= CLIENT_WIDTH;
	int m_nScreenHeight = CLIENT_HEIGHT;
	int m_nBitmapWidth	= -1;
	int m_nBitmapHeight = -1;
	int m_nPreviousPosX = -1;
	int m_nPreviousPosY = -1;

	int m_nVertexCnt = -1;
	int m_nIndexCnt = -1;

	bool m_bVisible = true;
	bool m_bBufferDirty = false;

public:
	bool GetVisible() { return m_bVisible; }
	int GetBitmapWidth() { return m_nBitmapWidth; }
	int GetBitmapHeight() { return m_nBitmapHeight; }

	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
};