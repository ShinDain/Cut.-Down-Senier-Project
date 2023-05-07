#include "../Header/Scene.h"

//#define CHARACTER_MODEL_PATH "Model/unitychan.bin"
#define CHARACTER_MODEL_PATH "Model/BoxUnityChan.bin"
#define CHARACTER_MODEL_EXTENTS XMFLOAT3(0.3f, 0.8f, 0.2f)
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
	objectData.xmf3Position = XMFLOAT3(0, 0, -50);
	objectData.xmf4Orientation = XMFLOAT4(0, 0, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(10, 10, 10);
	objectData.nMass = 10;
	objectData.objectType = Object_Character;
	objectData.colliderType = Collider_Box;
	objectData.xmf3Extents = CHARACTER_MODEL_EXTENTS;

	// 캐릭터 테스트
	CreateObject(pd3dDevice, pd3dCommandList,  objectData, CHARACTER_MODEL_PATH, 1, RenderLayer::Render_Skinned);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 1);



	objectData.xmf3Extents = CUBE_MODEL_EXTENTS;
	objectData.objectType = Object_Physics;
	// 복수의 박스 생성
	for (int i = 0; i < 2; ++i)
	{
		//objectData.xmf3Position = XMFLOAT3(-30 + 10 * (i / 3), 5 + 10 * (i % 3), 30 + 10 * (i / 9));
		objectData.xmf3Position = XMFLOAT3(0, 5 + 20 * i,0);

		CreateObject(pd3dDevice, pd3dCommandList, objectData, CUBE_MODEL_PATH, 0, RenderLayer::Render_Static);
	}


	// 플랫폼 테스트
	objectData.xmf3Position = XMFLOAT3(0, 20, 0);
	objectData.objectType = Object_Platform;
	objectData.xmf3Scale = XMFLOAT3(20, 1, 20);
	//CreateObject(pd3dDevice, pd3dCommandList, objectData, 0, RenderLayer::Render_Static);
	//m_vpAllObjs[1]->GetBody()->SetVelocity(XMFLOAT3(25, 0, 0));




	// 바닥
	objectData.xmf3Extents = XMFLOAT3(0, 0, 0);
	objectData.xmf3Position = XMFLOAT3(0,0,0);
	objectData.xmf4Orientation = XMFLOAT4(0, 1, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(200.0f, 0.1f, 500.0f);
	objectData.colliderType = Collider_Plane;
	objectData.objectType = Object_World;
	CreateObject(pd3dDevice, pd3dCommandList, objectData, CUBE_MODEL_PATH, 0, RenderLayer::Render_Static);
	// 왼쪽 벽
	objectData.xmf3Extents = XMFLOAT3(-100, 0, 0);
	objectData.xmf3Position = XMFLOAT3(-100, 25, 0);
	objectData.xmf4Orientation = XMFLOAT4(1, 0, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(0.1f, 50, 100);
	CreateObject(pd3dDevice, pd3dCommandList, objectData, CUBE_MODEL_PATH, 0, RenderLayer::Render_Static);
	// 오른쪽 벽
	objectData.xmf3Extents = XMFLOAT3(-100, 0, 0);
	objectData.xmf3Position = XMFLOAT3(100, 25, 0);
	objectData.xmf4Orientation = XMFLOAT4(-1, 0, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(0.1f, 50, 100);
	CreateObject(pd3dDevice, pd3dCommandList, objectData, CUBE_MODEL_PATH, 0, RenderLayer::Render_Static);
	
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->UpdateTransform(NULL);
	}

	// 카메라 초기화
	m_pCamera = std::make_unique<Third_Person_Camera>(m_vpAllObjs[0]);
	

#if defined(_DEBUG)
	//m_pCamera = std::make_unique<Camera>();
	//m_pCamera->SetPosition(0, 30, -100);
//	m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

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
	m_refCnt = g_LoadedModelData[CUBE_MODEL_PATH]->m_pRootObject.use_count();
	m_size = m_vpAllObjs.size();

	m_tTime += elapsedTime;

#endif

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->Update(elapsedTime);
	}

	// Contact 데이터 생성
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
		g_Shaders[ShaderType::Shader_Image]->ChangeShader(pd3dCommandList);
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);
	}

	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
	{
		g_Shaders[ShaderType::Shader_Static]->ChangeShader(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());

		m_vObjectLayer[RenderLayer::Render_Static][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_Static][i]->Render(elapsedTime, pd3dCommandList);
	}

	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Skinned].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_Skinned][i])
		{
			// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->Animate(elapsedTime);
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->m_pAnimationController)
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->Render(elapsedTime, pd3dCommandList);
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
		//GetCursorPos(&ptCursorPos);
		GetCursorPos(&ptCursorPos);
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
		dx = (float)(ptCursorPos.x - CLIENT_WIDTH / 2) / 30.0f;
		dy = (float)(ptCursorPos.y - CLIENT_HEIGHT / 2) / 30.0f;
		//m_LastMousePos = ptCursorPos;
		//SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
	}

	DWORD dwDirection = 0;
	if (pKeybuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeybuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeybuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeybuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;

	if(dx != 0 || dy != 0)
	{
		if (dx != 0 || dy != 0)
		{
			//m_vpAllObjs[0]->Rotate(0,dx, 0);

			m_pCamera->Pitch(dy);
			m_pCamera->RotateY(dx);
		}
	}

	if (dwDirection != 0)
	{
		m_vpAllObjs[0]->SetRotate(XMFLOAT3(0, m_pCamera->GetYaw(), 0));
		m_vpAllObjs[0]->Move(dwDirection);

		// 이동시 애니메이션이 전환 되도록 (임시)
		m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	}
	else
	{
		m_vpAllObjs[0]->Move(dwDirection);
		m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 1);

	}

	if (pKeybuffer[VK_SPACE] & 0xF0) m_vpAllObjs[0]->Jump();


