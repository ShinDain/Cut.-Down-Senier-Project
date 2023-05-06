#include "../Header/Scene.h"

#define CHARACTER_MADEL_PATH "Model/"
#define CUBE_MODEL_PATH "Model/Cube.bin"
#define CUBE_MODEL_EXTENTS XMFLOAT3(0.5f, 0.5f, 0.5f)

Scene::Scene()
{
	m_CollisionData.Reset(MAX_CONTACT_CNT);
	m_pCollisionResolver = std::make_unique<CollisionResolver>(MAX_CONTACT_CNT * 8);
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 패스 버퍼 생성
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	ObjectInitData objectData;
	objectData.pstrFilePath = CUBE_MODEL_PATH;
	objectData.xmf3Extents = CUBE_MODEL_EXTENTS;
	objectData.xmf3Position = XMFLOAT3(0, 20, 0);
	objectData.xmf4Orientation = XMFLOAT4(0, 0, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(10, 10, 10);
	objectData.nMass = 1;
	objectData.colliderType = Collider_Box;
	objectData.objectType = Object_Physics;

	// 복수의 박스 생성
	for (int i = 0; i < 100; ++i)
	{
		//objectData.xmf3Position = XMFLOAT3(i * 20 - 40, 20, 0);
		objectData.xmf3Position = XMFLOAT3(-50 + 10 * (i / 3), 5 + (i % 3) * 10, 0);

		CreateObject(pd3dDevice, pd3dCommandList, objectData, 0, RenderLayer::Static);
	}

	// 플랫폼 테스트
	objectData.xmf3Position = XMFLOAT3(500, 25, 0);
	objectData.objectType = Object_Platform;
	objectData.xmf3Scale = XMFLOAT3(20, 1, 20);
	//CreateObject(pd3dDevice, pd3dCommandList, objectData, 0, RenderLayer::Static);
	//m_vpAllObjs[50]->GetBody()->SetVelocity(XMFLOAT3(-120, 0, 0));


	// 바닥
	objectData.xmf3Extents = XMFLOAT3(0, 0, 0);
	objectData.xmf3Position = XMFLOAT3(0,0,0);
	objectData.xmf3Scale = XMFLOAT3(100, 0.1f, 100);
	objectData.colliderType = Collider_Plane;
	objectData.objectType = Object_World;
	CreateObject(pd3dDevice, pd3dCommandList, objectData, 0, RenderLayer::Static);
	
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->UpdateTransform(NULL);
	}

	// 카메라 초기화
	//m_pCamera = std::make_unique<Third_Person_Camera>(m_vpAllObjs[0]);
	m_pCamera = std::make_unique<Camera>();
	m_pCamera->SetPosition(0, 30, -100);
	m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);
	
#if defined(_DEBUG)



#endif

	return true;
}

void Scene::OnResize(float aspectRatio)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);
}

void Scene::Update(float elapsedTime)
{
#if defined(_DEBUG)
	ClearObjectLayer();
	m_refCnt = m_LoadedModelData[CUBE_MODEL_PATH]->m_pRootObject.use_count();
	m_size = m_vpAllObjs.size();

	m_tTime += elapsedTime;

#endif

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->Update(elapsedTime);
	}

	// Contact 데이터 생성
	// Collider 객체를 타입별로 분류
	GenerateContact();
	// Collision Resolve
	m_pCollisionResolver->ResolveContacts(m_CollisionData.pContacts, elapsedTime);


	m_pCamera->Update(elapsedTime);

	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX viewProj = XMMatrixMultiply(view, m_pCamera->GetProj());

	// 패스 버퍼 : 뷰 * 투영 변환 행렬 업데이트
	PassConstant passConstant;
	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	m_pPassCB->CopyData(0, passConstant);

	// ImageObject 렌더를 위한  직교 투영행렬 업데이트
	m_xmf4x4ImgObjMat = m_pCamera->GetOrtho4x4f();

}

