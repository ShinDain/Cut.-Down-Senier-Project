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
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Microsoft::WRL::ComPtr<ID3DBlob> m_VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader = nullptr;

public:

	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void OnprepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile) {};

	virtual void OnPostRender() {};

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

	void DisposeUploaders();

protected:
	char m_Name[64];

	UINT m_VertexByteStride = 0;
	UINT m_VertexBufferByteSize = 0;
	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT m_IndexBufferByteSize = 0;

	std::vector<SubmeshGeometry> m_DrawArgs;

	int m_nVertices = 0;

public:
	void SetMeshName(const char* str)						{ strcpy_s(m_Name, str); }
	void SetVertexByteStride(UINT vertexByteStride)			{ m_VertexByteStride = vertexByteStride; }
	void SetVertexBufferByteSize(UINT vertexBufferByteSize) { m_VertexBufferByteSize = vertexBufferByteSize; }
	void SetIndexFormat(DXGI_FORMAT indexFormat)			{ m_IndexFormat = indexFormat; }
	void SetIndexBufferByteSize(UINT indexBufferByteSize)	{ m_IndexBufferByteSize = indexBufferByteSize; }
	void SetVertexCnt(int nVertices)						{ m_nVertices = nVertices; }

	const char* GetMeshName()				{ return m_Name; }
	const UINT GetVertexByteStride()		{ return m_VertexByteStride; }
	const UINT GetVertexBufferByteSize()	{ return m_VertexBufferByteSize; }
	const DXGI_FORMAT GetIndexFormat()		{ return m_IndexFormat; }
	const UINT GetIndexBufferByteSize()		{ return m_IndexBufferByteSize; }
	const int GetVertexCnt()				{ return m_nVertices; }

};



