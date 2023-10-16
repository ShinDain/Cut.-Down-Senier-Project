#include "../Header/Scene.h"

UINT Scene::m_nStageNum = 0;
std::vector<std::shared_ptr<Object>> Scene::m_vObjectLayer[(int)RenderLayer::Render_Count];
//CollisionData Scene::m_CollisionData;
//std::unique_ptr<CollisionResolver> Scene::m_pCollisionResolver;

/// ////////////
std::vector<std::shared_ptr<CSound>> Scene::m_vpSounds;
/////////////

Scene::Scene()
{
	m_CollisionData.Reset(MAX_CONTACT_CNT);
	m_pCollisionResolver = std::make_unique<CollisionResolver>(MAX_CONTACT_CNT * 8);
}

Scene::~Scene()
{
	for (int i = 0; i < m_vpSounds.size(); ++i)
		m_vpSounds[i].reset();
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	std::shared_ptr<DWriteText> pDWriteText, std::shared_ptr<DWriteText> pBigSizeText)
{
	// 패스 버퍼 생성
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);
	m_pShadowPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	// 그림자 맵 생성
	m_ShadowMap = std::make_unique<DepthMap>(pd3dDevice, 2048, 2048);
	BuildDescriptorHeap(pd3dDevice, pd3dCommandList);

	// UI 초기화
	InitUI(pd3dDevice, pd3dCommandList, pDWriteText, pBigSizeText);

	// 게임 시작
	GameStart();

	// 플레이어
	//CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 10, -20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	CHARACTER_MODEL_NAME, PLAYER_TRACK_CNT);
	//
	//// 카메라 초기화
	//m_pCamera = nullptr;
	//m_pCamera = std::make_unique<Third_Person_Camera>(g_pPlayer);
	//m_pCamera->Pitch(15);
	//
	//// 무기
	//CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(-0.59f, 0.135f, 0.063f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(50, 0, 90), XMFLOAT3(1, 1, 1), WEAPON_MODEL_NAME, 0);
	//// 바닥
	//CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, 0);
	//
	//// 월드 오브젝트 테스트
	//CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 20), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), CHAIR_LEATHER_MODEL_NAME, 0);

	return true;
}

bool Scene::InitUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	std::shared_ptr<DWriteText> pDWriteText, std::shared_ptr<DWriteText> pBigSizeText)
{
	// 각종 UI들 초기화  함수로 분리 예정
	m_pPlayerHP_Bar = std::make_unique<ImgObject>();
	m_pPlayerHP_Frame = std::make_unique<ImgObject>();
	m_pPlayer_Aim = std::make_unique<ImgObject>();
	m_pEnemyHP_Bar = std::make_unique<ImgObject>();
	m_pEnemyHP_Frame = std::make_unique<ImgObject>();
	m_pEnemyName_Back = std::make_unique<ImgObject>();
	m_pHP_Back = std::make_unique<ImgObject>();
	m_pScore_Back = std::make_unique<ImgObject>();

	m_pPlayerHP_Bar->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_line.dds", PLAYER_HP_BAR_WIDTH, PLAYER_HP_BAR_HEIGHT);
	m_pPlayerHP_Frame->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_frame.dds", PLAYER_HP_BAR_WIDTH, PLAYER_HP_BAR_HEIGHT);
	m_pPlayer_Aim->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Aim.dds", PLAYER_AIM_WIDTH, PLAYER_AIM_HEIGHT);
	m_pEnemyHP_Bar->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_line.dds", ENEMY_HP_BAR_WIDTH, ENEMY_HP_BAR_HEIGHT);
	m_pEnemyHP_Frame->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Hp_frame.dds", ENEMY_HP_BAR_WIDTH, ENEMY_HP_BAR_HEIGHT);
	m_pEnemyName_Back->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Name_Back.dds", ENEMY_NAME_WIDTH, ENEMY_NAME_HEIGHT);
	m_pHP_Back->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Score_Back.dds", HP_BACK_WIDTH, HP_BACK_HEIGHT);
	m_pScore_Back->Initialize(pd3dDevice, pd3dCommandList, CLIENT_WIDTH, CLIENT_HEIGHT, L"GUI/Score_Back.dds", SCORE_BACK_WIDTH, SCORE_BACK_HEIGHT);

	m_pPlayerHP_Bar->ChangePosition(80, 30);
	m_pPlayerHP_Frame->ChangePosition(80, 30);
	m_pPlayer_Aim->ChangePosition((CLIENT_WIDTH / 2) - (m_pPlayer_Aim->GetBitmapWidth() / 2), (CLIENT_HEIGHT / 2) - (m_pPlayer_Aim->GetBitmapHeight() / 2));
	m_pEnemyHP_Bar->ChangePosition((CLIENT_WIDTH / 2) - (m_pEnemyHP_Bar->GetBitmapWidth() / 2), 65);
	m_pEnemyHP_Frame->ChangePosition((CLIENT_WIDTH / 2) - (m_pEnemyHP_Bar->GetBitmapWidth() / 2), 65);
	m_pEnemyName_Back->ChangePosition((CLIENT_WIDTH / 2) - (m_pEnemyHP_Bar->GetBitmapWidth() / 2), 25);
	m_pHP_Back->ChangePosition(-3, 21);
	m_pScore_Back->ChangePosition(0, 63);

	// TextUI 초기화
	m_pTextUIs = pDWriteText;
	m_pTextUIs->AddTextUI(L"HP ", -CLIENT_WIDTH / 2 + 35, -CLIENT_HEIGHT / 2 + 40);
	m_pTextUIs->AddTextUI(L"Score ", -CLIENT_WIDTH / 2 + 65, -CLIENT_HEIGHT / 2 + 81);
	m_pTextUIs->AddTextUI(L"Monster Name", 0, -CLIENT_HEIGHT / 2 + 47);
	m_pTextUIs->AddTextUI(L"Loading...", CLIENT_WIDTH / 2 - 100, CLIENT_HEIGHT / 2 - 40);
	m_pTextUIs->AddTextUI(L"Press Any Key To Start", 0, CLIENT_HEIGHT / 3 - 80);
	m_pTextUIs->AddTextUI(L"Press 'ESC' To Exit", 0, CLIENT_HEIGHT / 3 - 40);
	m_pTextUIs->AddTextUI(L"Thank you for playing.", 0, 70);

	// 큰 크기의 텍스트
	m_pBigSizeTextUI = pBigSizeText;
	m_pBigSizeTextUI->AddTextUI(L"", 450, -250);
	m_pBigSizeTextUI->AddTextUI(L"", 0, -30);
	m_pBigSizeTextUI->AddTextUI(L"", 0, 0);
	m_pBigSizeTextUI->AddTextUI(L"", 0, 0);

	return true;
}

void Scene::BuildDescriptorHeap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DsvDescriptorHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 2;			// ShdowMap, Dissolve Texture
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SrvDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	m_ShadowMapHeapIndex = 0;

	auto srvCpuStart = m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	auto srvGpuStart = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	auto dsvCpuStart = m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	UINT cbvSrvUavDescriptorSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT dsvDescriptorSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	m_ShadowMap->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_ShadowMapHeapIndex, cbvSrvUavDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_ShadowMapHeapIndex, cbvSrvUavDescriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 0, dsvDescriptorSize));

	// Dissolve Texture
	hDescriptor.Offset(1, cbvSrvUavDescriptorSize);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	const wchar_t* pstrFilePath = L"Textures/Noise.dds";
	LoadTexture(pd3dDevice, pd3dCommandList, pstrFilePath);
	auto dissolveTexture = FindReplicatedTexture(pstrFilePath);
	srvDesc.Format = dissolveTexture->Resource->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = dissolveTexture->Resource->GetDesc().MipLevels;

	pd3dDevice->CreateShaderResourceView(dissolveTexture->Resource.Get(), &srvDesc, hDescriptor);
}

void Scene::OnResize(float aspectRatio, float newWidth, float newHeight)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);

	m_pCinematicCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.f);

}

void Scene::Update(float totalTime ,float elapsedTime)
{
	if(m_bInCinematic)
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);

	// m_bNextStage 변수가 참 일시,
	ChangeStage();
	// m_bGameOver 변수가 참 일시,
	GameOver();
	// m_bGameEnd 변수가 참 일시,
	GameEnd();

	if (g_pEnterObject)
	{
		if (g_pEnterObject->GetIntersect())
		{
			m_bNextStage = true;
			g_pEnterObject->DestroyRunTime();
			g_pEnterObject = nullptr;
		}
	}
		
	if (m_bPaused)
	{
		// UI 이미지 업데이트
		UpdateUI(elapsedTime);
		// 카메라
		UpdateSceneCamera(0.0f);
		// 패스버퍼 업데이트
		UpdatePassCB(totalTime, elapsedTime);
		return;
	}

	// 화면 전환 효과
	UpdateFadeInOut(elapsedTime);
	// 오브젝트
	UpdateObject(elapsedTime);
	// 카메라
	UpdateSceneCamera(elapsedTime);
	// 플레이어
	UpdatePlayerData(elapsedTime);
	// 패스버퍼 업데이트
	UpdateShadowPassCB(totalTime, elapsedTime);
	UpdatePassCB(totalTime, elapsedTime);
	// ImageObject 렌더를 위한  직교 투영행렬 업데이트
	m_xmf4x4ImgObjMat = m_pCamera->GetOrtho4x4f();

	// 충돌 검사
	GenerateContact();
	ProcessPhysics(elapsedTime);
	// UI 이미지 업데이트
	UpdateUI(elapsedTime);
	// Sound 업데이트
	UpdateSound();
	// 이벤트 객체 업데이트
	UpdateEvent(elapsedTime);
}

void Scene::UpdateObject(float elapsedTime)
{
	// 오브젝트 정리
	ClearObjectLayer();

	// 카메라 프러스텀과 교차를 검증하여 렌더링 여부 업데이트
	BoundingFrustum* camFus;
	
	if(m_bInCinematic)
		camFus = m_pCinematicCamera->m_pCameraFrustum.get();
	else
		camFus = m_pCamera->m_pCameraFrustum.get();

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if (!g_vpAllObjs[i]->GetIsAlive()) continue;

		if (g_vpAllObjs[i]->m_pAnimationController)
		{
			g_vpAllObjs[i]->SetVisible(false);
			g_vpAllObjs[i]->Animate(elapsedTime);
		}

		if (g_vpAllObjs[i]->GetColliderType() == Collider_Box)
		{
			BoundingSphere* pObjectBS = g_vpAllObjs[i]->GetCollider()->GetBoundingSphere().get();

			if (camFus->Intersects(*pObjectBS))
				g_vpAllObjs[i]->SetVisible(true);
			else
				g_vpAllObjs[i]->SetVisible(false);
		}

		if (g_vpAllObjs[i]) g_vpAllObjs[i]->Update(elapsedTime);
	}
}