void Scene::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// img 오브젝트 렌더링 수정 필
	{
		g_Shaders[RenderLayer::Image]->ChangeShader(pd3dCommandList);
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);
	}

	g_Shaders[RenderLayer::Static]->ChangeShader(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i])
		{
			// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
			m_vpAllObjs[i]->Animate(elapsedTime);
			if (!m_vpAllObjs[i]->m_pAnimationController)
				m_vpAllObjs[i]->UpdateTransform(NULL);
			m_vpAllObjs[i]->Render(elapsedTime, pd3dCommandList);
		}
	}
}

void Scene::ProcessInput(UCHAR* pKeybuffer)
{
	float dx, dy;
	dx = dy = 0;

	POINT ptCursorPos;

	if (pKeybuffer[VK_LBUTTON] & 0xF0)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
		dx = (float)(ptCursorPos.x - CLIENT_WIDTH / 2) / 3.0f;
		dy = (float)(ptCursorPos.y - CLIENT_HEIGHT / 2) / 3.0f;
		//m_LastMousePos = ptCursorPos;
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
	}

	DWORD dwDirection = 0;
	if (pKeybuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeybuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeybuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeybuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;

	if(dwDirection != 0 || dx != 0 || dy != 0)
	{
		if (dx != 0 || dy != 0)
		{
			//m_vpAllObjs[0]->Rotate(0,-dx, 0);

			m_pCamera->Pitch(dy / 1000);
			m_pCamera->RotateY(dx / 1000);
		}

		//m_vpAllObjs[0]->Move(dwDirection, m_vpAllObjs[0]->GetAcceleration());
	}

#if defined(_DEBUG)
	/*if (pKeybuffer[VK_UP] & 0xF0) {
		Physics::CalculateImpulse(m_vpAllObjs[0].get(), m_vpAllObjs[1].get(), XMFLOAT3(0, 1, 0), XMFLOAT3(0, -5, 0), XMFLOAT3(0, -5, 0), 1, 0.5f);
	}
	if (pKeybuffer[VK_DOWN] & 0xF0) {
		Physics::CalculateImpulse(m_vpAllObjs[0].get(), m_vpAllObjs[1].get(), XMFLOAT3(0, -1, 0), XMFLOAT3(0, 5, 0), XMFLOAT3(0, 5, 0), 1, 0.5f);
	}
	if (pKeybuffer[VK_LEFT] & 0xF0) {
		Physics::CalculateImpulse(m_vpAllObjs[0].get(), m_vpAllObjs[1].get(), XMFLOAT3(-1, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), 1, 0.5f);
	}
	if (pKeybuffer[VK_RIGHT] & 0xF0) {
		Physics::CalculateImpulse(m_vpAllObjs[0].get(), m_vpAllObjs[1].get(), XMFLOAT3(1, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), 1, 0.5f);
	}*/
#endif
}


std::shared_ptr<Object> Scene::CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
											const ObjectInitData& objInitData, int nAnimationTracks, RenderLayer renderLayer)
{
	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> pObject;
	const char* pstrFilePath = objInitData.pstrFilePath;

	if (m_LoadedModelData.find(pstrFilePath) == m_LoadedModelData.end())
	{
		// 모델 로드
		pModelData = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, pstrFilePath);

		m_LoadedModelData.insert({ pstrFilePath, pModelData });
	}
	else	// 이미 로드한 모델인 경우
	{
		pModelData = m_LoadedModelData[pstrFilePath];
	}

	// RigidBody 생성
	std::shared_ptr<RigidBody> pBody = 
		std::make_shared<RigidBody>(objInitData.xmf3Position, objInitData.xmf4Orientation, objInitData.xmf3Scale, objInitData.nMass);

	// Object 타입에 따라 변수 조정
	switch (objInitData.objectType)
	{
	case Object_World:
		pBody->SetPhysics(false);
		pBody->SetInGravity(false);
		break;

	case Object_Physics:
		pBody->SetPhysics(true);
		pBody->SetInGravity(true);
		break;

	case Object_Platform:
		pBody->SetPhysics(true);
		pBody->SetInGravity(false);
		pBody->SetIsPlatform(true);
		break;

	case Object_Character:
		pBody->SetPhysics(true);
		pBody->SetInGravity(true);
		pBody->SetIsCharacter(true);
		break;

	case Object_UI:
		pBody->SetPhysics(false);
		pBody->SetInGravity(false);
		break;

	default:
		break;
	}

	// 충돌체 타입에 따라 
	
	switch (objInitData.colliderType)
	{
	case Collider_Plane:
		{
			std::shared_ptr<ColliderPlane> pCollider;
			pCollider = std::make_shared<ColliderPlane>(pBody.get(), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0), objInitData.xmf3Extents.x);
			m_ppColliderPlanes.emplace_back(pCollider);
			pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, pBody, pCollider, pModelData, nAnimationTracks);
		}
		break;

	case Collider_Box:
		{
			std::shared_ptr<ColliderBox> pCollider;
			pCollider = std::make_shared<ColliderBox>(pBody.get(), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), objInitData.xmf3Extents);
			m_ppColliderBoxs.emplace_back(pCollider);
			pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, pBody, pCollider, pModelData, nAnimationTracks);
		}
		break;

	case Collider_Sphere:
		{
			std::shared_ptr<ColliderSphere> pCollider;
			pCollider = std::make_shared<ColliderSphere>(pBody.get(), XMFLOAT3(0, 0, 0), objInitData.xmf3Extents.x);
			m_ppColliderSpheres.emplace_back(pCollider);
			pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, pBody, pCollider, pModelData, nAnimationTracks);
		}
		break;

	default:
		break;
	}

	m_vpAllObjs.emplace_back(pObject);
	m_vObjectLayer[renderLayer].emplace_back(pObject);


	return pObject;
}

