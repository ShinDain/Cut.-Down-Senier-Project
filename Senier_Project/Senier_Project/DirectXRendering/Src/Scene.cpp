#include "../Header/Scene.h"

//#define CHARACTER_MODEL_PATH "Model/unitychan.bin"
#define CHARACTER_MODEL_PATH "Model/BoxUnityChan.bin"
#define CHARACTER_MODEL_EXTENTS XMFLOAT3(0.3f, 0.8f, 0.2f)
#define CUBE_MODEL_PATH "Model/Cube.bin"
#define CUBE_MODEL_EXTENTS XMFLOAT3(0.5f, 0.5f, 0.5f)


static float pEtime = 0;

Scene::Scene()
{

}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 패스 버퍼 생성
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	ObjectInitData objectData;
	objectData.xmf3Position = XMFLOAT3(0, 0, 0);
	objectData.xmf4Orientation = XMFLOAT4(0, 0, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(10, 10, 10);
	objectData.nMass = 20;
	objectData.objectType = Object_Character;
	objectData.colliderType = Collider_Box;
	objectData.xmf3Extents = CHARACTER_MODEL_EXTENTS;

	// 캐릭터 테스트
	CreateObject(pd3dDevice, pd3dCommandList, objectData, CHARACTER_MODEL_PATH, 1, RenderLayer::Render_Skinned);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 1);



	// 임시 바닥
	objectData.xmf3Extents = XMFLOAT3(0,0,0);
	objectData.objectType = Object_World;
	objectData.xmf3Position = XMFLOAT3(0, 0, 0);
	objectData.xmf4Orientation = XMFLOAT4(0, 1, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(100, 0.1, 100);
	objectData.colliderType = Collider_Plane;
	CreateObject(pd3dDevice, pd3dCommandList, objectData, CUBE_MODEL_PATH, 1, RenderLayer::Render_Static);


	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->UpdateTransform();
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
	m_refCnt = g_LoadedModelData[CHARACTER_MODEL_PATH]->m_pRootObject.use_count();
	m_size = m_vpAllObjs.size();

	m_tTime += elapsedTime;

#endif


	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->Update(elapsedTime);
	}
	
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

	g_Shaders[ShaderType::Shader_Static]->ChangeShader(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
	{
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

	// 임시
	if (dwDirection != 0)
	{
		m_vpAllObjs[0]->SetRotate(XMFLOAT3(0, m_pCamera->GetYaw(), 0));
		m_vpAllObjs[0]->Move(dwDirection);
	}
	else
	{
		m_vpAllObjs[0]->Move(dwDirection);
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
	
	if (g_LoadedModelData.find(pstrFilePath) == g_LoadedModelData.end() && pstrFilePath != nullptr)
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

void Scene::ClearObjectLayer()
{
	/*for (int j = 0; j < g_ppColliderBoxs.size(); ++j)
	{
		if (g_ppColliderBoxs[j]->GetBody()->GetInvalid())
		{
			g_ppColliderBoxs.erase(g_ppColliderBoxs.begin() + j);
		}
	}
	for (int j = 0; j < g_ppColliderSpheres.size(); ++j)
	{
		if (g_ppColliderSpheres[j]->GetBody()->GetInvalid())
		{
			g_ppColliderSpheres.erase(g_ppColliderSpheres.begin() + j);
		}
	}
	for (int j = 0; j < g_ppColliderPlanes.size(); ++j)
	{
		if (g_ppColliderPlanes[j]->GetBody()->GetInvalid())
		{
			g_ppColliderPlanes.erase(g_ppColliderPlanes.begin() + j);
		}
	}*/

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