void Scene::UpdateUI(float elapsedTime)
{
	Player* pPlayer = (Player*)g_pPlayer.get();

	// 플레이어 HP 
	float playerHPRate = g_pPlayer->GetHP() / g_pPlayer->GetMaxHP();
	float playerHPBarWidth = PLAYER_HP_BAR_WIDTH * playerHPRate;
	m_pPlayerHP_Bar->ChangeSize(playerHPBarWidth, PLAYER_HP_BAR_HEIGHT);

	// 몬스터 HP
	Object* pTargetObject = pPlayer->GetPlayerTargetObject();
	if (pTargetObject && pTargetObject->GetObjectType() == Object_Monster && !m_bInCinematic)
	{
		m_pEnemyHP_Bar->SetVisible(true);
		m_pEnemyHP_Frame->SetVisible(true);
		m_pEnemyName_Back->SetVisible(true);

		float targetHPRate = pTargetObject->GetHP() / pTargetObject->GetMaxHP();
		float targetHPBarWidth = ENEMY_HP_BAR_WIDTH * targetHPRate;
		m_pEnemyHP_Bar->ChangeSize(targetHPBarWidth, ENEMY_HP_BAR_HEIGHT);
	}
	else
	{
		m_pEnemyHP_Bar->SetVisible(false);
		m_pEnemyHP_Frame->SetVisible(false);
		m_pEnemyName_Back->SetVisible(false);
	}

	// Text UI 업데이트
	if (m_FadeInValue >= 1.0f && !m_bPaused && !m_bInCinematic)
	{
		m_pPlayerHP_Bar->SetVisible(true);
		m_pPlayerHP_Frame->SetVisible(true);
		m_pPlayer_Aim->SetVisible(true);

		m_pHP_Back->SetVisible(true);
		m_pScore_Back->SetVisible(true);

		m_pTextUIs->UpdateTextUI(L"HP ", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_HP), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_HP), Text_UI_Idx_HP);

		// 점수 Text 갱신
		wchar_t pstrScore[64] = L"Score : ";
		wcscat_s(pstrScore, std::to_wstring(pPlayer->GetScore()).c_str());
		int length = wcslen(pstrScore) - 9;
		m_pTextUIs->UpdateTextUI(pstrScore, -CLIENT_WIDTH / 2 + 65 + (length * 7), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Score), Text_UI_Idx_Score);

		// 몬스터 이름 출력
		wchar_t pstrName[64] = L"";
		if (pTargetObject && pTargetObject->GetObjectType() == Object_Monster)
		{
			wcscpy_s(pstrName, pTargetObject->GetOutName());
		}
		m_pTextUIs->UpdateTextUI(pstrName, m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Monster_Name), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Monster_Name), Text_UI_Idx_Monster_Name);
	}
	else
	{
		m_pPlayerHP_Bar->SetVisible(false);
		m_pPlayerHP_Frame->SetVisible(false);
		m_pPlayer_Aim->SetVisible(false);
		m_pEnemyHP_Bar->SetVisible(false);
		m_pEnemyHP_Frame->SetVisible(false);
		m_pEnemyName_Back->SetVisible(false);
		m_pHP_Back->SetVisible(false);
		m_pScore_Back->SetVisible(false);

		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_HP), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_HP), Text_UI_Idx_HP);
		m_pTextUIs->UpdateTextUI(L"", -CLIENT_WIDTH / 2 + 65, m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Score), Text_UI_Idx_Score);
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Monster_Name), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Monster_Name), Text_UI_Idx_Monster_Name);
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);
	}

	if (m_bPressAnyKey && m_FadeState == 2)
		m_pTextUIs->UpdateTextUI(L"아무 키를 눌러 시작", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);
	else
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);

	if (m_bThanks && m_FadeState == 2)
		m_pTextUIs->UpdateTextUI(L"Thank you for playing.", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Thanks), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Thanks), Text_UI_Idx_Thanks);
	else
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Thanks), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Thanks), Text_UI_Idx_Thanks);

	if(m_bTitle && m_FadeState == 2)
		m_pBigSizeTextUI->UpdateTextUI(L"CUT. DOWN.",
			m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Title),
			m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Title), 
			Big_Text_UI_Idx_Title);
	else
		m_pBigSizeTextUI->UpdateTextUI(L"",
			m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Title),
			m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Title),
			Big_Text_UI_Idx_Title);

	if (m_bPaused)
	{
		for (int i = 0; i < Text_UI_Idx_Count; ++i)
		{
			m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(i), m_pTextUIs->GetTextUIPosY(i), i);
		}
		for (int i = 0; i < Big_Text_UI_Idx_Count; ++i)
		{
			m_pBigSizeTextUI->UpdateTextUI(L"", m_pBigSizeTextUI->GetTextUIPosX(i), m_pBigSizeTextUI->GetTextUIPosY(i), i);
		}

		m_pTextUIs->UpdateTextUI(L"아무 키를 눌러 시작", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_AnyKey), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_AnyKey), Text_UI_Idx_AnyKey);
		m_pTextUIs->UpdateTextUI(L"'ESC'키를 눌러 끝내기", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_ESC), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_ESC), Text_UI_Idx_ESC);
		m_pBigSizeTextUI->UpdateTextUI(L"Paused", m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Paused), m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Paused), Big_Text_UI_Idx_Paused);
	}
	else
	{
		m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_ESC), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_ESC), Text_UI_Idx_ESC);
		m_pBigSizeTextUI->UpdateTextUI(L"", m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Paused), m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Paused), Big_Text_UI_Idx_Paused);
	}


	// Scene 이미지 UI 업데이트
	m_pPlayerHP_Bar->Update(elapsedTime);
	m_pPlayerHP_Frame->Update(elapsedTime);
	m_pEnemyHP_Bar->Update(elapsedTime);
	m_pEnemyHP_Frame->Update(elapsedTime);
	m_pPlayer_Aim->Update(elapsedTime);
	m_pEnemyName_Back->Update(elapsedTime);
	m_pHP_Back->Update(elapsedTime);
	m_pScore_Back->Update(elapsedTime);
}

void Scene::UpdateSound()
{
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		if(m_vpSounds[i])
			m_vpSounds[i]->Update();
	}
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		// 단발성 효과음 종료시
		if (m_vpSounds[i])
		{
			if (!m_vpSounds[i]->GetIsPlaying())
			{
				m_vpSounds[i].reset();
				m_vpSounds.erase(m_vpSounds.begin() + i);
			}
		}
	}
}

void Scene::UpdateEvent(float elapsedTime)
{
	if (m_pEventObjects.size() == 0)
	{
		if (g_vpEventObjs.size() > 0)
			g_vpEventObjs[0]->SetIsActive(true);
		else if (g_pEnterObject && g_vpEventObjs.size() == 0)
		{
			g_pEnterObject->SetIsActive(true);
			if(!m_bInCinematic)
				g_pEnterObject->Update(true);
		}
	}

	for (int i = 0; i < g_vpEventObjs.size(); ++i)
	{
		if (g_vpEventObjs[i])
		{
			g_vpEventObjs[i]->Update(elapsedTime);

			if (g_vpEventObjs[i]->GetIntersect() && m_FadeState == 2)
			{
				const char* pstrFilePath = g_vpEventObjs[i]->GetFilePath();
				LoadMapData(g_pd3dDevice, g_pd3dCommandList, pstrFilePath, true);

				GenerateContact();
				ProcessPhysics(elapsedTime);

				if (!strcmp(pstrFilePath, "Outside/Outside_Cine_2"))
				{
					OutsideCine1();
				}
				else if (!strcmp(pstrFilePath, "Outside/Outside_Cine_3"))
				{
					OutsideCine2();
				}
				else if (!strcmp(pstrFilePath, "Hospital/Hospital_Cine_1"))
				{
					HospitalCine1();
				}
				else if (!strcmp(pstrFilePath, "Hospital/Hospital_Cine_2"))
				{
					HospitalCine2();
				}
				else if (!strcmp(pstrFilePath, "Hospital/Hospital_Cine_3"))
				{
					HospitalCine3();
				}
				else if (!strcmp(pstrFilePath, "Hospital/Hospital_Cine_4"))
				{
					HospitalCine4();
				}
				else if (!strcmp(pstrFilePath, "Dungeon/Dungeon_Cine_2"))
				{
					DungeonCine1();
				}
				else if (!strcmp(pstrFilePath, "Dungeon/Dungeon_Cine_3"))
				{
					DungeonCine2();
				}
				else if (!strcmp(pstrFilePath, "Dungeon/Dungeon_Cine_4"))
				{
					DungeonCine3();
				}

				g_vpEventObjs[i]->DestroyRunTime();
			}
		}
	}
}

void Scene::UpdatePlayerData(float elapsedTime)
{
	if (g_pPlayer)
	{
		Third_Person_Camera* tmpCam = (Third_Person_Camera*)m_pCamera.get();
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->SetCameraPosition(m_pCamera->GetPosition3f());

		if (tmpCam->GetIsShoulderView())
		{
			tmpPlayer->SetIsShoulderView(true);
			tmpPlayer->GetBody()->SetCharacterPitch(tmpCam->GetShoulderCameraPitch());
			tmpPlayer->SetRotate(XMFLOAT3(tmpCam->GetShoulderCameraPitch(), m_pCamera->GetYaw(), 0));
			tmpPlayer->SetCameraRotation(XMFLOAT3(tmpCam->GetShoulderCameraPitch(), m_pCamera->GetYaw(), 0));
		}
		else
		{
			tmpPlayer->SetIsShoulderView(false);
			tmpPlayer->GetBody()->SetCharacterPitch(0);
			tmpPlayer->SetCameraRotation(XMFLOAT3(m_pCamera->GetPitch(), m_pCamera->GetYaw(), 0));
		}
	}
}

void Scene::UpdateFadeInOut(float elapsedTime)
{
	if (m_FadeState == 0)
	{
		m_FadeInValue -= elapsedTime / 2;
		if (m_FadeInValue <= 0.0f)
		{
			m_FadeState = 2;
			m_FadeInValue = 0.0f;
		}
	}
	else if (m_FadeState == 1)
	{
		m_FadeInValue += elapsedTime / 2;
		if (m_FadeInValue >= 1.0f)
		{
			m_pTextUIs->UpdateTextUI(L"", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Loading), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Loading), Text_UI_Idx_Loading);
			m_FadeState = 2;
			m_FadeInValue = 1.0f;
		}
	}

	if (m_bFadeTimer)
	{
		m_ElapsedFadeTimer += elapsedTime;
		if (m_ElapsedFadeTimer >= m_FadeTimer)
		{
			m_bFadeTimer = false;
			m_ElapsedFadeTimer = 0.0f;
			m_FadeState = 1;
		}
	}
}

void Scene::UpdateSceneCamera(float elapsedTime)
{
	m_pCamera->Update(elapsedTime);
	if (m_pCinematic)
	{
		m_pCinematic->Update(elapsedTime);
		if (m_pCinematic->GetCinematicEnd())
			m_bInCinematic = false;

		m_pCinematicCamera->Update(elapsedTime);
	}
}

void Scene::UpdatePassCB(float totalTime, float elapsedTime)
{
	// 패스 버퍼 : 뷰 * 투영 변환 행렬 업데이트
	PassConstant passConstant;

	if (m_bInCinematic)
	{
		XMMATRIX view = m_pCinematicCamera->GetView();
		XMMATRIX viewProj = XMMatrixMultiply(view, m_pCinematicCamera->GetProj());
		passConstant.EyePosW = m_pCinematicCamera->GetPosition3f();
		XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	}
	else
	{
		XMMATRIX view = m_pCamera->GetView();
		XMMATRIX viewProj = XMMatrixMultiply(view, m_pCamera->GetProj());
		passConstant.EyePosW = m_pCamera->GetPosition3f();
		XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	}

	XMMATRIX shadowTransform = XMLoadFloat4x4(&m_xmf4x4ShadowTransform);
	XMStoreFloat4x4(&passConstant.ShadowTransform, XMMatrixTranspose(shadowTransform));

	passConstant.RenderTargetSize = XMFLOAT2((float)CLIENT_WIDTH, (float)CLIENT_HEIGHT);
	passConstant.InvRenderTargetSize = XMFLOAT2(1.0f / CLIENT_WIDTH, 1.0f / CLIENT_HEIGHT);
	passConstant.NearZ = 1.0f;
	passConstant.FarZ = 100.0f;
	passConstant.TotalTime = totalTime;
	passConstant.DeltaTime = elapsedTime;
	passConstant.AmbientLight = { 0.7f, 0.7f, 0.7f, 1.0f };
	passConstant.Lights[0].Direction = m_BaseLightDirections[m_nStageNum];
	passConstant.Lights[0].Strength = { 0.2f, 0.2f, 0.3f };
	//passConstant.Lights[0].Strength = { 0.0f, 0.0f, 0.0f };
	passConstant.Lights[0].Position = { 0, 30.0f, 20 };

	passConstant.FadeInValue = m_FadeInValue;

	m_pPassCB->CopyData(0, passConstant);
}

void Scene::UpdateShadowPassCB(float totalTime, float elapsedTime)
{
	float sceneBoundRadius = 400;
	if(m_nStageNum == 0)
		sceneBoundRadius = 600;

	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&m_BaseLightDirections[m_nStageNum]);
	lightDir = XMVector3Normalize(lightDir);
	XMFLOAT3 xmf3PlayerPos;
	if (m_bInCinematic)
		xmf3PlayerPos = m_pCinematicCamera->GetPosition3f();
	else
		xmf3PlayerPos = g_pPlayer->GetPosition();
	XMVECTOR targetPos = XMLoadFloat3(&xmf3PlayerPos);
	//XMVECTOR targetPos = XMVectorSet(0, 0, 0, 1);
	XMVECTOR lightPos = targetPos + -2.0f * sceneBoundRadius * lightDir;
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - sceneBoundRadius;
	float b = sphereCenterLS.y - sceneBoundRadius;
	float lightNear = sphereCenterLS.z - sceneBoundRadius;
	float r = sphereCenterLS.x + sceneBoundRadius;
	float t = sphereCenterLS.y + sceneBoundRadius;
	float lightFar = sphereCenterLS.z + sceneBoundRadius;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, lightNear, lightFar);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = lightView * lightProj * T;
	XMStoreFloat4x4(&m_xmf4x4ShadowTransform, shadowTransform);

	XMVECTOR tmpPosition = XMVectorSet(0, 0, 0, 1);
	tmpPosition = XMVector3TransformCoord(tmpPosition, shadowTransform);

	float shadowMapWidth = m_ShadowMap->Width();
	float shadowMapHeight = m_ShadowMap->Height();

	// 그림자 패스 버퍼 
	PassConstant passConstant;

	XMMATRIX view = lightView;
	XMMATRIX viewProj = XMMatrixMultiply(view, lightProj);

	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&passConstant.ShadowTransform, XMMatrixTranspose(shadowTransform));

	XMStoreFloat3(&passConstant.EyePosW, lightPos);
	passConstant.RenderTargetSize = XMFLOAT2((float)shadowMapWidth, (float)shadowMapHeight);
	passConstant.InvRenderTargetSize = XMFLOAT2(1.0f / shadowMapWidth, 1.0f / shadowMapHeight);
	passConstant.NearZ = lightNear;
	passConstant.FarZ = lightFar;
	passConstant.TotalTime = totalTime;
	passConstant.DeltaTime = elapsedTime;

	m_pShadowPassCB->CopyData(0, passConstant);
}

