#pragma once

// 장면에 삽입될 오브젝트 초기화 및 관리

#include <algorithm>
#include "Camera.h"
#include "../../Common/Header/MathHelper.h"
#include "Global.h"
#include "Shader.h"
#include "Object.h"

#include "../../Physics/Header/Contact.h"
#include "../../Physics/Header/Collider.h"
#include "../../Physics/Header/CollisionDetact.h"
#include "../../Physics/Header/CollisionResolver.h"


class Scene
{
public:
	Scene();
	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;
	virtual ~Scene();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnResize(float aspectRatio);
	virtual void Update(float elapsedTime);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	void ProcessInput(UCHAR* pKeybuffer);

	std::shared_ptr<Object> CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
										const ObjectInitData& objInitData, int nAnimationTracks, RenderLayer renderLayer);

	void GenerateContact();
	void ClearObjectLayer();
private:

	XMFLOAT4X4 m_xmf4x4ViewProj = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4ImgObjMat = MathHelper::identity4x4();

	// 프레임마다 넘겨줄 상수 버퍼
	std::unique_ptr<UploadBuffer<PassConstant>> m_pPassCB = nullptr;

	// 오브젝트 객체들
	std::vector<std::shared_ptr<Object>> m_vpAllObjs;
	std::vector<std::shared_ptr<Object>> m_vObjectLayer[(int)RenderLayer::Count];

	// 씬을 렌더링할 메인 카메라
	std::unique_ptr<Camera> m_pCamera = nullptr;


	POINT m_LastMousePos = { 0,0 };
public:
	void SetViewProjMatrix(XMFLOAT4X4 viewProj) { m_xmf4x4ViewProj = viewProj; }


private:
	std::map<const char*, std::shared_ptr<ModelDataInfo>> m_LoadedModelData;

	CollisionData m_CollisionData;
	std::unique_ptr<CollisionResolver> m_pCollisionResolver = nullptr;

	std::vector<std::shared_ptr<ColliderPlane>> m_ppColliderPlanes;
	std::vector<std::shared_ptr<ColliderBox>> m_ppColliderBoxs;
	std::vector<std::shared_ptr<ColliderSphere>> m_ppColliderSpheres;

	

#if defined(_DEBUG) | defined(DEBUG)
public:

	int m_refCnt = 0;
	int m_size = 0;
	
	float m_tTime = 0;
#endif
};






