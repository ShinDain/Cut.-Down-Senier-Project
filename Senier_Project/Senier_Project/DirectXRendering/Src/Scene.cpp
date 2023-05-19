#include "../Header/Scene.h"

static float pEtime = 0;
CollisionData Scene::m_CollisionData;
std::unique_ptr<CollisionResolver> Scene::m_pCollisionResolver;

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
	// �н� ���� ����
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	// �� ������ �ε�
	//LoadMapData(pd3dDevice, pd3dCommandList, "Map");

	// ĳ���� �׽�Ʈ
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0,0,-50), XMFLOAT4(0,0,0,1), XMFLOAT3(0,0,0),CHARACTER_MODEL_NAME, 3);
	//m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(1, 13);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackEnable(0, true);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackEnable(1, true);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackEnable(2, false);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(2, 13);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(1, 1);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 0);

	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), ZOMBIE_MODEL_NAME, 1);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(-20, 0, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), ZOMBIE_MODEL_NAME, 1);
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(20, 0, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), ZOMBIE_MODEL_NAME, 1);

	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0.2, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(50, 0, 90), WEAPON_MODEL_NAME, 0);

	// �ӽ� �ٴ�
	CreateObject(pd3dDevice, pd3dCommandList, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1), XMFLOAT3(0,0,0), GROUND_MODEL_NAME, 0);

	// ī�޶� �ʱ�ȭ
	m_pCamera = std::make_unique<Third_Person_Camera>(m_vpAllObjs[0]);
	

#if defined(_DEBUG)
	//m_pCamera = std::make_unique<Camera>();
	//m_pCamera->SetPosition(0, 30, -100);
	//m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

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
	m_refCnt = g_LoadedModelData[CHARACTER_MODEL_NAME]->m_pRootObject.use_count();
	m_size = m_vpAllObjs.size();

	m_tTime += elapsedTime;

#endif

	// ���� �˻�
	Intersect();

	GenerateContact();
	ProcessPhysics(elapsedTime);


	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->Update(elapsedTime);
	}
	
	m_pCamera->Update(elapsedTime);

	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX viewProj = XMMatrixMultiply(view, m_pCamera->GetProj());

	// �н� ���� : �� * ���� ��ȯ ��� ������Ʈ
	PassConstant passConstant;
	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	m_pPassCB->CopyData(0, passConstant);

	// ImageObject ������ ����  ���� ������� ������Ʈ
	m_xmf4x4ImgObjMat = m_pCamera->GetOrtho4x4f();

}

void Scene::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// img ������Ʈ ������ ���� ��
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

	g_Shaders[ShaderType::Shader_TextureMesh]->ChangeShader(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());
	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_TextureMesh].size(); ++i)
	{
		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->UpdateTransform(NULL);
		m_vObjectLayer[RenderLayer::Render_TextureMesh][i]->Render(elapsedTime, pd3dCommandList);
	}

	for (int i = 0; i < m_vObjectLayer[RenderLayer::Render_Skinned].size(); ++i)
	{
		if (m_vObjectLayer[RenderLayer::Render_Skinned][i])
		{
			// Render �Լ� ������ Bone ����� ���̴��� ���޵Ǳ� ������ Render ������ �ִϸ��̼��� �������ش�.
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
	

	// �ӽ�
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

void Scene::LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName)
{
	FILE* pInFile = NULL;

	char pstrFilePath[64] = { '\0' };
	strcat_s(pstrFilePath, pstrFileName);
	strcat_s(pstrFilePath, ".bin");
	::fopen_s(&pInFile, pstrFilePath, "rb");
	::rewind(pInFile);

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		if (ReadStringFromFile(pInFile, pstrToken))
		{
			UINT nReads = 0;
			for (; ; )
			{
				ReadStringFromFile(pInFile, pstrToken);

				if (!strcmp(pstrToken, "<Frame>:"))
				{
					char tmpstr[64] = { '\0' };
					ReadStringFromFile(pInFile, tmpstr);

					//SetName(tmpstr);
				}
				else if (!strcmp(pstrToken, "<Transform>:"))
				{
					/*nReads = (UINT)fread(m_xmf3Position, sizeof(float), 3, pInFile);
					nReads = (UINT)fread(m_xmf3Rotation.x, sizeof(float), 1, pInFile);
					nReads = (UINT)fread(m_xmf3Rotation.y, sizeof(float), 1, pInFile);
					nReads = (UINT)fread(m_xmf3Rotation.z, sizeof(float), 1, pInFile);
					nReads = (UINT)fread(m_xmf3Scale, sizeof(float), 3, pInFile);
					nReads = (UINT)fread(m_xmf4Orientation, sizeof(float), 4, pInFile);*/
				}
				else if (!strcmp(pstrToken, "</Frame>"))
				{
					break;
				}
			}
		}
	}
}