void Scene::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_FadeInValue <= 0)
		return;

	if (!m_bPaused)
	{
		ChangeShader(ShaderType::Shader_Skinned, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Skinned].size(); ++i)
		{
			if (m_vObjectLayer[RenderLayer::Render_Skinned][i])
			{
				if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->GetIsAlive())
					continue;
				// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->Animate(0.0f);
				if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->m_pAnimationController)
					m_vObjectLayer[RenderLayer::Render_Skinned][i]->UpdateTransform(NULL);
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->Render(elapsedTime, pd3dCommandList);
			}
		}

		ChangeShader(ShaderType::Shader_Static, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_Static][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_Static][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Static][i]->Render(elapsedTime, pd3dCommandList);
		}

		ChangeShader(ShaderType::Shader_TextureMesh, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_TextureMesh].size(); ++i)
		{
			if (m_nStageNum == 0)
			{
				if (i == 1)
					continue;
				else if (i == 2)
					continue;
			}

			if (!m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->Render(elapsedTime, pd3dCommandList);
		}
		// 첫번째 스테이지 팬스만 특별 취급 => 안좋음
		if (m_nStageNum == 0)
		{
			if (m_vObjectLayer[RenderLayer::Render_TextureMesh][1]->GetIsAlive())
			{
				m_vObjectLayer[RenderLayer::Render_TextureMesh][1]->UpdateTransform(NULL);
				m_vObjectLayer[RenderLayer::Render_TextureMesh][1]->Render(elapsedTime, pd3dCommandList);
			}
			if (m_vObjectLayer[RenderLayer::Render_TextureMesh][2]->GetIsAlive())
			{
				m_vObjectLayer[RenderLayer::Render_TextureMesh][2]->UpdateTransform(NULL);
				m_vObjectLayer[RenderLayer::Render_TextureMesh][2]->Render(elapsedTime, pd3dCommandList);
			}
		}

		ChangeShader(ShaderType::Shader_BackFace, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Backface].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_Backface][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_Backface][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Backface][i]->Render(elapsedTime, pd3dCommandList);
		}

		ChangeShader(ShaderType::Shader_CuttedStatic, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedStatic].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->Render(elapsedTime, pd3dCommandList);
		}

		ChangeShader(ShaderType::Shader_CuttedTextureMesh, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedTexture].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->Render(elapsedTime, pd3dCommandList);
		}

		ChangeShader(ShaderType::Shader_CuttedSkinned, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedSkinned].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->GetIsAlive())
				continue;
			// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
			m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Animate(0.0f);
			if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->m_pAnimationController)
				m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Render(elapsedTime, pd3dCommandList);
		}

		ChangeShader(ShaderType::Shader_TextureMesh, pd3dCommandList);
		for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Effect].size(); ++i)
		{
			if (!m_vObjectLayer[RenderLayer::Render_Effect][i]->GetIsAlive())
				continue;

			m_vObjectLayer[RenderLayer::Render_Effect][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Effect][i]->Render(elapsedTime, pd3dCommandList);
		}
	}
	// img 오브젝트
	{
		g_Shaders[ShaderType::Shader_Image]->ChangeShader(pd3dCommandList);
	
		//// Scene 그림자맵 테스트 시 주석 해제
		//ID3D12DescriptorHeap* descriptorHeap[] = { m_SrvDescriptorHeap.Get() };
		//pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);
		//
		//D3D12_GPU_DESCRIPTOR_HANDLE texHandle = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		//pd3dCommandList->SetGraphicsRootDescriptorTable(1, texHandle);

		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);

		m_pHP_Back->Render(elapsedTime, pd3dCommandList);
		m_pScore_Back->Render(elapsedTime, pd3dCommandList);

		m_pPlayerHP_Frame->Render(elapsedTime, pd3dCommandList);
		m_pPlayerHP_Bar->Render(elapsedTime, pd3dCommandList);
		m_pPlayer_Aim->Render(elapsedTime, pd3dCommandList);
		m_pEnemyName_Back->Render(elapsedTime, pd3dCommandList);

		m_pEnemyHP_Bar->Render(elapsedTime, pd3dCommandList);
		m_pEnemyHP_Frame->Render(elapsedTime, pd3dCommandList);
	}
}

void Scene::RenderSceneToShadowMap(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_FadeInValue <= 0)
		return;

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_VIEWPORT viewPort = m_ShadowMap->Viewport();
	D3D12_RECT scissorRect = m_ShadowMap->ScissorRect();
	pd3dCommandList->RSSetViewports(1, &viewPort);
	pd3dCommandList->RSSetScissorRects(1, &scissorRect);

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	pd3dCommandList->ResourceBarrier(1, &transition);

	pd3dCommandList->ClearDepthStencilView(m_ShadowMap->Dsv(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);


	g_Shaders[ShaderType::Shader_Skinned]->ChangeShader(pd3dCommandList);

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv = m_ShadowMap->Dsv();
	pd3dCommandList->OMSetRenderTargets(0, nullptr, false, &dsv);
	pd3dCommandList->SetGraphicsRootConstantBufferView(m_nPassCBParameterIdx, m_pShadowPassCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Skinned].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_Skinned][i])
		{
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->GetIsAlive())
				continue;
			// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
			//m_vObjectLayer[RenderLayer::Render_Skinned][i]->Animate(0.0f);
			if (!m_vObjectLayer[RenderLayer::Render_Skinned][i]->m_pAnimationController)
				m_vObjectLayer[RenderLayer::Render_Skinned][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Skinned][i]->DepthRender(0.0f, pd3dCommandList);
		}
	}

	// 셰이더 변경 여기
	g_Shaders[ShaderType::Shader_DepthMap]->ChangeShader(pd3dCommandList);
	//g_Shaders[ShaderType::Shader_Static]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Static].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_Static][i]->GetShadowed())
		{
			m_vObjectLayer[RenderLayer::Render_Static][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_Static][i]->DepthRender(0.0f, pd3dCommandList);
		}
	}
	//g_Shaders[ShaderType::Shader_TextureMesh]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_TextureMesh].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->GetShadowed())
		{
			m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->UpdateTransform(NULL);
			m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->DepthRender(0.0f, pd3dCommandList);
		}
	}

	// 잘린 면의 배제 없이 통째로 깊이가 그려지는 중
	//g_Shaders[ShaderType::Shader_CuttedStatic]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedStatic].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedStatic][i]->DepthRender(0.0f, pd3dCommandList);
	}
	//g_Shaders[ShaderType::Shader_CuttedTextureMesh]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedTexture].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->GetIsAlive())
			continue;

		m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedTexture][i]->DepthRender(0.0f, pd3dCommandList);
	}
	g_Shaders[ShaderType::Shader_CuttedSkinned]->ChangeShader(pd3dCommandList);
	//g_Shaders[ShaderType::Shader_Skinned]->ChangeShader(pd3dCommandList);
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_CuttedSkinned].size(); ++i)
	{
		if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->GetIsAlive())
			continue;
		// Render 함수 내에서 Bone 행렬이 셰이더로 전달되기 때문에 Render 직전에 애니메이션을 진행해준다.
		//m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->Animate(0.0f);
		if (!m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->m_pAnimationController)
			m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_CuttedSkinned][i]->DepthRender(0.0f, pd3dCommandList);
	}

	transition = CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	pd3dCommandList->ResourceBarrier(1, &transition);
}

void Scene::ChangeShader(ShaderType nShaderType, ID3D12GraphicsCommandList* pd3dCommandList)
{
	g_Shaders[nShaderType]->ChangeShader(pd3dCommandList);

	pd3dCommandList->SetGraphicsRootConstantBufferView(m_nPassCBParameterIdx, m_pPassCB->Resource()->GetGPUVirtualAddress());

	if (nShaderType == ShaderType::Shader_WireFrame)
		return;

	// Scene 그림자맵 
	ID3D12DescriptorHeap* descriptorHeap[] = { m_SrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE texHandle = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_nDescTableParameterIdx, texHandle);
}

void Scene::ProcessInput(UCHAR* pKeybuffer)
{
	if (m_FadeState != 2)
		return;
	// 시네마틱 재생중
	if (m_bInCinematic)
	{
		g_pPlayer->Move(0);
		return;
	}
		
	if (m_pCamera)
	{
		Third_Person_Camera* tmpCam = (Third_Person_Camera*)m_pCamera.get();
		if (pKeybuffer[VK_RBUTTON] & 0xF0)
		{
			tmpCam->SetIsShoulderView(true);
		}
		else
		{
			tmpCam->SetIsShoulderView(false);
		}
		float dx, dy;
		dx = dy = 0;

		// 카메라 이동
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
		dx = (float)(ptCursorPos.x - CLIENT_WIDTH / 2) / 25.0f;
		dy = (float)(ptCursorPos.y - CLIENT_HEIGHT / 2) / 25.0f;

		if (dx != 0 || dy != 0)
		{
			if (dx != 0 || dy != 0)
			{
				m_pCamera->Pitch(dy);
				m_pCamera->RotateY(dx);
			}
		}
	}

	if (g_pPlayer)
	{
		g_pPlayer->ProcessInput(pKeybuffer);
	}
	
#if defined(_DEBUG)

#endif
}

void Scene::AnykeyProcess()
{
	// 키입력 수신
	if (m_bPressAnyKey && m_FadeState == 2)
	{
		// 게임 시작시
		if (m_bGameStart)
		{
			m_pCinematic->Play();
			m_bPressAnyKey = false;
			m_bGameStart = false;
			m_bTitle = false;

			m_pBigSizeTextUI->UpdateTextUI(L"",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Title),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Title),
				Big_Text_UI_Idx_Title);
		}

		// 게임 오버시
		if (m_bGameOver)
		{
			m_bGameOver = false;
			m_bPressAnyKey = false;
			Restart();
			m_pBigSizeTextUI->UpdateTextUI(L"",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Over),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Over),
				Big_Text_UI_Idx_Over);
		}
		// 게임 종료시
		if (m_bGameEnd)
		{
			m_bGameEnd = false;
			m_bThanks = false;
			m_bPressAnyKey = false;
			GameStart();
			m_pBigSizeTextUI->UpdateTextUI(L"",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_End),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_End),
				Big_Text_UI_Idx_End);
		}
		return;
	}
}

void Scene::KeyDownEvent(WPARAM wParam)
{
#if defined(_DEBUG)
	switch (wParam)
	{
	case 'O':
		m_bGameEnd = true;
		break;
	case 'I':		// 다음 스테이지로 가는 테스트용 
		m_bNextStage = true;
		break;
	}

	// 애니메이션 테스트 용도
	for (int i = 0; i < g_vpCharacters.size(); ++i)
	{
		g_vpCharacters[i]->KeyDownEvent(wParam);
	}

#endif

	switch (wParam)
	{
	case VK_F12:
	{
		for (int i = 0; i < m_pEventObjects.size(); ++i)
		{
			m_pEventObjects[i]->Cutting(XMFLOAT3(1, 0, 1));
		}
	}
	break;
	case 'P':
		m_bPaused = !m_bPaused;
		break;
	case VK_ESCAPE:
		if (m_bPaused)
			PostQuitMessage(0);
		m_bPaused = !m_bPaused;
		break;
	default:
		m_bPaused = false;
		break;
	}

	AnykeyProcess();

	// 시네마틱 재생중
	if (m_bInCinematic)
	{
		return;
	}
	
	if(g_pPlayer) g_pPlayer->KeyDownEvent(wParam);
}

void Scene::KeyUpEvent(WPARAM wParam)
{
	// 시네마틱 재생중
	if (m_bInCinematic)
		return;

	if (g_pPlayer) g_pPlayer->KeyUpEvent(wParam);
}

void Scene::LeftButtonDownEvent()
{
	m_bPaused = false;

	AnykeyProcess();

	// 시네마틱 재생중
	if (m_bInCinematic)
	{
		return;
	}

	if (g_pPlayer)
	{
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->LeftButtonDownEvent();
	}
}

