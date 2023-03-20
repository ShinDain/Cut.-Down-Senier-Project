#pragma once
#include "D3DUtil.h"
#include "GameTimer.h"
#include "Global.h"

// 메시 정점/인덱스 버퍼 저장

using namespace DirectX;

class Object;

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh& rhs) = delete;
	Mesh& operator=(const Mesh& rhs) = delete;
	virtual ~Mesh();

protected:
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_PositionBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_ColorBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_ColorBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_ColorBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_TexC0BufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_TexC0BufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_TexC0BufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_TexC1BufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_TexC1BufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_TexC1BufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_NormalBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_NormalBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_NormalBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_TangentBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_TangentBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_TangentBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_BiTangentBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_BiTangentBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_BiTangentBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_IndexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_IndexBufferUploader = nullptr;
	D3D12_INDEX_BUFFER_VIEW					 m_IndexBufferView;

	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R32_UINT;

public:
	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {}

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void OnPostRender() {};

	void DisposeUploaders();

protected:
	char m_Name[64];

	std::vector<SubmeshGeometry> m_vDrawArgs;

	int m_nVertices = 0;

	std::vector<XMFLOAT3> m_vPositions;
	std::vector<XMFLOAT3> m_vColors;
	std::vector<XMFLOAT2> m_vTextureC0;
	std::vector<XMFLOAT2> m_vTextureC1;
	std::vector<XMFLOAT3> m_vNormals;
	std::vector<XMFLOAT3> m_vTangents;
	std::vector<XMFLOAT3> m_vBiTangents;

	std::vector<UINT> m_vIndices;
	
	XMFLOAT3 m_AABBCenter;
	XMFLOAT3 m_AABBExtents;

	void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU,
		Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
		UINT bufferByteSize,
		UINT strideInBytes,
		D3D12_VERTEX_BUFFER_VIEW* pVertexBufferView, void* pData);

	void CreateIndexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU,
		Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
		UINT bufferByteSize,
		D3D12_INDEX_BUFFER_VIEW* pVertexBufferView, void* pData);

public:
	void SetMeshName(const char* str)						{ strcpy_s(m_Name, str); }
	void SetIndexFormat(DXGI_FORMAT indexFormat)			{ m_IndexFormat = indexFormat; }
	void SetVertexCnt(int nVertices)						{ m_nVertices = nVertices; }
	void SetAABBCenter(XMFLOAT3 AABBCenter)					{ m_AABBCenter = AABBCenter; }
	void SetAABBExtents(XMFLOAT3 AABBExtents)				{ m_AABBExtents = AABBExtents; }

	const char* GetMeshName()				{ return m_Name; }
	const DXGI_FORMAT GetIndexFormat()		{ return m_IndexFormat; }
	const int GetVertexCnt()				{ return m_nVertices; }
	const XMFLOAT3 GetAABBCenter()			{ return m_AABBCenter; }
	const XMFLOAT3 GetAABBExtents()			{ return m_AABBExtents; }
};

#define SKINNED_ANIMATION_BONES 256

class SkinnedMesh : public Mesh
{
public:
	SkinnedMesh();
	SkinnedMesh(const SkinnedMesh& rhs) = delete;
	SkinnedMesh& operator=(const SkinnedMesh& rhs) = delete;
	virtual ~SkinnedMesh();

protected:
	std::vector<XMINT4>			m_vxmn4BoneIndices;
	std::vector<XMFLOAT4>		m_vxmf4BoneWeights;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_BoneIndexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_BoneIndexBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW			   m_BoneIndexBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_BoneWeightBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_BoneWeightBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW			   m_BoneWeightBufferView;

	std::vector<char[64]> m_vstrSkinningBoneNames;
	std::vector<std::shared_ptr<Object>> m_vpSkinningBoneFrameCaches;

	std::vector<XMFLOAT4X4> m_vxmf4x4BindPoseBoneOffsets;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_BindPoseBoneOffsetResource = nullptr;
	std::vector<XMFLOAT4X4> m_vxmf4x4MappedBindPoseBoneOffset;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_SkinningBoneTransformResource = nullptr;
	std::vector<XMFLOAT4X4> m_vxmf4x4MappedSkinningBoneTransform;

public:
	void LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	void PrepareSkinning(Object* pModelRootObject);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList) override;

protected:
	int m_nBonesPerVertex = 4;
	int m_nSkinningBones = 0;

public:
	void SetBonesPerVertex(int nBonesPerVertex) { m_nBonesPerVertex = nBonesPerVertex; }
	void SetSkinningBones(int nSkinningBones) { m_nSkinningBones = nSkinningBones; }

	const int GetBonesPerVertex() { return m_nBonesPerVertex; }
	const int GetSkinningBones() { return m_nSkinningBones; }
};

