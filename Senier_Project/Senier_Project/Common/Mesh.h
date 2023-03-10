#pragma once
#include "D3DUtil.h"
#include "GameTimer.h"
#include "Global.h"

// 메시 정점/인덱스 버퍼 저장

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh& rhs) = delete;
	Mesh& operator=(const Mesh& rhs) = delete;
	virtual ~Mesh();

protected:

	std::string mName;
	
	D3D12_PRIMITIVE_TOPOLOGY mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Microsoft::WRL::ComPtr<ID3DBlob> mVertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mIndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBufferUploader = nullptr;

	UINT mVertexByteStride = 0;
	UINT mVertexBufferByteSize = 0;
	DXGI_FORMAT mIndexFormat = DXGI_FORMAT_R16_UINT;
	UINT mIndexBufferByteSize = 0;

	std::vector<SubmeshGeometry> mDrawArgs;

	int mnVertices = 0;

public:

	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void OnprepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile) {};

	virtual void OnPostRender() {};

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

	void DisposeUploaders();

	int GetVertexCnt() { return mnVertices; }

};