void Scene::RightButtonDownEvent()
{
	m_bPaused = false;

	AnykeyProcess();

	// 시네마틱 재생중
	if (m_bInCinematic)
	{
		return;
	}

	if (g_pPlayer) 
	{
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->RightButtonDownEvent();
	}
}

void Scene::LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName, bool bEvent)
{
	FILE* pInFile = NULL;

	UINT nEventCnt = 0;

	char pstrFilePath[64] = { '\0' };
	strcpy_s(pstrFilePath, 64, "Map/");
	strcat_s(pstrFilePath, pstrFileName);
	strcat_s(pstrFilePath, ".bin");
	::fopen_s(&pInFile, pstrFilePath, "rb");
	::rewind(pInFile);

	char pstrToken[64] = { '\0' };

	const char* pstrObjectName = "\0";
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmf3Rotation;
	XMFLOAT3 xmf3Scale;
	XMFLOAT4 xmf4Orientation;

	UINT nReads = 0;
	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (g_ppColliderBoxs.size() > 45)
		{
			float a = 100;
		}

		if(!strcmp(pstrToken, "<Hierarchy>:"))
		{
		} 
		else if (!strcmp(pstrToken, "<Frame>:"))
		{
			char tmpstr[64] = { '\0' };
			nReads = ReadStringFromFile(pInFile, tmpstr);

			if (tmpstr[nReads - 1] == ')')
			{
				for (int i = 1; i < 10; ++i)
				{
					if (tmpstr[nReads - i] == '(')
					{
						tmpstr[nReads - i] = '\0';
						tmpstr[nReads - i - 1] = '\0';
						break;
					}
					else
						tmpstr[nReads - i] = '\0';
				}
			}

			pstrObjectName = tmpstr;

			for (int i = 0; i < g_DefaultObjectNames.size(); ++i)
			{
				if (pstrObjectName == g_DefaultObjectNames[i])
					pstrObjectName = g_DefaultObjectNames[i].c_str();
			}
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			nReads = (UINT)fread(&xmf3Position, sizeof(float), 3, pInFile);
			xmf3Position.x *= 10;
			xmf3Position.y *= 10;
			xmf3Position.z *= 10;

			nReads = (UINT)fread(&xmf3Rotation, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&xmf4Orientation, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			std::shared_ptr<Object> pObject;

			if(g_DefaultObjectData[pstrObjectName].objectType == Object_Monster)
				pObject = CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, XMFLOAT3(1,1,1), pstrObjectName, MONSTER_TRACK_CNT);
			else if (g_DefaultObjectData[pstrObjectName].objectType == Object_Event)
			{
				pObject = CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, xmf3Scale, pstrObjectName, 0);
				
				if(strcmp(pstrObjectName, ENTER_BOX_MODEL_NAME))
				{
					char pstrEventFilePath[64];
					strcpy_s(pstrEventFilePath, pstrFileName);
					strcat_s(pstrEventFilePath, "_Cine_");
					nEventCnt += 1;
					char numchar = nEventCnt + '0';
					int length = strlen(pstrEventFilePath);
					pstrEventFilePath[length] = numchar;
					pstrEventFilePath[length + 1] = '\0';

					CEvent* pEvent = (CEvent*)pObject.get();
					pEvent->SetFilePath(pstrEventFilePath);
				}
			}
			else
				pObject = CreateObject(pd3dDevice, pd3dCommandList, xmf3Position, xmf4Orientation, xmf3Rotation, XMFLOAT3(1, 1, 1), pstrObjectName, 0);

			if (bEvent)
				m_pEventObjects.emplace_back(pObject);
		}
		else if (!strcmp(pstrToken, "</Hierarchy>"))
		{
			break;
		}
		else
		{
			continue;
		}
	}

}


std::shared_ptr<Object> Scene::CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale, const char* pstrFileName, int nAnimationTracks)
{
	ObjectInitData objectData;
	objectData.xmf3Position = xmf3Position;
	objectData.xmf3Rotation = xmf3Rotation;
	objectData.xmf4Orientation = xmf4Orientation;
	if (pstrFileName == nullptr)
	{
		objectData.nMass = 9999;
		objectData.objectType = Object_World;
		objectData.colliderType = Collider_Plane;
		objectData.xmf3Extents.x = nAnimationTracks;
	}
	else
	{
		objectData.xmf3Scale = g_DefaultObjectData[pstrFileName].xmf3OffsetScale;
		objectData.xmf3Scale.x *= xmf3Scale.x;
		objectData.xmf3Scale.y *= xmf3Scale.y;
		objectData.xmf3Scale.z *= xmf3Scale.z;
		objectData.nMass = g_DefaultObjectData[pstrFileName].nMass;
		objectData.objectType = g_DefaultObjectData[pstrFileName].objectType;
		objectData.colliderType = g_DefaultObjectData[pstrFileName].colliderType;
		objectData.soundType = g_DefaultObjectData[pstrFileName].soundType;
		objectData.xmf3Extents = g_DefaultObjectData[pstrFileName].xmf3Extents;
		objectData.xmf3MeshOffsetPosition = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetPosition;
		objectData.xmf3MeshOffsetRotation = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetRotation;
		objectData.bShadow = g_DefaultObjectData[pstrFileName].bShadowed;
	}

	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> pObject;
	std::string strFileName;

	if (pstrFileName != nullptr)
	{
		strFileName = pstrFileName;

		if (g_LoadedModelData.find(pstrFileName) == g_LoadedModelData.end())
		{
			// 로드되지 않은 모델인 경우
			assert(false);
			//return nullptr;
		}
		else	// 이미 로드한 모델인 경우
		{
			pModelData = g_LoadedModelData[pstrFileName];
		}
	}
	else if (pstrFileName == nullptr)
	{
		pModelData = nullptr;
		strFileName = "";
	}
	
	switch (objectData.objectType)
	{
	case Object_Player:
	{
		if (m_nStageNum == 1)
		{
			objectData.xmf3Scale.x *= 0.8f;
			objectData.xmf3Scale.y *= 0.8f;
			objectData.xmf3Scale.z *= 0.8f;
		}

		std::shared_ptr<Player> pPlayer = std::make_shared<Player>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		strcpy_s(pPlayer->m_pstrFileName, strFileName.c_str());
		wcscpy_s(pPlayer->m_pstrOutName, L"Player");
		pObject = std::static_pointer_cast<Object>(pPlayer);

		g_vpAllObjs.emplace_back(pObject);
		g_vpCharacters.emplace_back(pPlayer);
		g_pPlayer = pPlayer;
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;

	case Object_Monster:
	{
		if (m_nStageNum == 1)
		{
			objectData.xmf3Scale.x *= 0.8f;
			objectData.xmf3Scale.y *= 0.8f;
			objectData.xmf3Scale.z *= 0.8f;
		}

		if (!strcmp(pstrFileName, ZOMBIE_MODEL_NAME))
		{
			std::shared_ptr<Zombie> pMonster = std::make_shared<Zombie>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"좀비");
		}
		else if (!strcmp(pstrFileName, HIGHZOMBIE_MODEL_NAME))
		{
			std::shared_ptr<HighZombie> pMonster = std::make_shared<HighZombie>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"강화된 좀비");
		}
		else if (!strcmp(pstrFileName, SCAVENGER_MODEL_NAME))
		{
			std::shared_ptr<Scavenger> pMonster = std::make_shared<Scavenger>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"사마귀");
		}
		else if (!strcmp(pstrFileName, GHOUL_MODEL_NAME))
		{
			std::shared_ptr<Ghoul> pMonster = std::make_shared<Ghoul>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"구울");
		}
		else if (!strcmp(pstrFileName, CYBER_TWINS_MODEL_NAME))
		{
			std::shared_ptr<CyberTwins> pMonster = std::make_shared<CyberTwins>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"트윈스");
		}
		else if (!strcmp(pstrFileName, NECROMANCER_MODEL_NAME))
		{
			std::shared_ptr<Necromancer> pMonster = std::make_shared<Necromancer>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
			wcscpy_s(pObject->m_pstrOutName, L"강령술사");
		}
		else
		{
			// 스킨 메시 애니메이션 테스트용
			std::shared_ptr<AnimTestCharacter> pMonster = std::make_shared<AnimTestCharacter>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
			pObject = std::static_pointer_cast<Object>(pMonster);
		}

		g_vpAllObjs.emplace_back(pObject);
		g_vpMovableObjs.emplace_back(pObject);
		g_vpCharacters.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;

	case Object_Weapon:
	{
		char tmp[64] = "mixamorig:RightHand";
		std::shared_ptr<Weapon> pWeapon = std::make_shared<Weapon>(pd3dDevice, pd3dCommandList, objectData, tmp, g_pPlayer, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pWeapon);
		Player* tmpPlayer = (Player*)(g_pPlayer.get());
		tmpPlayer->SetWeapon(pWeapon);

		g_vpAllObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
	break;	

	case Object_Movable:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		g_vpAllObjs.emplace_back(pObject);
		g_vpMovableObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	   
		objectData.xmf3Scale.x *= 1.03f;
		objectData.xmf3Scale.y *= 1.03f;
		objectData.xmf3Scale.z *= 1.03f;
		objectData.bShadow = false;
		std::shared_ptr<BackfaceObject> pBackfaceObject = std::make_shared<BackfaceObject>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pBackfaceObject->SetOriginalObject(pObject);
		g_vpAllObjs.emplace_back(pBackfaceObject);
		m_vObjectLayer[Render_Backface].emplace_back(pBackfaceObject);
	}
	break;

	case Object_Item:
	{
		std::shared_ptr<Item> pItem = std::make_shared<Item>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pItem);

		g_vpAllObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
	break;

	case Object_PlayerProjectile:
	{
		std::shared_ptr<Projectile> pProjectile = std::make_shared<Projectile>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pProjectile->SetProjectileProperty(ProjectileProperty::Projectile_Player);
		pObject = std::static_pointer_cast<Object>(pProjectile);
		
		g_vpAllObjs.emplace_back(pObject); 
		if (objectData.colliderType != Collider_None)
		{
			g_vpMovableObjs.emplace_back(pObject);
			g_vpWorldObjs.emplace_back(pObject);
		}
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);

		objectData.xmf3Scale.x *= 1.03f;
		objectData.xmf3Scale.y *= 1.03f;
		objectData.xmf3Scale.z *= 1.03f;
		objectData.bShadow = false;
		std::shared_ptr<BackfaceObject> pBackfaceObject = std::make_shared<BackfaceObject>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pBackfaceObject->SetOriginalObject(pObject);
		g_vpAllObjs.emplace_back(pBackfaceObject);
		m_vObjectLayer[Render_Backface].emplace_back(pBackfaceObject);
	}
	break;

	case Object_EnemyProjectile:
	{
		std::shared_ptr<Projectile> pProjectile = std::make_shared<Projectile>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pProjectile->SetProjectileProperty(ProjectileProperty::Projectile_Enemy);
		pObject = std::static_pointer_cast<Object>(pProjectile);

		g_vpAllObjs.emplace_back(pObject);
		if (objectData.colliderType != Collider_None)
		{
			g_vpMovableObjs.emplace_back(pObject);
			g_vpWorldObjs.emplace_back(pObject);
		}
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);

		objectData.xmf3Scale.x *= 1.03f;
		objectData.xmf3Scale.y *= 1.03f;
		objectData.xmf3Scale.z *= 1.03f;
		objectData.bShadow = false;
		std::shared_ptr<BackfaceObject> pBackfaceObject = std::make_shared<BackfaceObject>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pBackfaceObject->SetOriginalObject(pObject);
		g_vpAllObjs.emplace_back(pBackfaceObject);
		m_vObjectLayer[Render_Backface].emplace_back(pBackfaceObject);
	}
	break;

	case Object_Effect:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);

		g_vpAllObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;
	case Object_Event:
	{
		std::shared_ptr<CEvent> pEvent = std::make_shared<CEvent>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		
		if (!strcmp(strFileName.c_str(), "Enter_Object"))
		{
			g_pEnterObject = pEvent;
			pEvent->SetVisible(true);
		}
		else
		{
			g_vpEventObjs.emplace_back(pEvent);
		}
		pObject = std::static_pointer_cast<Object>(pEvent);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;
	default:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);

		g_vpAllObjs.emplace_back(pObject);
		g_vpWorldObjs.emplace_back(pObject);
		m_vObjectLayer[g_DefaultObjectData[strFileName].renderLayer].emplace_back(pObject);
	}
		break;
	}

	if (pObject && strFileName.c_str() != nullptr) strcpy_s(pObject->m_pstrFileName, strFileName.c_str());

	return pObject;
}