std::shared_ptr<Object> Scene::CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, XMFLOAT3 xmf3Rotation, const char* pstrFileName, int nAnimationTracks)
{
	ObjectInitData objectData;
	objectData.xmf3Position = xmf3Position;
	objectData.xmf3Rotation = xmf3Rotation;
	objectData.xmf4Orientation = xmf4Orientation;
	objectData.xmf3Scale = g_DefaultObjectData[pstrFileName].xmf3OffsetScale;
	objectData.nMass = g_DefaultObjectData[pstrFileName].nMass;
	objectData.objectType = g_DefaultObjectData[pstrFileName].objectType;
	objectData.colliderType = g_DefaultObjectData[pstrFileName].colliderType;
	objectData.xmf3Extents = g_DefaultObjectData[pstrFileName].xmf3Extents;
	objectData.xmf3ColliderOffsetPosition = g_DefaultObjectData[pstrFileName].xmf3ColliderOffsetPosition;
	objectData.xmf3ColliderOffsetRotation = g_DefaultObjectData[pstrFileName].xmf3ColliderOffsetRotation;
	
	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> pObject;

	if (g_LoadedModelData.find(pstrFileName) == g_LoadedModelData.end() && pstrFileName != nullptr)
	{
		// �� �ε�
		pModelData = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, pstrFileName, g_DefaultObjectData[pstrFileName].pstrTexPath);

		g_LoadedModelData.insert({ pstrFileName, pModelData });
	}
	else	// �̹� �ε��� ���� ���
	{
		pModelData = g_LoadedModelData[pstrFileName];
	}

	switch (objectData.objectType)
	{
	case Object_Player:
	{
		std::shared_ptr<Player> pPlayer = std::make_shared<Player>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pPlayer);
	}
		break;

	case Object_Monster:
	{
		std::shared_ptr<Monster> pMonster = std::make_shared<Monster>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pMonster);
	}
		break;


	case Object_Weapon:
	{
		char tmp[64] = "mixamorig:RightHand";
		std::shared_ptr<Weapon> pWeapon = std::make_shared<Weapon>(pd3dDevice, pd3dCommandList, objectData, tmp, m_vpAllObjs[0], pModelData, nAnimationTracks, nullptr);
		pObject = std::static_pointer_cast<Object>(pWeapon);
		std::static_pointer_cast<Player>(m_vpAllObjs[0])->SetWeapon(pWeapon);
	}
	break;	

	default:
	{
		pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, objectData, pModelData, nAnimationTracks, nullptr);
	}
		break;
	}

	m_vpAllObjs.emplace_back(pObject);
	m_vObjectLayer[g_DefaultObjectData[pstrFileName].renderLayer].emplace_back(pObject);
	m_vObjectLayer[g_DefaultObjectData[pstrFileName].renderLayer][0]->GetCollider();
//	pObject->Initialize(pd3dDevice, pd3dCommandList, NULL);

	return pObject;
}

void Scene::GenerateContact()
{
	unsigned int nContactCnt = MAX_CONTACT_CNT * 8;

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if(m_vpAllObjs[i]->GetBody())
			m_vpAllObjs[i]->GetBody()->ClearContact();
	}

	m_CollisionData.Reset(nContactCnt);
	m_CollisionData.friction = 0;
	m_CollisionData.restitution = 0.1f;
	m_CollisionData.tolerance = 0.1f;

	// �÷��̾� �˻�
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
	// ������ �˻�
	for (int i = 0; i < g_ppColliderPlanes.size(); ++i)
	{
		for (int j = 1; j < g_ppColliderBoxs.size(); ++j)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;
			CollisionDetector::BoxAndHalfSpace(*g_ppColliderBoxs[j], *g_ppColliderPlanes[i], m_CollisionData);
		}
	}
	// �ڽ����� �˻�
	for (int i = 1; i < g_ppColliderBoxs.size(); ++i)
	{
		for (int j = i + 1; j < g_ppColliderBoxs.size(); ++j)
		{
			if (m_CollisionData.ContactCnt() > nContactCnt) return;

			CollisionDetector::BoxAndBox(*g_ppColliderBoxs[i], *g_ppColliderBoxs[j], m_CollisionData);
		}
	}
}

void Scene::ProcessPhysics(float elapsedTime)
{

	m_pCollisionResolver->ResolveContacts(m_CollisionData.pContacts, elapsedTime);

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

	// ��ü ��ȸ
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (!m_vpAllObjs[i]->GetIsAlive())
		{
			m_vpAllObjs[i]->Destroy();
			m_vpAllObjs.erase(m_vpAllObjs.begin() + i);
		}
	}

	// ���̾� ��ȸ
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
