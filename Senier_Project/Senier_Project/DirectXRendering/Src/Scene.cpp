#include "../Header/Scene.h"

#define TEST_MODEL_NAME "Model/Cube.bin"

Scene::Scene()
{
	m_CollisionData.Reset(MAX_CONTACT_CNT);
	m_pCollisionResolver = std::make_unique<CollisionResolver>(256);
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// �н� ���� ����
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	// static Shader �ʱ�ȭ

	//for(int i = 0 ; i < 5; ++i)
	CreateObject(pd3dDevice, pd3dCommandList, TEST_MODEL_NAME, 0, RenderLayer::Static);
	//CreateObject(pd3dDevice, pd3dCommandList, TEST_MODEL_NAME, 0, RenderLayer::Static);
	
	// object Collider ȹ��
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		m_ppColliders.push_back(m_vpAllObjs[i]->GetCollider());
	}

	//m_vpAllObjs[1]->SetPosition(5, -20, 0);
	//m_vpAllObjs[1]->SetScale(10, 1, 10);

	//m_vpAllObjs[0]->SetRotate(-10, 0, 20);
	m_vpAllObjs[0]->SetPosition(0, 100, 0);

	//CreateObject(pd3dDevice, pd3dCommandList, TEST_MODEL_NAME, 0, RenderLayer::Static);
	//CreateObject(pd3dDevice, pd3dCommandList, TEST_MODEL_NAME, 0, RenderLayer::Static);
	//CreateObject(pd3dDevice, pd3dCommandList, TEST_MODEL_NAME, 0, RenderLayer::Static);
	//m_vpAllObjs[2]->SetPosition(0, 35, 0);
	//m_vpAllObjs[3]->SetPosition(0, 50, 0);
	//m_vpAllObjs[4]->SetPosition(0, 65, 0);
	//m_vpAllObjs[2]->SetPhysics(true);
	//m_vpAllObjs[3]->SetPhysics(true);
	//m_vpAllObjs[4]->SetPhysics(true);


	
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->UpdateTransform(NULL);
	}

	// ī�޶� �ʱ�ȭ
	//m_pCamera = std::make_unique<Third_Person_Camera>(m_vpAllObjs[0]);
	m_pCamera = std::make_unique<Camera>();
	m_pCamera->SetPosition(0, 0, -100);
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
	m_refCnt = m_LoadedModelData[TEST_MODEL_NAME]->m_pRootObject.use_count();
	m_size = m_vpAllObjs.size();
#endif

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i]) m_vpAllObjs[i]->Update(elapsedTime);
	}

	// Contact ������ ����
	GenerateContact();
	// Collision Resolve
	m_pCollisionResolver->ResolveContacts(m_CollisionData.pContacts, elapsedTime);


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
		g_Shaders[RenderLayer::Image]->ChangeShader(pd3dCommandList);
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);
	}

	g_Shaders[RenderLayer::Static]->ChangeShader(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (m_vpAllObjs[i])
		{
			// Render �Լ� ������ Bone ����� ���̴��� ���޵Ǳ� ������ Render ������ �ִϸ��̼��� �������ش�.
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
	const char* pstrFileName, int nAnimationTracks, RenderLayer renderLayer)
{
	std::shared_ptr<ModelDataInfo> pModelData;
	std::shared_ptr<Object> pObject;

	if (m_LoadedModelData.find(pstrFileName) == m_LoadedModelData.end())
	{
		// �� �ε�
		pModelData = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, pstrFileName);

		m_LoadedModelData.insert({ pstrFileName, pModelData });
	}
	else	// �̹� �ε��� ���� ���
	{
		pModelData = m_LoadedModelData[pstrFileName];
	}

	// ������Ʈ ����
	pObject = std::make_shared<Object>(pd3dDevice, pd3dCommandList, pModelData, nAnimationTracks);

#if defined(_DEBUG)
	pObject->SetScale(10, 10, 10);
#endif

	m_vpAllObjs.emplace_back(pObject);
	m_vObjectLayer[renderLayer].emplace_back(pObject);


	return pObject;
}

void Scene::GenerateContact()
{
	ColliderPlane plane(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0), 0);

	m_CollisionData.Reset(MAX_CONTACT_CNT);
	m_CollisionData.friction = 0.9f;
	m_CollisionData.restitution = 0.1f;
	m_CollisionData.tolerance = 0.1f;

	// ������ ������ �˻�
	for (int i = 0; i < m_ppColliders.size(); ++i)
	{
		if (m_CollisionData.ContactCnt() > MAX_CONTACT_CNT) return;
		CollisionDetector::BoxAndHalfSpace(*(m_ppColliders[i]), plane, m_CollisionData);

	}


}

void Scene::ClearObjectLayer()
{
	// ��ü ��ȸ
	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		if (!m_vpAllObjs[i]->GetIsAlive())
		{
			m_vpAllObjs.erase(m_vpAllObjs.begin() + i);
		}
	}

	// ���̾� ��ȸ
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