std::shared_ptr<Object> Scene::CreateCuttedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	Object* pObject, float direction, XMFLOAT3 xmf3PlaneNormal, bool bIsCutted)
{
	//float cuttingPower = rand() % 30 + 40; 
	float cuttingPower = 40; 
	
	XMVECTOR noiseDir = XMLoadFloat3(&xmf3PlaneNormal);
	noiseDir *= -1 * (float)(rand() % 30) / 100;
	
	float directions[3] = {0,0,0};
	XMFLOAT3 xmf3PlaneNormals[3] = { XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT3(0,0,0) };
	UINT planeCnt = 0;

	XMVECTOR angularVel = XMLoadFloat3(&pObject->GetLookVector());
	angularVel *= direction;


	if (bIsCutted)
	{
		CuttedObject* pCuttedObj = (CuttedObject*)(pObject);

		UINT lastPlaneCnt = pCuttedObj->GetPlaneCnt();
		if (lastPlaneCnt >= 3)
			return nullptr;
		for (int i = 0; i < lastPlaneCnt; ++i)
		{
			directions[i] = pCuttedObj->GetPlaneDirection(i);
			xmf3PlaneNormals[i] = pCuttedObj->GetPlaneNormal(i);
		}
		directions[lastPlaneCnt] = direction * -1;
		xmf3PlaneNormals[lastPlaneCnt] = xmf3PlaneNormal;
		planeCnt = lastPlaneCnt + 1;
		pCuttedObj->AddPlane(direction, xmf3PlaneNormal);


		XMVECTOR cuttingVel = XMLoadFloat3(&xmf3PlaneNormal);
		cuttingVel += noiseDir;
		cuttingVel *= cuttingPower * direction * -1;
		XMFLOAT3 xmf3CuttingVel;
		XMStoreFloat3(&xmf3CuttingVel, cuttingVel);
		pCuttedObj->GetBody()->AddVelocity(xmf3CuttingVel);

		XMVECTOR inverseAngularVel = angularVel;
		inverseAngularVel *= -1;
		XMFLOAT3 xmf3InverseAngularVel;
		XMStoreFloat3(&xmf3InverseAngularVel, inverseAngularVel);
		pCuttedObj->GetBody()->AddAngleVelocity(xmf3InverseAngularVel);
	}
	else
	{
		directions[0] = direction;
		xmf3PlaneNormals[0] = xmf3PlaneNormal;
		planeCnt += 1;
	}

	std::string pstrFileName;
	pstrFileName = pObject->m_pstrFileName;

	ObjectInitData objectData;

	objectData.xmf3Position = pObject->GetPosition();
	objectData.xmf3Rotation = pObject->GetRotation();
	objectData.xmf4Orientation = pObject->GetOrientation();
	objectData.xmf3Scale = pObject->GetScale();
	//objectData.nMass = g_DefaultObjectData[pstrFileName].nMass;
	objectData.nMass = 10;
	//objectData.nMass = g_DefaultObjectData[pstrFileName].nMass * 0.3f;
	objectData.objectType = g_DefaultObjectData[pstrFileName].objectType;
	objectData.colliderType = g_DefaultObjectData[pstrFileName].colliderType;
	objectData.xmf3Extents = g_DefaultObjectData[pstrFileName].xmf3Extents;
	objectData.xmf3MeshOffsetPosition = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetPosition;
	objectData.xmf3MeshOffsetRotation = g_DefaultObjectData[pstrFileName].xmf3MeshOffsetRotation;

	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> tmpObject;

	pModelData = g_LoadedModelData[pstrFileName];

	switch (objectData.objectType)
	{
	//case Object_Player:
	//{
	//	std::shared_ptr<Object> tmpObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
	//
	//	g_vpCuttedSkinnedObjs.emplace_back(tmpObject);
	//	m_vObjectLayer[RenderLayer::Render_CuttedSkinned].emplace_back(tmpObject);
	//}
	//break;

	case Object_Monster:
	{
		int trackAnimationSet = pObject->m_pAnimationController->GetTrackAnimationSet(3);
	 	float trackPosition = pObject->m_pAnimationController->GetTrackPosition(3);

		std::shared_ptr<CuttedObject> pCuttedObject = std::make_shared<CuttedObject>(pd3dDevice, pd3dCommandList,
			objectData, pModelData, planeCnt, directions,xmf3PlaneNormals, trackAnimationSet, trackPosition, nullptr);
		tmpObject = std::static_pointer_cast<Object>(pCuttedObject);

		strcpy_s(pCuttedObject->m_pstrFileName, pstrFileName.c_str());

		g_vpAllObjs.emplace_back(tmpObject);
		g_vpCuttedObjects.emplace_back(tmpObject);
		m_vObjectLayer[RenderLayer::Render_CuttedSkinned].emplace_back(tmpObject);
		pCuttedObject->SetCuttedCBIdx(CUTTED_CB_SKINNED_IDX);
	}
	break;

	case Object_Movable:
	{
		std::shared_ptr<CuttedObject> pCuttedObject = std::make_shared<CuttedObject>(pd3dDevice, pd3dCommandList,
			objectData, pModelData, planeCnt, directions, xmf3PlaneNormals, nullptr);
		tmpObject = std::static_pointer_cast<Object>(pCuttedObject);

		strcpy_s(pCuttedObject->m_pstrFileName, pstrFileName.c_str());

		g_vpAllObjs.emplace_back(tmpObject);
		g_vpCuttedObjects.emplace_back(tmpObject);
		if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_TextureMesh)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedTexture].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_TEXTURE_IDX);
		}
		else if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_Static)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedStatic].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_STATIC_IDX);
		}
	}
	break;

	default:
	{
		std::shared_ptr<CuttedObject> pCuttedObject = std::make_shared<CuttedObject>(pd3dDevice, pd3dCommandList,
			objectData, pModelData, planeCnt, directions, xmf3PlaneNormals, nullptr);
		tmpObject = std::static_pointer_cast<Object>(pCuttedObject);

		strcpy_s(pCuttedObject->m_pstrFileName, pstrFileName.c_str());

		g_vpAllObjs.emplace_back(tmpObject);
		g_vpCuttedObjects.emplace_back(tmpObject);
		if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_TextureMesh)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedTexture].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_TEXTURE_IDX);
		}
		else if (g_DefaultObjectData[pstrFileName].renderLayer == RenderLayer::Render_Static)
		{
			m_vObjectLayer[RenderLayer::Render_CuttedStatic].emplace_back(tmpObject);
			pCuttedObject->SetCuttedCBIdx(CUTTED_CB_STATIC_IDX);
		}
	}
	break;
	}

	XMVECTOR cuttingVel = XMLoadFloat3(&xmf3PlaneNormal);
	cuttingVel += noiseDir;
	cuttingVel *= cuttingPower * direction;
	XMFLOAT3 xmf3CuttingVel;
	XMStoreFloat3(&xmf3CuttingVel, cuttingVel);
	tmpObject->GetBody()->AddVelocity(xmf3CuttingVel);

	XMFLOAT3 xmf3Angularvel;
	XMStoreFloat3(&xmf3Angularvel, angularVel);
	tmpObject->GetBody()->AddAngleVelocity(xmf3Angularvel);

	return tmpObject;
}

void Scene::GenerateContact()
{
	unsigned int nContactCnt = MAX_CONTACT_CNT * 8;

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if(g_vpAllObjs[i]->GetBody())
			g_vpAllObjs[i]->GetBody()->ClearContact();
	}

	m_CollisionData.Reset(nContactCnt);
	m_CollisionData.friction = 0;
	m_CollisionData.restitution = 0.1f;
	m_CollisionData.tolerance = 0.1f;

	// 캐릭터 검사
	for (int k = 0; k < g_vpCharacters.size(); ++k)
	{
		if (!g_vpCharacters[k]->GetIsAlive()) continue;
		ColliderBox* characterBox = (ColliderBox*)g_vpCharacters[k]->GetCollider().get();
		XMVECTOR position_1 = XMLoadFloat3(&g_vpCharacters[k]->GetPosition());

		if (!characterBox) continue;

		for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*characterBox, *g_ppColliderPlanes[i], m_CollisionData);
		}

		for (int i = 0; i < g_vpAllObjs.size(); ++i)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			if (!g_vpAllObjs[i]->GetIsAlive()) continue;
			if (g_vpAllObjs[i]->GetColliderType() != ColliderType::Collider_Box) continue;

			if (!g_vpAllObjs[i]->GetCollider()->GetCharacterActive()) continue;

			ColliderBox* pColliderBox = (ColliderBox*)g_vpAllObjs[i]->GetCollider().get();

			if (characterBox == pColliderBox) continue;

			// 박스 검사 이전 가능 여부 선행 검사 
			//if (!characterBox->GetBoundingSphere()->Intersects(*(pColliderBox->GetBoundingSphere().get())))
			//	continue;

			XMVECTOR position_2 = XMLoadFloat3(&g_vpAllObjs[i]->GetPosition());
			float distance = XMVectorGetX(XMVector3Length(position_1 - position_2));
			if (distance > 100) continue;

			Object* pObject1 = g_vpCharacters[k].get();
			Object* pObject2 = g_vpAllObjs[i].get();
			CollisionDetector::BoxAndBox(*characterBox, *pColliderBox, m_CollisionData, pObject1, pObject2);
		}
	}

	m_CollisionData.friction = 0.9f;
	// 평면과의 검사
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		XMVECTOR planeNormal = XMLoadFloat3(&g_ppColliderPlanes[i]->GetDirection());
		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (!g_vpWorldObjs[k]->GetIsAlive()) continue;
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[k]->GetCollider());
			if (!colliderBox) continue;
			//if (!colliderBox->GetBoundingSphere().Intersects(planeNormal))
			//	continue;

			CollisionDetector::BoxAndHalfSpace(*colliderBox, *g_ppColliderPlanes[i], m_CollisionData);
		}
	}
	// 박스끼리 검사
	for (int i = 0; i < g_vpWorldObjs.size(); ++i)
	{
		if (!g_vpWorldObjs[i]->GetIsAlive()) continue;
		if (g_vpWorldObjs[i]->GetColliderType() != ColliderType::Collider_Box) continue;
		if (g_vpWorldObjs[i]->GetObjectType() != ObjectType::Object_Movable) continue;
		ColliderBox* pColliderBox1 = (ColliderBox*)g_vpWorldObjs[i]->GetCollider().get();

		XMVECTOR position_1 = XMLoadFloat3(&g_vpWorldObjs[i]->GetPosition());

		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			if (g_vpWorldObjs[i] == g_vpWorldObjs[k]) continue;
			if (!g_vpWorldObjs[k]->GetIsAlive()) continue;
			if (g_vpWorldObjs[k]->GetColliderType() != ColliderType::Collider_Box) continue;

			ColliderBox* pColliderBox2 = (ColliderBox*)g_vpWorldObjs[k]->GetCollider().get();
			if (pColliderBox1 == pColliderBox2) continue;

			XMVECTOR position_2 = XMLoadFloat3(&g_vpWorldObjs[k]->GetPosition());
			float distance = XMVectorGetX(XMVector3Length(position_1 - position_2));
			if (distance > 100) continue;

			// 박스 검사 이전 가능 여부 선행 검사 
			//if (!pColliderBox1->GetBoundingSphere()->Intersects(*(pColliderBox2->GetBoundingSphere().get())))
			//	continue;

			Object* pObject1 = g_vpWorldObjs[i].get();
			Object* pObject2 = g_vpWorldObjs[k].get();
			CollisionDetector::BoxAndBox(*pColliderBox1, *pColliderBox2, m_CollisionData, pObject1, pObject2);
		}
	}

	// 잘린 물체 검사
	// 평면과의 검사
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		for (int k = 0; k < g_vpCuttedObjects.size(); ++k)
		{
			std::shared_ptr<ColliderBox> colliderBox = std::static_pointer_cast<ColliderBox>(g_vpCuttedObjects[k]->GetCollider());
			if (!colliderBox) continue;

			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*colliderBox, *g_ppColliderPlanes[i], m_CollisionData);
		}
	}
	// 박스와 검사
	for (int i = 0; i < g_vpCuttedObjects.size(); ++i)
	{
		if (!g_vpCuttedObjects[i]->GetIsAlive()) continue;
		ColliderBox* pColliderBox1 = (ColliderBox*)g_vpCuttedObjects[i]->GetCollider().get();

		XMVECTOR position_1 = XMLoadFloat3(&g_vpCuttedObjects[i]->GetPosition());

		for (int k = 0; k < g_vpWorldObjs.size(); ++k)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			if (!g_vpWorldObjs[k]->GetIsAlive()) continue;
			if (g_vpWorldObjs[k]->GetObjectType() == Object_Movable) continue;
			if (g_vpWorldObjs[k]->GetColliderType() != ColliderType::Collider_Box) continue;

			ColliderBox* pColliderBox2 = (ColliderBox*)g_vpWorldObjs[k]->GetCollider().get();
			if (pColliderBox1 == pColliderBox2) continue;

			XMVECTOR position_2 = XMLoadFloat3(&g_vpWorldObjs[k]->GetPosition());
			float distance = XMVectorGetX(XMVector3Length(position_1 - position_2));
			if (distance > 100) continue;

			Object* pObject1 = g_vpCuttedObjects[i].get();
			Object* pObject2 = g_vpWorldObjs[k].get();
			CollisionDetector::BoxAndBox(*pColliderBox1, *pColliderBox2, m_CollisionData, pObject1, pObject2);
		}
	}
}

