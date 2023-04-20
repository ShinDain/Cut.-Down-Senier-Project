#pragma once

// 화면에 2D로 그려지는 Object

#include "Global.h"
#include "GameTimer.h"
#include "MathHelper.h"

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

	void Update(const GameTimer& gt);
	void OnResize();
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual bool UpdateBuffer(int nPosX, int nPosY);

protected:
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

};