#include "../Header/Scene.h"

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
	objectData.xmf3Rotation = XMFLOAT3(0, 0, 0);
	objectData.xmf4Orientation = XMFLOAT4(0, 1, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(10,10, 10);
	objectData.nMass = 20;
	objectData.objectType = Object_Player;
	objectData.colliderType = Collider_Box;
	objectData.xmf3Extents = CHARACTER_MODEL_EXTENTS;

	// 캐릭터 테스트
	CreateObject(pd3dDevice, pd3dCommandList, objectData, CHARACTER_MODEL_PATH, 2, RenderLayer::Render_Skinned);
	//m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(1, 13);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackEnable(1, false);

	objectData.objectType = Object_Monster;
	objectData.xmf3Extents = ZOMBIE_MODEL_EXTENTS;
	objectData.xmf3Position = XMFLOAT3(-20, 0, 0);
	CreateObject(pd3dDevice, pd3dCommandList, objectData, ZOMBIE_MODEL_PATH, 1, RenderLayer::Render_Skinned);
	objectData.xmf3Position = XMFLOAT3(0, 0, 0);
	CreateObject(pd3dDevice, pd3dCommandList, objectData, ZOMBIE_MODEL_PATH, 1, RenderLayer::Render_Skinned);
	objectData.xmf3Position = XMFLOAT3(20, 0, 0);
	CreateObject(pd3dDevice, pd3dCommandList, objectData, ZOMBIE_MODEL_PATH, 1, RenderLayer::Render_Skinned);

	objectData.xmf3Position = XMFLOAT3(0, 0.2, 0);
	objectData.xmf3Rotation = XMFLOAT3(50, 0, 90);
	objectData.xmf3Scale = XMFLOAT3(0.2, 0.2, 0.2);
	objectData.objectType = Object_Weapon;			// 임시로 무기
	objectData.colliderType = Collider_Box;
	objectData.xmf3Extents = WEAPON_MODEL_EXTENTS;
	CreateObject(pd3dDevice, pd3dCommandList, objectData, WEAPON_MODEL_PATH, 1, RenderLayer::Render_Static);

	// 임시 바닥
	objectData.xmf3Extents = XMFLOAT3(0,0,0);
	objectData.objectType = Object_World;
	objectData.xmf3Position = XMFLOAT3(0, 0, 0);
	objectData.xmf3Rotation = XMFLOAT3(0, 0, 0);
	objectData.xmf4Orientation = XMFLOAT4(0, 1, 0, 1);
	objectData.xmf3Scale = XMFLOAT3(100, 0.1, 100);
	objectData.colliderType = Collider_Plane;
	CreateObject(pd3dDevice, pd3dCommandList, objectData, CUBE_MODEL_PATH, 1, RenderLayer::Render_Static);

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

	// 교차 검사
	Intersect();

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

	if (pKeybuffer[VK_RBUTTON] & 0xF0)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
		dx = (float)(ptCursorPos.x - CLIENT_WIDTH / 2) / 30.0f;
		dy = (float)(ptCursorPos.y - CLIENT_HEIGHT / 2) / 30.0f;
	}

	if (dx != 0 || dy != 0)
	{
		if (dx != 0 || dy != 0)
		{
			//m_vpAllObjs[0]->Rotate(0,dx, 0);

			m_pCamera->Pitch(dy);
			m_pCamera->RotateY(dx);
		}
	}

	/*for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		m_vpAllObjs[i]->ProcessInput(pKeybuffer);
	}*/
	

	// 임시
	if (m_vpAllObjs[0])
	{
		m_vpAllObjs[0]->ProcessInput(pKeybuffer);
		m_vpAllObjs[0]->SetRotate(XMFLOAT3(0, m_pCamera->GetYaw(), 0));
		
	}
	
#if defined(_DEBUG)

#endif
}

void Scene::KeyDownEvent(WPARAM wParam)
{
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		m_vpAllObjs[i]->KeyDownEvent(wParam);
	}
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
	case Object_Player:
	{
		std::shared_ptr<Player> pPlayer = std::make_shared<Player>(pd3dDevice, pd3dCommandList, objInitData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pPlayer);
	}
		break;

	case Object_Monster:
	{
		std::shared_ptr<Monster> pMonster = std::make_shared<Monster>(pd3dDevice, pd3dCommandList, objInitData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pMonster);
	}
		break;


	case Object_Weapon:
	{
		char tmp[64] = "mixamorig:RightHand";
		std::shared_ptr<Weapon> pWeapon = std::make_shared<Weapon>(pd3dDevice, pd3dCommandList, objInitData, tmp, m_vpAllObjs[0], pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pWeapon);
		std::static_pointer_cast<Player>(m_vpAllObjs[0])->SetWeapon(pWeapon);
	}
	break;	

	default:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objInitData, pModelData, nAnimationTracks, nullptr);
	}
		break;
	}

	m_vpAllObjs.emplace_back(pObject);
	m_vObjectLayer[renderLayer].emplace_back(pObject);
	m_vObjectLayer[renderLayer][0]->GetCollider();
//	pObject->Initialize(pd3dDevice, pd3dCommandList, NULL);

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
				m_vObjectLayer[i][j]->Destroy();
				m_vObjectLayer[i].erase(m_vObjectLayer[i].begin() + j);
			}
		}
	}
}

void Scene::Intersect()
{
	for (int i = 0; i < g_ppColliderBoxs.size(); ++i)
	{
		g_ppColliderBoxs[i]->SetIntersect(0);
	}

	for (int i = 0; i < g_ppColliderBoxs.size() - 1; ++i)
	{
		if (!g_ppColliderBoxs[i]->GetIsActive())
			continue;
		for (int k = i + 1; k < g_ppColliderBoxs.size(); ++k)
		{
			if (!g_ppColliderBoxs[k]->GetIsActive())
				continue;
			if (IntersectTests::BoxAndBox(*g_ppColliderBoxs[i], *g_ppColliderBoxs[k]))
			{
				g_ppColliderBoxs[i]->SetIntersect(1);
				g_ppColliderBoxs[k]->SetIntersect(1);
			}
		}
	}
}