void Scene::ProcessPhysics(float elapsedTime)
{
	m_pCollisionResolver->ResolveContacts(m_CollisionData.pContacts, elapsedTime);
}

void Scene::PlayCinematic()
{
	if (m_pCinematic == nullptr)
		return;

	m_bInCinematic = true;
	m_pCinematic->Play();
}

void Scene::ClearObjectLayer()
{
	for (int j = 0; j < g_ppColliderBoxs.size(); ++j)
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
	}

	// 전체 순회
	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		if (!g_vpAllObjs[i]->GetIsAlive())
		{
			g_vpAllObjs[i]->DestroyRunTime();
			g_vpAllObjs.erase(g_vpAllObjs.begin() + i);
		}
	}
	for (int i = 0; i < g_vpMovableObjs.size(); ++i)
	{
		if (!g_vpMovableObjs[i]->GetIsAlive())
		{
			g_vpMovableObjs[i]->DestroyRunTime();
			g_vpMovableObjs.erase(g_vpMovableObjs.begin() + i);
		}
	}
	for (int i = 0; i < g_vpCharacters.size(); ++i)
	{
		if (!g_vpCharacters[i]->GetIsAlive())
		{
			g_vpCharacters[i]->DestroyRunTime();
			g_vpCharacters.erase(g_vpCharacters.begin() + i);
		}
	}
	for (int i = 0; i < g_vpWorldObjs.size(); ++i)
	{
		if (!g_vpWorldObjs[i]->GetIsAlive())
		{
			g_vpWorldObjs[i]->DestroyRunTime();
			g_vpWorldObjs.erase(g_vpWorldObjs.begin() + i);
		}
	}
	for (int i = 0; i < g_vpCuttedObjects.size(); ++i)
	{
		if (!g_vpCuttedObjects[i]->GetIsAlive())
		{
			g_vpCuttedObjects[i]->DestroyRunTime();
			g_vpCuttedObjects.erase(g_vpCuttedObjects.begin() + i);
		}
	}

	for (int i = 0; i < g_vpEventObjs.size(); ++i)
	{
		if (!g_vpEventObjs[i]->GetIsAlive())
		{
			g_vpEventObjs[i]->DestroyRunTime();
			g_vpEventObjs.erase(g_vpEventObjs.begin() + i);
		}
	}


	for (int i = 0; i < m_pEventObjects.size(); ++i)
	{
		if (!m_pEventObjects[i]->GetIsAlive())
		{
			m_pEventObjects[i]->DestroyRunTime();
			m_pEventObjects.erase(m_pEventObjects.begin() + i);
		}
	}

	// 레이어 순회
	for (int i = 0; i < RenderLayer::Render_Count; ++i)
	{
		for (int j = 0; j < m_vObjectLayer[i].size(); ++j)
		{
			if (!m_vObjectLayer[i][j]->GetIsAlive())
			{
				m_vObjectLayer[i][j]->DestroyRunTime();
				m_vObjectLayer[i].erase(m_vObjectLayer[i].begin() + j);
			}
		}
	}
}

// Sound
void Scene::EmitSound(const char* pstrFilePath, bool bLoop)
{
	std::shared_ptr<CSound> newSound = std::make_shared<CSound>(pstrFilePath, bLoop);
	newSound->Play();
	m_vpSounds.emplace_back(newSound);
}

void Scene::EmitSound(const char* pstrFilePath, bool bLoop, float pitch, float volume)
{
	std::shared_ptr<CSound> newSound = std::make_shared<CSound>(pstrFilePath, bLoop);
	newSound->Play();
	newSound->SetPitch(pitch);
	newSound->SetVolme(volume);
	m_vpSounds.emplace_back(newSound);
}

void Scene::EmitHitSound(SoundType nType, bool bLoop)
{
	char pstrFilePath[64];
	int tmpRand;

	switch (nType)
	{
	case Sound_Wood_Light:
		strcpy_s(pstrFilePath, WOOD_LIGHT_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Wood_Heavy:
		strcpy_s(pstrFilePath, WOOD_HEAVY_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Stone:
		strcpy_s(pstrFilePath, STONE_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Steel_Light:
		strcpy_s(pstrFilePath, STEEL_LIGHT_HIT_SOUND);
		tmpRand = rand() % 3 + 1;
		break;
	case Sound_Steel_Heavy:
		strcpy_s(pstrFilePath, STEEL_HEAVY_HIT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Fabric:
		strcpy_s(pstrFilePath, FABRIC_HIT_SOUND);
		tmpRand = rand() % 1 + 1;
		break;
	case Sound_Character:
		strcpy_s(pstrFilePath, CHARACTER_HIT_SOUND);
		tmpRand = rand() % 3 + 1;
		break;
	default:
		return;
	}

	strcat_s(pstrFilePath, std::to_string(tmpRand).c_str());
	strcat_s(pstrFilePath, ".wav");
	EmitSound(pstrFilePath, bLoop, 1.0f, 0.3f);
}

void Scene::EmitCutSound(SoundType nType, bool bLoop)
{
	char pstrFilePath[64];
	int tmpRand;
	float volume = 0.3f;

	switch (nType)
	{
	case Sound_Wood_Light:
		strcpy_s(pstrFilePath, WOOD_LIGHT_CUT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Wood_Heavy:
		strcpy_s(pstrFilePath, WOOD_HEAVY_CUT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Stone:
		strcpy_s(pstrFilePath, STONE_CUT_SOUND);
		tmpRand = rand() % 2 + 1;
		volume = 1.0f;
		break;
	case Sound_Steel_Light:
		strcpy_s(pstrFilePath, STEEL_LIGHT_CUT_SOUND);
		tmpRand = rand() % 3 + 1;
		volume = 1.0f;
		break;
	case Sound_Steel_Heavy:
		strcpy_s(pstrFilePath, STEEL_HEAVY_CUT_SOUND);
		tmpRand = rand() % 3 + 1;
		volume = 1.0f;
		break;
	case Sound_Fabric:
		strcpy_s(pstrFilePath, FABRIC_CUT_SOUND);
		tmpRand = rand() % 1 + 1;
		break;
	case Sound_Character:
		strcpy_s(pstrFilePath, CHARACTER_CUT_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	default:
		return;
	}

	strcat_s(pstrFilePath, std::to_string(tmpRand).c_str());
	strcat_s(pstrFilePath, ".wav");

	EmitSound(pstrFilePath, bLoop, 1.0f, volume);
}


void Scene::EmitBrokenSound(SoundType nType, bool bLoop)
{
	char pstrFilePath[64];
	int tmpRand;
	float volume = 0.3f;

	switch (nType)
	{
	case Sound_Wood_Light:
		strcpy_s(pstrFilePath, WOOD_LIGHT_BROKEN_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Wood_Heavy:
		strcpy_s(pstrFilePath, WOOD_HEAVY_BROKEN_SOUND);
		tmpRand = rand() % 4 + 1;
		break;
	case Sound_Stone:
		strcpy_s(pstrFilePath, STONE_BROKEN_SOUND);
		tmpRand = rand() % 3 + 1;
		volume = 1.0f;
		break;
	case Sound_Steel_Light:
		strcpy_s(pstrFilePath, STEEL_LIGHT_BROKEN_SOUND);
		tmpRand = rand() % 2 + 1;
		break;
	case Sound_Steel_Heavy:
		strcpy_s(pstrFilePath, STEEL_HEAVY_BROKEN_SOUND);
		tmpRand = rand() % 2 + 1;
		volume = 0.1f;
		break;
	case Sound_Fabric:
		strcpy_s(pstrFilePath, FABRIC_BROKEN_SOUND);
		tmpRand = rand() % 1 + 1;
		volume = 0.5f;
		break;
	default:
		return;
	}

	strcat_s(pstrFilePath, std::to_string(tmpRand).c_str());
	strcat_s(pstrFilePath, ".wav");
	EmitSound(pstrFilePath, bLoop, 1.0f, volume);
}

void Scene::ChangeStage()
{
	if (m_bNextStage)
	{
		m_pTextUIs->UpdateTextUI(L"Loading...", m_pTextUIs->GetTextUIPosX(Text_UI_Idx_Loading), m_pTextUIs->GetTextUIPosY(Text_UI_Idx_Loading), Text_UI_Idx_Loading);
		if (m_FadeInValue <= 0.0f)
		{
			m_PlayerStartScore = ((Player*)g_pPlayer.get())->GetScore();

			m_nStageNum += 1;
			m_nStageNum %= 3;
			StageStart(m_nStageNum);
			m_bFadeTimer = true;
			m_bNextStage = false;
		}
		else if (m_FadeInValue >= 1.0f && m_FadeState == 2)
		{
			m_FadeState = 0;
		}
	}
}

void Scene::GameStart()
{
#if not defined(_DEBUG)
	// 페이드 아웃 상태로 시작
	m_FadeState = 0;
	m_FadeInValue = 0.0f;
#endif
	

	if (g_pPlayer)
	{
		g_pPlayer->DestroyRunTime();
		g_pPlayer = nullptr;
		m_PlayerStartScore = 0;
	}

	// 0번 스테이지부터 시작
	m_nStageNum = 0;
	
#if defined(_DEBUG)
	 m_nStageNum = 3;
#endif
	// 스테이지 초기화 함수
	StageStart(m_nStageNum);

#if not defined(_DEBUG)
	// 페이드 인
	m_FadeState = 1;
#endif

	// 시작 시네마틱 초기화
	InitCinematic();
	
	// 플레이 직후 종료 => 키입력 후부터 시작
	PlayCinematic();
	m_pCinematic->Stop();
	// 키입력 수신
	m_bPressAnyKey = true;
	m_bGameStart = true;
	m_bTitle = true;

	EmitSound("Sound/Ambient/GameStart.wav", false, 1.0f, 0.15f);
	EmitSound("Sound/BGM/Start.mp3", true, 0.8f, 0.1f);
}

void Scene::Restart()
{
	// 스테이지 시작 점수로 복귀
	((Player*)(g_pPlayer.get()))->SetScore(m_PlayerStartScore);
	((Player*)(g_pPlayer.get()))->SetHP(g_pPlayer->GetMaxHP());

	m_nStageNum -= 1;
	m_bNextStage = true;
}

void Scene::GameOver()
{
	if (g_pPlayer->GetHP() <= 0)
	{
		m_bGameOver = true;
	}

	if (m_bGameOver)
	{
		if (m_FadeInValue <= 0.0f)
		{
			m_pBigSizeTextUI->UpdateTextUI(L"Game Over",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_Over),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_Over),
				Big_Text_UI_Idx_Over);

			m_bPressAnyKey = true;
		}
		else if (m_FadeInValue >= 1.0f && m_FadeState == 2)
		{
			m_FadeState = 0;
			for (int i = 0; i < m_vpSounds.size(); ++i)
			{
				m_vpSounds[i]->Stop();
				m_vpSounds[i].reset();
				m_vpSounds.erase(m_vpSounds.begin() + i);
			}
			EmitSound("Sound/BGM/GameOver.mp3", false, 1.0f, 0.2f);
		}
	}
}

void Scene::GameEnd()
{
	if (m_bGameEnd && !m_bInCinematic)
	{
		if (m_FadeInValue <= 0.0f)
		{
			m_pBigSizeTextUI->UpdateTextUI(L"The End.",
				m_pBigSizeTextUI->GetTextUIPosX(Big_Text_UI_Idx_End),
				m_pBigSizeTextUI->GetTextUIPosY(Big_Text_UI_Idx_End),
				Big_Text_UI_Idx_End);

			m_bThanks = true;
			m_bPressAnyKey = true;
		}
		else if (m_FadeInValue >= 1.0f && m_FadeState == 2)
		{
			m_FadeState = 0;
		}
	}
}

void Scene::StageStart(UINT nMapNum)
{
	float hp = 300;
	UINT score = 0;
	if (g_pPlayer)
	{
		hp = g_pPlayer->GetHP();
		score = ((Player*)(g_pPlayer.get()))->GetScore();
	}

	for (int i = 0; i < g_vpAllObjs.size(); ++i)
	{
		g_vpAllObjs[i]->DestroyRunTime();
	}
	if(g_pEnterObject) g_pEnterObject->DestroyRunTime();

	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		m_vpSounds[i]->Stop();
		m_vpSounds[i].reset();
		m_vpSounds.erase(m_vpSounds.begin() + i);
	}

	ClearObjectLayer();

	// 플레이어
	CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 10, -20), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0f, 1.0f, 1.0f),
		CHARACTER_MODEL_NAME, PLAYER_TRACK_CNT);
	g_pPlayer->SetHP(hp);
	((Player*)(g_pPlayer.get()))->SetScore(score);

	// 카메라 초기화
	m_pCamera = nullptr;
	m_pCamera = std::make_unique<Third_Person_Camera>(g_pPlayer);
	m_pCamera->SetPitch(15);

	// 무기
	CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(-0.59f, 0.135f, 0.063f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(50, 0, 90), XMFLOAT3(1, 1, 1), WEAPON_MODEL_NAME, 0);

	switch (nMapNum)
	{
	case 0:
		g_pPlayer->SetRotate(XMFLOAT3(0, 90, 0));
		m_pCamera->SetYaw(90);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Outside/Outside_EventObjs", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Outside/OutSideMap", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Outside/Outside", false);
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, -1, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -460);
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(1, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -150);
		break;
	case 1:
		g_pPlayer->SetRotate(XMFLOAT3(0, 180, 0));
		m_pCamera->RotateY(180);
		g_pPlayer->GetBody()->SetPosition(XMFLOAT3(0, 10, 10));
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Hospital/Hospital_EventObjs", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Hospital/HospitalInsideMap", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Hospital/Hospital", false);
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, -1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -40);
		break;
	case 2:
		g_pPlayer->SetRotate(XMFLOAT3(0, 90, 0));
		m_pCamera->RotateY(90);
		g_pPlayer->GetBody()->SetPosition(XMFLOAT3(20, 10, 0));
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Dungeon/Dungeon_EventObjs", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Dungeon/DungeonMap", false);
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "Dungeon/Dungeon", false);
		CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, -1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -80);

		EmitSound("Sound/BGM/Dungeon.wav", true, 0.8f, 0.2f);

		break;

	case 3:
		g_pPlayer->SetRotate(XMFLOAT3(0, 90, 0));
		m_pCamera->RotateY(90);
		g_pPlayer->GetBody()->SetPosition(XMFLOAT3(20, 10, 0));
		LoadMapData(g_pd3dDevice, g_pd3dCommandList, "TestMap", false);
		//CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, -1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, -80);

		break;
	default:
		assert(false);
	}

	// 바닥
	CreateObject(g_pd3dDevice, g_pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), nullptr, 0);

}