#if defined(_DEBUG)
	if (pKeybuffer['K'] & 0xF0)
	{
		// 오브젝트 생성

	}
#endif
}


std::shared_ptr<Object> Scene::CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
											const ObjectInitData& objInitData, const char* pstrFilePath, int nAnimationTracks, RenderLayer renderLayer)
{
	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> pObject;
	
	if (g_LoadedModelData.find(pstrFilePath) == g_LoadedModelData.end())
	{
		// 모델 로드
		pModelData = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, pstrFilePath);

		g_LoadedModelData.insert({ pstrFilePath, pModelData });
	}
	else	// 이미 로드한 모델인 경우
	{
		pModelData = g_LoadedModelData[pstrFilePath];
	}

	switch (objInitData.objectType)
	{
	case Object_Character:
	{
		std::shared_ptr<Character> pCharacter = std::make_shared<Character>(pd3dDevice, pd3dCommandList, objInitData, pModelData, nAnimationTracks);
		pObject = std::static_pointer_cast<Object>(pCharacter);
	}
		break;

	default:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objInitData, pModelData, nAnimationTracks);
	}
		break;
	}

	m_vpAllObjs.emplace_back(pObject);
	m_vObjectLayer[renderLayer].emplace_back(pObject);
	
	// 충돌체 타입에 따라 
	switch (objInitData.colliderType)
	{
	case Collider_Plane:
	{
		g_ppColliderPlanes.emplace_back(std::static_pointer_cast<ColliderPlane>(pObject->GetCollider()));
	}
	break;

	case Collider_Box:
	{
		g_ppColliderBoxs.emplace_back(std::static_pointer_cast<ColliderBox>(pObject->GetCollider()));
	}
	break;

	case Collider_Sphere:
	{
		g_ppColliderSpheres.emplace_back(std::static_pointer_cast<ColliderSphere>(pObject->GetCollider()));
	}
	break;

	default:
		break;
	}

	pObject->Initialize(pd3dDevice, pd3dCommandList, NULL);

	return pObject;
}

void Scene::GenerateContact()
{
	unsigned int nContactCnt = MAX_CONTACT_CNT * 8;

	m_CollisionData.Reset(nContactCnt);
	m_CollisionData.friction = 0;
	m_CollisionData.restitution = 0.1f;
	m_CollisionData.tolerance = 0.1f;

	// 플레이어 검사
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		if (m_CollisionData.ContactCnt() > nContactCnt) return;
		CollisionDetector::BoxAndHalfSpace(*g_ppColliderBoxs[0], *g_ppColliderPlanes[i], m_CollisionData);
	}
	for (int i = 1; i < g_ppColliderBoxs.size(); ++i)
	{
		if (m_CollisionData.ContactCnt() > nContactCnt) return;

		CollisionDetector::BoxAndBox(*g_ppColliderBoxs[0], *g_ppColliderBoxs[i], m_CollisionData);
	}


	m_CollisionData.friction = 0.9f;
	// 평면과의 검사
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		for (int j = 1; j < g_ppColliderBoxs.size(); ++j)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*g_ppColliderBoxs[j], *g_ppColliderPlanes[i], m_CollisionData);
		}
	}
	// 박스끼리 검사
	for (int i = 1; i < g_ppColliderBoxs.size(); ++i)
	{
		for (int j = i + 1; j < g_ppColliderBoxs.size(); ++j)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;

			CollisionDetector::BoxAndBox(*g_ppColliderBoxs[i], *g_ppColliderBoxs[j], m_CollisionData);
		}
	}
}

void Scene::ClearObjectLayer()
{
	bool bFind = false;
	for (int j = 0; j < g_ppColliderBoxs.size(); ++j)
	{
		if (bFind) break;
		if (g_ppColliderBoxs[j]->GetBody()->GetInvalid())
		{
			g_ppColliderBoxs.erase(g_ppColliderBoxs.begin() + j);
			bFind = true;
		}
	}
	for (int j = 0; j < g_ppColliderSpheres.size(); ++j)
	{
		if (g_ppColliderSpheres[j]->GetBody()->GetInvalid())
		{
			g_ppColliderSpheres.erase(g_ppColliderSpheres.begin() + j);
			bFind = true;
		}
	}
	for (int j = 0; j < g_ppColliderPlanes.size(); ++j)
	{
		if (bFind) break;
		if (g_ppColliderPlanes[j]->GetBody()->GetInvalid())
		{
			g_ppColliderPlanes.erase(g_ppColliderPlanes.begin() + j);
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
	for (int i = 0; i < RenderLayer::Render_Count; ++i)
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