void Scene::GenerateContact()
{
	unsigned int nContactCnt = MAX_CONTACT_CNT * 8;

	m_CollisionData.Reset(nContactCnt);
	m_CollisionData.friction = 0.9f;
	m_CollisionData.restitution = 0.1f;
	m_CollisionData.tolerance = 0.1f;

	// 평면과의 검사
	for (int i = 0; i < m_ppColliderBoxs.size(); ++i)
	{
		for (int j = 0; j < m_ppColliderPlanes.size(); ++j)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*m_ppColliderBoxs[i], *m_ppColliderPlanes[j], m_CollisionData);

		}
	}

	m_CollisionData.friction = 1.0f;
	// 박스끼리 검사
	for (int i = 0; i < m_ppColliderBoxs.size() - 1; ++i)
	{
		for (int j = i + 1; j < m_ppColliderBoxs.size(); ++j)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;

			CollisionDetector::BoxAndBox(*m_ppColliderBoxs[i], *m_ppColliderBoxs[j], m_CollisionData);
		}
	}
}

void Scene::ClearObjectLayer()
{
	bool bFind = false;
	for (int j = 0; j < m_ppColliderBoxs.size(); ++j)
	{
		if (bFind) break;
		if (m_ppColliderBoxs[j]->GetBody()->GetInvalid())
		{
			m_ppColliderBoxs.erase(m_ppColliderBoxs.begin() + j);
			bFind = true;
		}
	}
	for (int j = 0; j < m_ppColliderSpheres.size(); ++j)
	{
		if (m_ppColliderSpheres[j]->GetBody()->GetInvalid())
		{
			m_ppColliderSpheres.erase(m_ppColliderSpheres.begin() + j);
			bFind = true;
		}
	}
	for (int j = 0; j < m_ppColliderPlanes.size(); ++j)
	{
		if (bFind) break;
		if (m_ppColliderPlanes[j]->GetBody()->GetInvalid())
		{
			m_ppColliderPlanes.erase(m_ppColliderPlanes.begin() + j);
			bFind = true;
		}
	}

	// 전체 순회
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (!m_vpAllObjs[i]->GetIsAlive())
		{
			m_vpAllObjs[i]->Destroy();
			m_vpAllObjs.erase(m_vpAllObjs.begin() + i);
		}
	}

	// 레이어 순회
	for (int i = 0; i < RenderLayer::Count; ++i)
	{
		for (int j = 0; j < m_vObjectLayer[i].size(); ++j)
		{
			if (!m_vObjectLayer[i][j]->GetIsAlive())
			{
				m_vObjectLayer[i].erase(m_vObjectLayer[i].begin() + j);
			}
		}
	}


}