bool Scene::InitCinematic()
{
	m_pCinematic->Destroy();
	m_pCinematic.reset();
	
	m_pCinematicCamera = std::make_shared<CinematicCamera>();
	m_pCinematicCamera->SetPosition(XMFLOAT3(35, 20, -55));
	m_pCinematicCamera->SetOrientationByRotation(XMFLOAT3(10, -30, 0));
	m_pCinematicCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();
	
	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, XMFLOAT3(35, 20, -55), XMFLOAT3(10, -30, 0));
	pCinematic->AddCameraKeyFrame(1.0f, XMFLOAT3(35, 20, -55), XMFLOAT3(10, -30, 0));
	pCinematic->AddCameraKeyFrame(5.0f, XMFLOAT3(-50, 38, -20), XMFLOAT3(15, 90, 0));
	pCinematic->AddCameraKeyFrame(6.0f, XMFLOAT3(-48, 38, -20), XMFLOAT3(15, 90, 0));
	
	pCinematic->AddSoundKeyFrame(0.0f, "Sound/Ambient/Ravens.wav", 0.5f);

	m_pCinematic = pCinematic;
	
	return true;
}

void Scene::OutsideCine1() // 대문 통과
{
	m_pCinematic.reset();
	m_pCinematic = nullptr;	

	//pCinematic->AddSoundKeyFrame(0.5f, "Sound/Item/Heal.wav", 0.5f);
	//pCinematic->AddSoundKeyFrame(0.5f, "Sound/Item/SpawnScore.wav", 0.5f);

	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition3f();
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraRotation.x = m_pCamera->GetPitch();
	xmf3CameraRotation.y = m_pCamera->GetYaw();
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(1.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 160;
	xmf3CameraRotation.x = 25;
	xmf3CameraRotation.y = 0;
	pCinematic->AddCameraKeyFrame(2.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 160;
	xmf3CameraPosition.y = 50;
	xmf3CameraPosition.z = 100;
	pCinematic->AddCameraKeyFrame(5.5f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(6.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 200;
	xmf3CameraPosition.y = 60;
	xmf3CameraPosition.z = 320;
	xmf3CameraRotation.x = 20;
	xmf3CameraRotation.y = -170;
	pCinematic->AddCameraKeyFrame(7.5f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(10.f, xmf3CameraPosition, xmf3CameraRotation);

	std::shared_ptr<Object> pObject;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	// 오브젝트 연결 및 조작
	for (int i = 0; i < m_pEventObjects.size(); ++i)
	{
		pObject = m_pEventObjects[i];
		position = pObject->GetPosition();
		rotation = pObject->GetRotation();
		scale = pObject->GetScale();

		pCinematic->AddTrack(pObject, position, rotation, scale);
		pCinematic->AddKeyFrame(i, 2.0f, position, rotation, scale, &Object::MoveForward); // 
		position.x += 120;
		position.z -= 130;
		pCinematic->AddKeyFrame(i, 3.7f, position, rotation, scale, &Object::MoveStop); //
		rotation.y = 180;
		pCinematic->AddKeyFrame(i, 4.7f, position, rotation, scale, nullptr); // 대문쪽을 쳐다봄
		pCinematic->AddKeyFrame(i, 6.0f, position, rotation, scale, nullptr); // 대문쪽을 쳐다봄
		pCinematic->AddKeyFrame(i, 8.0f, position, rotation, scale, &Object::CinematicAction);	// 한번 휘적
		//pCinematic->AddKeyFrame(i, 10.0f, position, rotation, scale, &Object::CinematicFindPlayer);	// 플레이어 공격
	}

	pObject = g_pPlayer;
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	rotation.y = 0;
	pCinematic->AddKeyFrame(8, 6.0f, position, rotation, scale, nullptr); // 대문쪽을 쳐다봄

	pObject = g_vpWorldObjs[0];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(9, 2.0f, position, rotation, scale, nullptr); 
	position.x = 190;
	position.z = 30;
	rotation.z = -90;
	pCinematic->AddKeyFrame(9, 3.0f, position, rotation, scale, nullptr); 

	pObject = g_vpWorldObjs[1];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(10, 2.0f, position, rotation, scale, nullptr);
	position.x = 130;
	position.z = 30;
	rotation.z = 90;
	pCinematic->AddKeyFrame(10, 3.0f, position, rotation, scale, nullptr);

	//pCinematic->AddSoundKeyFrame(0.0f, "Sound/BGM/Start.mp3", 0.1f);
	pCinematic->AddSoundKeyFrame(1.0f, "Sound/Ambient/CameraMove2.wav", 0.5f);
	pCinematic->AddSoundKeyFrame(2.0f, "Sound/Ambient/MetalDoorOpen.mp3", 0.5f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}
void Scene::OutsideCine2() // 좀비 모두 처치 후 포탈 강조
{
	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition3f();
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraRotation.x = m_pCamera->GetPitch();
	xmf3CameraRotation.y = m_pCamera->GetYaw();
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(1.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 75;
	xmf3CameraPosition.y = 55;
	xmf3CameraPosition.z = 340;
	xmf3CameraRotation.x = 25;
	xmf3CameraRotation.y = 0;
	pCinematic->AddCameraKeyFrame(4.0f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(6.0f, xmf3CameraPosition, xmf3CameraRotation);

	pCinematic->AddSoundKeyFrame(1.5f, "Sound/Ambient/CameraMove1.wav", 0.5f);
	pCinematic->AddSoundKeyFrame(1.5f, "Sound/Ambient/SomethingChange.wav", 0.2f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}

void Scene::HospitalCine1() // 스테이지 시작
{
	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3PlayerPosition = g_pPlayer->GetPosition();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition3f();
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraRotation.x = 15;
	xmf3CameraRotation.y = 180;
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);

	pCinematic->AddSoundKeyFrame(2.5f, "Sound/Ambient/CameraMove1.wav", 0.3f);

	pCinematic->AddCameraKeyFrame(3.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 155;
	xmf3CameraPosition.y = 140;
	xmf3CameraPosition.z = -110;
	xmf3CameraRotation.x = 70;
	pCinematic->AddCameraKeyFrame(5.0f, xmf3CameraPosition, xmf3CameraRotation);

	pCinematic->AddSoundKeyFrame(5.5f, "Sound/Ambient/CameraMove1.wav", 0.3f);

	pCinematic->AddCameraKeyFrame(6.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 48;
	xmf3CameraPosition.z = -46;
	xmf3CameraRotation.y = 60;
	pCinematic->AddCameraKeyFrame(7.5f, xmf3CameraPosition, xmf3CameraRotation);

	pCinematic->AddSoundKeyFrame(8.0f, "Sound/Ambient/CameraMove1.wav", 0.3f);

	pCinematic->AddCameraKeyFrame(8.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = -76;
	xmf3CameraPosition.z = -87;
	xmf3CameraRotation.y = 0;
	pCinematic->AddCameraKeyFrame(9.5f, xmf3CameraPosition, xmf3CameraRotation);

	pCinematic->AddSoundKeyFrame(10.5f, "Sound/Ambient/CameraMove1.wav", 0.3f);

	pCinematic->AddCameraKeyFrame(10.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = -120;
	xmf3CameraPosition.z = -160;
	xmf3CameraRotation.y = -120;
	pCinematic->AddCameraKeyFrame(11.5f, xmf3CameraPosition, xmf3CameraRotation);

	pCinematic->AddSoundKeyFrame(12.0f, "Sound/Ambient/CameraMove1.wav", 0.3f);

	pCinematic->AddCameraKeyFrame(12.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 3;
	xmf3CameraPosition.y = 20;
	xmf3CameraPosition.z = -60;
	xmf3CameraRotation.x = 0;
	xmf3CameraRotation.y = 180;
	pCinematic->AddCameraKeyFrame(13.5f, xmf3CameraPosition, xmf3CameraRotation);

	pCinematic->AddSoundKeyFrame(14.0f, "Sound/Ambient/CameraMove1.wav", 0.3f);

	pCinematic->AddCameraKeyFrame(14.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 3;
	xmf3CameraPosition.z = -270;
	pCinematic->AddCameraKeyFrame(17.0f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(18.5f, xmf3CameraPosition, xmf3CameraRotation);

	EmitSound("Sound/BGM/Hospital.wav", true, 0.8f, 0.2f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}
void Scene::HospitalCine2() // 몬스터 모두 처치 후 문 열림
{
	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3PlayerPosition = g_pPlayer->GetPosition();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition3f();
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraRotation.x = m_pCamera->GetPitch();
	xmf3CameraRotation.y = m_pCamera->GetYaw();
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(1.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 3;
	xmf3CameraPosition.y = 20;
	xmf3CameraPosition.z = -60;
	xmf3CameraRotation.x = 0;
	xmf3CameraRotation.y = 180;
	pCinematic->AddCameraKeyFrame(2.5f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(6.5f, xmf3CameraPosition, xmf3CameraRotation);

	std::shared_ptr<Object> pObject;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	pObject = g_vpWorldObjs[0];	
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(0, 3.0f, position, rotation, scale, nullptr); 
	position.x = 13;
	position.z = -160;
	rotation.y = 90;
	pCinematic->AddKeyFrame(0, 4.0f, position, rotation, scale, nullptr); 

	pObject = g_vpWorldObjs[1];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(1, 3.0f, position, rotation, scale, nullptr); 
	position.x = -6;
	position.z = -160;
	rotation.y = 90;
	pCinematic->AddKeyFrame(1, 4.0f, position, rotation, scale, nullptr);

	pCinematic->AddSoundKeyFrame(3.0f, "Sound/Ambient/SomethingChange.wav", 0.1f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}
void Scene::HospitalCine3() // 문 입장 후 다시 문 닫힘, 보스몬스터 포효
{
	// 사운드 초기화
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		m_vpSounds[i]->Stop();
		m_vpSounds[i].reset();
		m_vpSounds.erase(m_vpSounds.begin() + i);
	}

	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3CameraPosition = XMFLOAT3(-75, 60, -260);
	XMFLOAT3 xmf3CameraRotation = XMFLOAT3(30, 45, 0);

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	//pCinematic->AddCameraKeyFrame(1.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraRotation.y = 135;
	pCinematic->AddCameraKeyFrame(2.0f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(3.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraRotation.y = 55;
	pCinematic->AddCameraKeyFrame(5.5f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(8.5f, xmf3CameraPosition, xmf3CameraRotation);

	std::shared_ptr<Object> pObject;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	
	pObject = g_pPlayer;
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	position.x = 5;
	position.z = -170;
	rotation.y = 180;
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(0, 0.1f, position, rotation, scale, &Object::WalkForward);
	position.x = 5;
	position.z = -325;
	pCinematic->AddKeyFrame(0, 2.0f, position, rotation, scale, &Object::MoveStop);
	pCinematic->AddKeyFrame(0, 3.0f, position, rotation, scale, nullptr);
	rotation.y = 0;
	pCinematic->AddKeyFrame(0, 4.0f, position, rotation, scale, nullptr);
	
	m_pCamera->SetYaw(0);

	// 중간 보스
	pObject = m_pEventObjects[0];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(1, 3.5f, position, rotation, scale, &Object::MoveForward);
	position.x = 5;
	position.z = -200;
	rotation.y = 180;
	pCinematic->AddKeyFrame(1, 5.5f, position, rotation, scale, &Object::MoveStop);
	pCinematic->AddKeyFrame(1, 6.0f, position, rotation, scale, &Object::CinematicAction);
	pCinematic->AddKeyFrame(1, 8.5f, position, rotation, scale, &Object::CinematicFindPlayer);
	
	// 왼쪽 문
	pObject = g_vpWorldObjs[0];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	position.x = 13;
	position.z = -160;
	rotation.y = 90;
	pCinematic->AddTrack(pObject, position, rotation, scale);
	//pCinematic->AddKeyFrame(2, 1.5f, position, rotation, scale, nullptr);
	pCinematic->AddKeyFrame(2, 6.0f, position, rotation, scale, nullptr);
	position.x = 8.5f;
	position.z = -165;
	rotation.y = 0;
	//pCinematic->AddKeyFrame(2, 3.0f, position, rotation, scale, nullptr);
	pCinematic->AddKeyFrame(2, 7.5f, position, rotation, scale, nullptr);
	
	// 오른쪽 문
	pObject = g_vpWorldObjs[1];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	position.x = -6;
	position.z = -160;
	rotation.y = 90;
	pCinematic->AddTrack(pObject, position, rotation, scale);
	//pCinematic->AddKeyFrame(3, 1.5f, position, rotation, scale, nullptr);
	pCinematic->AddKeyFrame(3, 6.0f, position, rotation, scale, nullptr);
	position.x = -0.5f;
	position.z = -165;
	rotation.y = 180;
	//pCinematic->AddKeyFrame(3, 3.0f, position, rotation, scale, nullptr);
	pCinematic->AddKeyFrame(3, 7.5f, position, rotation, scale, nullptr);
	

	pCinematic->AddSoundKeyFrame(3.0f, "Sound/Ambient/TwinsEnter.wav", 0.1f);
	pCinematic->AddSoundKeyFrame(7.5f, "Sound/BGM/TwinsBattle.wav", 0.1f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}
void Scene::HospitalCine4() // 보스 처치후 문이 열리며 포탈 생성
{
	// 사운드 초기화
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		m_vpSounds[i]->Stop();
		m_vpSounds[i].reset();
		m_vpSounds.erase(m_vpSounds.begin() + i);
	}

	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3PlayerPosition = g_pPlayer->GetPosition();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition3f();
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraRotation.x = m_pCamera->GetPitch();
	xmf3CameraRotation.y = m_pCamera->GetYaw();
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(1.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 3;
	xmf3CameraPosition.y = 20;
	xmf3CameraPosition.z = -270;
	xmf3CameraRotation.x = 0;
	xmf3CameraRotation.y = 180;
	pCinematic->AddCameraKeyFrame(2.5f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(4.5f, xmf3CameraPosition, xmf3CameraRotation);

	std::shared_ptr<Object> pObject;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	//  문
	pObject = g_vpWorldObjs[2];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(0, 2.5f, position, rotation, scale, nullptr);
	position.x = -5.5f;
	position.z = -347;
	rotation.y = -90;
	pCinematic->AddKeyFrame(0, 3.5f, position, rotation, scale, nullptr);

	pCinematic->AddSoundKeyFrame(2.5f, "Sound/Ambient/DungeonDoorOpen.wav", 0.2f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}

void Scene::DungeonCine1() // 보스룸 열림
{
	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3PlayerPosition = g_pPlayer->GetPosition();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition3f();
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraRotation.x = m_pCamera->GetPitch();
	xmf3CameraRotation.y = m_pCamera->GetYaw();
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(1.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 12;
	xmf3CameraPosition.y = 55;
	xmf3CameraPosition.z = -350;
	xmf3CameraRotation.x = 0;
	xmf3CameraRotation.y = 90;
	pCinematic->AddCameraKeyFrame(1.5f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(4.0f, xmf3CameraPosition, xmf3CameraRotation);

	std::shared_ptr<Object> pObject;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	//  문
	pObject = g_vpWorldObjs[0];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(0, 2.5f, position, rotation, scale, nullptr);
	position.z = -405;
	pCinematic->AddKeyFrame(0, 3.5f, position, rotation, scale, nullptr);
	

	pObject = g_vpWorldObjs[1];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(1, 1.5f, position, rotation, scale, nullptr);
	position.z = -361;
	pCinematic->AddKeyFrame(1, 2.5f, position, rotation, scale, nullptr);
	position.z = -405;
	pCinematic->AddKeyFrame(1, 3.5f, position, rotation, scale, nullptr);


	pCinematic->AddSoundKeyFrame(1.5f, "Sound/Ambient/SomethingChange.wav", 0.1f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}
void Scene::DungeonCine2() // 보스룸 입장, 뭔가 하던 보스가 쳐다보며 시작
{
	// 사운드 초기화
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		m_vpSounds[i]->Stop();
		m_vpSounds[i].reset();
		m_vpSounds.erase(m_vpSounds.begin() + i);
	}

	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3CameraPosition;
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraPosition.x = 220;
	xmf3CameraPosition.y = 70;
	xmf3CameraPosition.z = -283;
	xmf3CameraRotation.x = 15;
	xmf3CameraRotation.y = 90;
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(3.0f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(4.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 350;
	xmf3CameraPosition.y = 55;
	xmf3CameraPosition.z = -225;
	xmf3CameraRotation.y = 120;
	pCinematic->AddCameraKeyFrame(4.5f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 430;
	xmf3CameraPosition.y = 40;
	xmf3CameraPosition.z = -255;
	xmf3CameraRotation.x = 10;
	xmf3CameraRotation.y = 255;
	pCinematic->AddCameraKeyFrame(5.5f, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(9.5f, xmf3CameraPosition, xmf3CameraRotation);

	std::shared_ptr<Object> pObject;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	//  문
	pObject = g_vpWorldObjs[0];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(0, 6.5f, position, rotation, scale, nullptr);
	position.z = -361;
	pCinematic->AddKeyFrame(0, 7.5f, position, rotation, scale, nullptr);

	pObject = g_vpWorldObjs[1];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(1, 6.5f, position, rotation, scale, nullptr);
	position.z = -361;
	pCinematic->AddKeyFrame(1, 7.5f, position, rotation, scale, nullptr);
	position.z = -322;
	pCinematic->AddKeyFrame(1, 8.5f, position, rotation, scale, nullptr);

	pObject = g_pPlayer;
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(2, 0.1f, position, rotation, scale, &Object::WalkForward);
	pCinematic->AddKeyFrame(2, 1.0f, position, rotation, scale, &Object::MoveStop);
	position.x = 165;
	position.z = -285;
	rotation.y = 90;
	pCinematic->AddKeyFrame(2, 1.5f, position, rotation, scale, nullptr);
	pCinematic->AddKeyFrame(2, 6.0f, position, rotation, scale, &Object::CinematicAction);

	pObject = m_pEventObjects[0];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(3, 1.0f, position, rotation, scale, &Object::CinematicAction);
	pCinematic->AddKeyFrame(3, 4.0f, position, rotation, scale, nullptr);
	rotation.y = 270;
	pCinematic->AddKeyFrame(3, 5.0f, position, rotation, scale, nullptr);
	pCinematic->AddKeyFrame(3, 9.5f, position, rotation, scale, &Object::CinematicFindPlayer);

	pCinematic->AddSoundKeyFrame(0.0f, "Sound/Ambient/DungeonEnter.wav", 0.1f);
	pCinematic->AddSoundKeyFrame(9.0f, "Sound/BGM/FinalBossBattle.wav", 0.2f);

	m_pCinematic = pCinematic;
	PlayCinematic();
}
void Scene::DungeonCine3()	// 보스 처치 후, 크리스탈 파괴하고 유유히 이탈
{
	// 사운드 초기화
	for (int i = 0; i < m_vpSounds.size(); ++i)
	{
		m_vpSounds[i]->Stop();
		m_vpSounds[i].reset();
		m_vpSounds.erase(m_vpSounds.begin() + i);
	}

	m_pCinematic.reset();
	m_pCinematic = nullptr;

	XMFLOAT3 xmf3CameraPosition;
	XMFLOAT3 xmf3CameraRotation;
	xmf3CameraPosition.x = 220;
	xmf3CameraPosition.y = 70;
	xmf3CameraPosition.z = -283;
	xmf3CameraRotation.x = 15;
	xmf3CameraRotation.y = 90;
	xmf3CameraRotation.z = 0;

	m_pCinematicCamera->SetPosition(xmf3CameraPosition);
	m_pCinematicCamera->SetOrientationByRotation(xmf3CameraRotation);

	std::shared_ptr<Cinematic> pCinematic = std::make_shared<Cinematic>();

	// 카메라 연결 및 조작
	pCinematic->AddCamera(m_pCinematicCamera, xmf3CameraPosition, xmf3CameraRotation);
	pCinematic->AddCameraKeyFrame(8.0f, xmf3CameraPosition, xmf3CameraRotation);
	xmf3CameraPosition.x = 200;
	xmf3CameraPosition.y = 40;
	xmf3CameraPosition.z = -300;
	xmf3CameraRotation.x = 5;
	xmf3CameraRotation.y = 240;
	pCinematic->AddCameraKeyFrame(10.0f, xmf3CameraPosition, xmf3CameraRotation);
	

	std::shared_ptr<Object> pObject;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	//  문
	pObject = g_vpWorldObjs[0];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(0, 11.0f, position, rotation, scale, nullptr);
	position.z = -405;
	pCinematic->AddKeyFrame(0, 12.0f, position, rotation, scale, nullptr);


	pObject = g_vpWorldObjs[1];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(1, 10.0f, position, rotation, scale, nullptr);
	position.z = -361;
	pCinematic->AddKeyFrame(1, 11.0f, position, rotation, scale, nullptr);
	position.z = -405;
	pCinematic->AddKeyFrame(1, 12.0f, position, rotation, scale, nullptr);

	pObject = g_pPlayer;
	position = pObject->GetPosition();
	position.x = 330;
	position.z = -285;
	rotation = pObject->GetRotation();
	rotation.y = 90;
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(2, 1.0f, position, rotation, scale, &Object::WalkForward);
	position.x = 380;
	position.z = -285;
	pCinematic->AddKeyFrame(2, 1.5f, position, rotation, scale, &Object::MoveStop);
	pCinematic->AddKeyFrame(2, 2.5f, position, rotation, scale, &Object::CinematicAction);
	pCinematic->AddKeyFrame(2, 8.5f, position, rotation, scale, nullptr);
	position.x = 155;
	position.z = -335;
	rotation.y = 270;
	pCinematic->AddKeyFrame(2, 9.0f, position, rotation, scale, nullptr);
	pCinematic->AddKeyFrame(2, 12.0f, position, rotation, scale, &Object::WalkForward);
	position.x = 39;
	position.z = -331;
	pCinematic->AddKeyFrame(2, 13.5, position, rotation, scale, &Object::MoveStop);

	pObject = g_vpWorldObjs[2];
	position = pObject->GetPosition();
	rotation = pObject->GetRotation();
	scale = pObject->GetScale();
	pCinematic->AddTrack(pObject, position, rotation, scale);
	pCinematic->AddKeyFrame(3, 2.5f, position, rotation, scale, &Object::DegradedBroken);

	pCinematic->AddSoundKeyFrame(4.5f, "Sound/BGM/TheEnd.wav", 0.2f);

	m_pCinematic = pCinematic;
	PlayCinematic();

	m_bGameEnd = true;
}