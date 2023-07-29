#include "../Header/Shader.h"
#include "../Header/Scene.h"
#include "../Header/Object.h"

Object::Object()
{
}

Object::Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
				ObjectInitData objData,
				std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
	m_nObjectCBParameterIdx = 3;

	m_DestroyTime = 0.0;
	m_DissolveTime = 0.0f;
}

Object::~Object()
{
	Destroy();
}

bool Object::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
						ObjectInitData objData,
						std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	if (pModelData)
	{
		SetChild(pModelData->m_pRootObject);
		if (nAnimationTracks > 0)
			m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pModelData);
	}
	std::shared_ptr<Collider> pCollider;
	std::shared_ptr<RigidBody> pBody;

	m_nObjectType = objData.objectType;
	// �浹ü Ÿ�Կ� ���� 
	m_nColliderType = objData.colliderType;
	m_nSoundType = objData.soundType;
	switch (objData.colliderType)
	{
	case Collider_Plane:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation,objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		std::shared_ptr<ColliderPlane> pColliderPlane;
		XMFLOAT3 direction = XMFLOAT3(objData.xmf4Orientation.x, objData.xmf4Orientation.y, objData.xmf4Orientation.z);
		pColliderPlane = std::make_shared<ColliderPlane>(pBody, direction, objData.xmf3Extents.x);
		g_ppColliderPlanes.emplace_back(pColliderPlane);
		pCollider = std::static_pointer_cast<Collider>(pColliderPlane);
	}
	break;

	case Collider_Box:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation,objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		std::shared_ptr<ColliderBox> pColliderBox;
		pColliderBox = std::make_shared<ColliderBox>(pBody, objData.xmf3Extents);
		g_ppColliderBoxs.emplace_back(pColliderBox);
		pCollider = std::static_pointer_cast<Collider>(pColliderBox);
	}
	break;

	case Collider_Sphere:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation,objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		std::shared_ptr<ColliderSphere> pColliderSphere;
		pColliderSphere = std::make_shared<ColliderSphere>(pBody, objData.xmf3Extents.x);
		g_ppColliderSpheres.emplace_back(pColliderSphere);
		pCollider = std::static_pointer_cast<Collider>(pColliderSphere);
	}
	break;

	case Collider_None:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation, objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		pCollider = nullptr;

		m_xmf3Position = objData.xmf3Position;
		m_xmf4Orientation = objData.xmf4Orientation;
		m_xmf3Rotation = objData.xmf3Rotation;
		m_xmf3Scale = objData.xmf3Scale;
		m_Mass = objData.nMass;
	}

	default:
		break;
	}

	m_pBody = pBody;
	m_pCollider = pCollider;
	m_xmf3ColliderExtents = objData.xmf3Extents;
	m_xmf3RenderOffsetPosition = objData.xmf3MeshOffsetPosition;
	m_xmf3RenderOffsetRotation = objData.xmf3MeshOffsetRotation;
	m_bShadow = objData.bShadow;

#if defined(_DEBUG) && defined(COLLIDER_RENDER)
	if (m_pCollider) m_pCollider->BuildMesh(pd3dDevice, pd3dCommandList);
#endif

	BuildConstantBuffers(pd3dDevice);
	UpdateToRigidBody(0.0f);
	UpdateTransform(nullptr);

	return true;
}

void Object::Animate(float elapsedTime)
{
	if (m_pAnimationController)
		m_pAnimationController->AdvanceTime(elapsedTime, this);

	if (m_pSibling) { m_pSibling->Animate(elapsedTime); }
	if (m_pChild) {	m_pChild->Animate(elapsedTime); }
}

void Object::Update(float elapsedTime)
{
	if (!m_bIsAlive)
		return;

	// ������Ʈ �ı� Ÿ�̸�
	if (m_bDestroying)
	{
		if (m_bDissolveStart)
		{
			m_ElapsedDestroyTime += elapsedTime;
			m_DissolveValue = m_ElapsedDestroyTime / (m_DestroyTime);
			if (m_ElapsedDestroyTime >= m_DestroyTime)
			{
				m_bIsAlive = false;
				return;
			}
		}
		else
		{
			m_ElapsedDissolveTime += elapsedTime;
			if (m_ElapsedDissolveTime >= m_DissolveTime)
			{
				m_bDissolveStart = true;
			}
		}
	}

	// ���� �ð� ��� ����
	if (m_bInvincible)
	{
		m_ElapsedInvincibleTime += elapsedTime;
		if (m_InvincibleTime <= m_ElapsedInvincibleTime)
		{
			m_ElapsedInvincibleTime = 0.0f;
			m_bInvincible = false;
		}
	}

	//if(m_nObjectType != ObjectType::Object_World)
		UpdateToRigidBody(elapsedTime);


	if (m_bActiveTimer)
	{
		m_ElapsedActiveTime += elapsedTime;
		if (m_ElapsedActiveTime > m_ActiveTime)
		{
			m_bActiveTimer = false;
			m_pCollider->SetIsActive(true);
			m_ElapsedActiveTime = 0.0f;
		}
	}
	
	if (m_pObjectCB) UpdateObjectCB();

	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}
}

void Object::UpdateToRigidBody(float elapsedTime)
{
	// RigidBody�� �������� ��ġ�� �����Ѵ�.
	if (m_pBody)
	{
		m_pBody->Update(elapsedTime);
		if (m_pBody->GetInvalid())
		{
			Destroy();
			return;
		}
		if (m_pCollider)
			m_pCollider->UpdateWorldTransform();
		m_xmf3Position = m_pBody->GetPosition();
		SetOrientation(m_pBody->GetOrientation());
		SetObjectRotation(m_pBody->GetRotate());
		m_xmf3Scale = m_pBody->GetScale();
	}
	else
	{
		if (m_pCollider)
		{
			m_pCollider->SetWorld(m_xmf4x4World);
			m_pCollider->UpdateWorldTransform();
		}
	}
}

void Object::UpdateObjectCB()
{
	ObjConstant objConstant;
	objConstant.DissolveValue = m_DissolveValue;
	if (m_pObjectCB) m_pObjectCB->CopyData(0, objConstant);
}

void Object::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	// Animate �Ŀ� ȣ��Ǿ� Bone ����� ����
	// 
	if (pxmf4x4Parent)
	{
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4LocalTransform), XMLoadFloat4x4(pxmf4x4Parent)));
	}
	else
	{
		// RootObject�� ���
		m_xmf4x4LocalTransform = MathHelper::identity4x4();
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX xmmatScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
		XMMATRIX xmmatOrientation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
		//XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotation.x), XMConvertToRadians(m_xmf3Rotation.y), XMConvertToRadians(m_xmf3Rotation.z));
		XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		// S * R * T
		//xmmatRotate = XMMatrixMultiply(xmmatRotate, xmmatOrientation);
		world = XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatOrientation, xmmatTranslate));
		//world = XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatRotate, xmmatTranslate));

		XMMATRIX offset = XMMatrixTranslation(-m_xmf3RenderOffsetPosition.x, -m_xmf3RenderOffsetPosition.y, -m_xmf3RenderOffsetPosition.z);
		XMMATRIX offsetRotate = XMMatrixRotationRollPitchYaw(m_xmf3RenderOffsetRotation.x, m_xmf3RenderOffsetRotation.y, m_xmf3RenderOffsetRotation.z);
		world = XMMatrixMultiply(XMMatrixMultiply(offsetRotate, offset), world);
		XMStoreFloat4x4(&m_xmf4x4LocalTransform, world);

		m_xmf4x4World = m_xmf4x4LocalTransform;

		m_xmf3RenderPosition = XMFLOAT3(0, 0, 0);
		XMVECTOR renderPosition = XMLoadFloat3(&m_xmf3RenderPosition);
		renderPosition = XMVector3TransformCoord(renderPosition, world);
		XMStoreFloat3(&m_xmf3RenderPosition, renderPosition);
	}

	if (m_pCollider) m_pCollider->UpdateWorldTransform();

	if (m_pSibling) {
		m_pSibling->UpdateTransform(pxmf4x4Parent);
	}
	if (m_pChild) {
		m_pChild->UpdateTransform(&m_xmf4x4World);
	}
}

void Object::OnPrepareRender(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ���̴��� ���޵� Bone ��� ���۸� �����Ѵ�.
	if (m_pAnimationController) m_pAnimationController->ChangeBoneTransformCB(pd3dCommandList);
	if (m_pObjectCB) pd3dCommandList->SetGraphicsRootConstantBufferView(m_nObjectCBParameterIdx, m_pObjectCB->Resource()->GetGPUVirtualAddress());
}

void Object::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_bVisible)
		return;

	OnPrepareRender(elapsedTime, pd3dCommandList);

	if (m_pMesh)
	{
		XMFLOAT4X4 xmf4x4World;
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(m_nObjectConstantsParameterIdx, 16, &xmf4x4World, 0);
		XMFLOAT4X4 xmf4x4InverseTransWorld;
		XMStoreFloat4x4(&xmf4x4InverseTransWorld, XMMatrixTranspose(XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_xmf4x4World)))));
		pd3dCommandList->SetGraphicsRoot32BitConstants(m_nObjectConstantsParameterIdx, 16, &xmf4x4InverseTransWorld, 16);

		for (int i = 0; i < m_vpMaterials.size(); ++i)
		{
			m_vpMaterials[i]->MaterialSet(pd3dCommandList);

			m_pMesh->Render(elapsedTime, pd3dCommandList, i);
		}
	}

#if defined(_DEBUG) && defined(COLLIDER_RENDER)
	if (m_pCollider)
	{
		g_Shaders[ShaderType::Shader_WireFrame]->ChangeShader(pd3dCommandList);
		m_pCollider->Render(elapsedTime, pd3dCommandList);
	}
#endif

	if (m_pSibling) m_pSibling->Render(elapsedTime, pd3dCommandList);
	if (m_pChild) m_pChild->Render(elapsedTime, pd3dCommandList);
}

void Object::DepthRender(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//if (!m_bVisible)
	//	return;

	OnPrepareRender(elapsedTime, pd3dCommandList);

	if (m_pMesh)
	{
		XMFLOAT4X4 xmf4x4World;
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(m_nObjectConstantsParameterIdx, 16, &xmf4x4World, 0);
		XMFLOAT4X4 xmf4x4InverseTransWorld;
		XMStoreFloat4x4(&xmf4x4InverseTransWorld, XMMatrixTranspose(XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_xmf4x4World)))));
		pd3dCommandList->SetGraphicsRoot32BitConstants(m_nObjectConstantsParameterIdx, 16, &xmf4x4InverseTransWorld, 16);

		for (int i = 0; i < m_vpMaterials.size(); ++i)
		{
			m_pMesh->Render(elapsedTime, pd3dCommandList, i);
		}
	}

	if (m_pSibling) m_pSibling->DepthRender(elapsedTime, pd3dCommandList);
	if (m_pChild) m_pChild->DepthRender(elapsedTime, pd3dCommandList);
}

void Object::BuildConstantBuffers(ID3D12Device* pd3dDevice)
{
	m_pObjectCB = std::make_unique<UploadBuffer<ObjConstant>>(pd3dDevice, 1, true);
	m_ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjConstant));
}

void Object::BuildTextureDescriptorHeap(ID3D12Device* pd3dDevice)
{
	for (int i = 0; i < m_vpMaterials.size(); ++i)
	{
		m_vpMaterials[i]->BuildDescriptorHeap(pd3dDevice);
	}

	if (m_pSibling) m_pSibling->BuildTextureDescriptorHeap(pd3dDevice);
	if (m_pChild) m_pChild->BuildTextureDescriptorHeap(pd3dDevice);
}

void Object::CreateScoreItems(int nCnt)
{
	int nRand;

	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	xmf3Velocity.y = 0;

	for (int i = 0; i < nCnt; ++i)
	{
		nRand = rand() % 5;

		switch (nRand)
		{
		case 0:
		{
			std::shared_ptr<Object> pObject = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList, m_xmf3Position,
			XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), SCOREITEM1_MODEL_NAME, 0);

			Item* pItem = (Item*)pObject.get();
			pItem->SetItemType(Item::ItemType::Score_1);
			pItem->SetCrushVelocity(xmf3Velocity);
		}
			break;
		case 1:
		{
			std::shared_ptr<Object> pObject = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList, m_xmf3Position,
				XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), SCOREITEM2_MODEL_NAME, 0);

			Item* pItem = (Item*)pObject.get();
			pItem->SetItemType(Item::ItemType::Score_2);
			pItem->SetCrushVelocity(xmf3Velocity);
		}
		break;
		case 2:
		{
			std::shared_ptr<Object> pObject = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList, m_xmf3Position,
				XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), SCOREITEM3_MODEL_NAME, 0);

			Item* pItem = (Item*)pObject.get();
			pItem->SetItemType(Item::ItemType::Score_3);
			pItem->SetCrushVelocity(xmf3Velocity);
		}
		break;
		case 3:
		{
			std::shared_ptr<Object> pObject = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList, m_xmf3Position,
				XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), SCOREITEM4_MODEL_NAME, 0);

			Item* pItem = (Item*)pObject.get();
			pItem->SetItemType(Item::ItemType::Score_4);
			pItem->SetCrushVelocity(xmf3Velocity);
		}
		break;
		case 4:
		{
			std::shared_ptr<Object> pObject = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList, m_xmf3Position,
				XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), SCOREITEM5_MODEL_NAME, 0);

			Item* pItem = (Item*)pObject.get();
			pItem->SetItemType(Item::ItemType::Score_5);
			pItem->SetCrushVelocity(xmf3Velocity);
		}
		break;
		}
	}

	// ���� ȿ����
	//Scene::EmitSound("Sound/Item/SpawnScore.wav", false, 0.75f, 0.1f);
}

void Object::CreateHealItems(int nCnt)
{
	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	xmf3Velocity.y = 0;

	for (int i = 0; i < nCnt; ++i)
	{
		std::shared_ptr<Object> pObject = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList, m_xmf3Position,
			XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), HEALITEM_MODEL_NAME, 0);

		Item* pItem = (Item*)pObject.get();
		pItem->SetItemType(Item::ItemType::Heal_1);
		pItem->SetCrushVelocity(xmf3Velocity);
	}

	// ���� ȿ����
	Scene::EmitSound("Sound/Item/SpawnHeal.wav", false, 1.0f, 0.1f);
}

std::shared_ptr<ModelDataInfo> Object::LoadModelDataFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	const char* pstrFileName, const char* pstrObjectPath, const char* pstrTexPath)
{
	FILE* pInFile = NULL;

	char pstrFilePath[128] = { '\0' };
	strcpy_s(pstrFilePath, 64, "Model/");
	if (strlen(pstrObjectPath))
	{
		strcat_s(pstrFilePath, pstrObjectPath);
		strcat_s(pstrFilePath, "/");
	}
	strcat_s(pstrFilePath, pstrFileName);
	strcat_s(pstrFilePath, ".bin");
	::fopen_s(&pInFile, pstrFilePath, "rb");
	::rewind(pInFile);

	std::shared_ptr<ModelDataInfo> pModelData = std::make_shared<ModelDataInfo>();
	pModelData->m_pRootObject = std::make_shared<Object>();

	char pstrToken[64] = { '\0' };
	int nSkinnedMeshes = 0;

	for (; ; )
	{
		if (ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>:"))
			{
				pModelData->m_pRootObject = Object::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile, &nSkinnedMeshes, NULL, pstrFileName, pstrTexPath);
				pModelData->m_nSkinnedMeshes = nSkinnedMeshes;

				pModelData->m_pRootObject->BuildTextureDescriptorHeap(pd3dDevice);
				::ReadStringFromFile(pInFile, pstrToken); //"</Hierarchy>"
			}
			else if (!strcmp(pstrToken, "<Animation>:"))
			{
				Object::LoadAnimationFromFile(pInFile, pModelData);

				pModelData->PrepareSkinning();
			}
			else if (!strcmp(pstrToken, "</Animation>:"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	

	return pModelData;
}

std::shared_ptr<Object> Object::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	FILE* pInFile, int* pnSkinnedMeshes, Object* pRootObject, const char* pstrFileName, const char* pstrTexPath)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	std::shared_ptr<Object> pObject = std::make_shared<Object>();
	//pObject->Initialize(pd3dDevice, pd3dCommandList, NULL);

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ReadintegerFromFile(pInFile);
			nTextures = ReadintegerFromFile(pInFile);

			char tmpstr[64] = { '\0' };
			ReadStringFromFile(pInFile, tmpstr);
			
			strcpy_s(pObject->m_FrameName, tmpstr);
			pObject->SetName(tmpstr);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{ 
			nReads = (UINT)fread(&pObject->m_xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf3Rotation.x, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf3Rotation.y, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf3Rotation.z, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf4Orientation, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)fread(&pObject->m_xmf4x4LocalTransform, sizeof(XMFLOAT4X4), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			std::shared_ptr<SkinnedMesh> pSkinMesh = std::make_shared<SkinnedMesh>();
			pSkinMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);
			
			ReadStringFromFile(pInFile, pstrToken); // <Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pObject->SetMesh(pSkinMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			if(pRootObject)
				pObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
			else
				pObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pInFile, pObject.get(), pstrFileName, pstrTexPath);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChild = ReadintegerFromFile(pInFile);

			if (nChild > 0)
			{
				for (int i = 0; i < nChild; ++i)
				{
					if (pRootObject)
					{
						std::shared_ptr<Object> pChild = LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile, pnSkinnedMeshes, pRootObject, pstrFileName, pstrTexPath);
						if (pChild) pObject->SetChild(pChild);
					}
					else
					{
						std::shared_ptr<Object> pChild = LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile, pnSkinnedMeshes, pObject.get(), pstrFileName, pstrTexPath);
						if (pChild) pObject->SetChild(pChild);
					}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}

		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}

	return pObject;
}

void Object::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	FILE* pInFile, Object* pRootObject, const char* pstrFileName, const char* pstrTexPath)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	int nMatcnt = 0;

	UINT nReads;
	nMaterial = ReadintegerFromFile(pInFile);

	std::vector<std::shared_ptr<Material>> vpMat;

	for (int i = 0; i < nMaterial; ++i)
	{
		vpMat.emplace_back(std::make_shared<Material>());
	}
	
	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMatcnt = ReadintegerFromFile(pInFile);

			int nType = m_pMesh->GetType();
			if (nType & VERTEXT_NORMAL_TANGENT_TEXTURE)
			{
				if (nType & VERTEXT_BONE_INDEX_WEIGHT)
				{
					vpMat[nMatcnt]->SetShader(g_Shaders[ShaderType::Shader_Skinned]);
				}
				else
				{
					vpMat[nMatcnt]->SetShader(g_Shaders[ShaderType::Shader_TextureMesh]);
				}
			}
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			vpMat[nMatcnt]->SetAlbedoColor(tmp);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			vpMat[nMatcnt]->SetEmissiveColor(tmp);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			vpMat[nMatcnt]->SetSpecularColor(tmp);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			vpMat[nMatcnt]->SetGlossiness(tmp);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			vpMat[nMatcnt]->SetSmoothness(tmp);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			vpMat[nMatcnt]->SetMetallic(tmp);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			vpMat[nMatcnt]->SetMetallic(tmp);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			vpMat[nMatcnt]->SetGlossyReflection(tmp);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			bool ret = vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
			if (!ret) vpMat[nMatcnt]->SetIsNonTextureMat(true);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject, pstrFileName, pstrTexPath);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}

	SetMaterials(vpMat);
}

void Object::Cutting(XMFLOAT3 xmf3PlaneNormal)
{
	XMFLOAT3 xmf3CutNormal = xmf3PlaneNormal;
	XMVECTOR cutNormal = XMLoadFloat3(&xmf3CutNormal);
	XMMATRIX xmmatOrientation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
	cutNormal = XMVector3TransformNormal(cutNormal, xmmatOrientation);
	XMStoreFloat3(&xmf3CutNormal, cutNormal);

	m_bIsAlive = false;
	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList, this, 1, xmf3CutNormal, false);
	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList, this, -1, xmf3CutNormal, false);

	// ���� �������� �Ҹ�
	Scene::EmitCutSound(m_nSoundType, false);
}

void Object::DegradedBroken()
{
	// �ִ� 3���� ���� ����� ���� �� ����
	// �������� 3���� ����
	
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0); 

	float Angle_X = rand() % 60 - 30;
	float Angle_Y = rand() % 30 - 15;
	float Angle_Z = rand() % 60 - 30;

	XMMATRIX rotateMat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Angle_X), XMConvertToRadians(Angle_Y), XMConvertToRadians(Angle_Z));

	// ������ ��� ����
	XMFLOAT3 xmf3PlaneNormal[3];
	XMVECTOR planeNormal[3];	// ���� �и�
	XMFLOAT3 xmf3RandVector3 = { 0,0,0 };
	for (int i = 0; i < 3; ++i)
	{
		xmf3RandVector3.x = rand() % 100;
		xmf3RandVector3.y = rand() % 100;
		xmf3RandVector3.z = rand() % 100;
		planeNormal[i] = XMLoadFloat3(&xmf3RandVector3);
		planeNormal[i] = XMVector3TransformNormal(planeNormal[i], rotateMat);
		planeNormal[i] = XMVector3Normalize(planeNormal[i]);
		XMStoreFloat3(&xmf3PlaneNormal[i], planeNormal[i]);
	}

	std::shared_ptr<Object> pCuttedPositive_1 = Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		this, 1, xmf3PlaneNormal[0], false);
	std::shared_ptr<Object> pCuttedPositive_2 = Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		pCuttedPositive_1.get(), 1, xmf3PlaneNormal[1], true);
	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		pCuttedPositive_1.get(), 1, xmf3PlaneNormal[2], true);
	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		pCuttedPositive_2.get(), 1, xmf3PlaneNormal[2], true);

	std::shared_ptr<Object> pCuttedNegative_1 = Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		this, -1, xmf3PlaneNormal[0], false);
	std::shared_ptr<Object> pCuttedNegative_2 = Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		pCuttedNegative_1.get(), 1, xmf3PlaneNormal[1], true);
	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		pCuttedNegative_1.get(), 1, xmf3PlaneNormal[2], true);
	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList,
		pCuttedNegative_2.get(), 1, xmf3PlaneNormal[2], true);

	m_bIsAlive = false;
	
	// �ν����� �Ҹ�
	Scene::EmitBrokenSound(m_nSoundType, false);

}

void Object::Destroy()
{
	m_bIsAlive = false;

	if(m_pMesh)
		m_pMesh.reset();
	for (int i = 0; i < m_vpMaterials.size(); ++i)
		m_vpMaterials[i].reset();

	if (m_pAnimationController)
	{
		m_pAnimationController.reset();
	}
	if(m_pObjectCB) m_pObjectCB.reset();

	if(m_pCollider) m_pCollider.reset();
	if (m_pBody)
	{
		m_pBody->Destroy();
		m_pBody.reset();
	}

	if (m_pChild)
	{
		m_pChild->Destroy();
	}
	if (m_pSibling)
	{
		m_pSibling->Destroy();
	}
}

void Object::DestroyRunTime()
{
	m_bIsAlive = false;

	if (m_pMesh)
		m_pMesh.reset();
	for (int i = 0; i < m_vpMaterials.size(); ++i)
		m_vpMaterials[i].reset();

	if (m_pAnimationController)
	{
		m_pAnimationController.reset();
	}
	//if (m_pObjectCB) m_pObjectCB.reset();

	if (m_pCollider) m_pCollider.reset();
	if (m_pBody)
	{
		m_pBody->Destroy();
		m_pBody.reset();
	}

	if (m_pChild)
	{
		m_pChild.reset();
	}
	if (m_pSibling)
	{
		m_pSibling.reset();
	}
}

void Object::LoadAnimationFromFile(FILE* pInFile, std::shared_ptr<ModelDataInfo> pModelData)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;
	int nAnimationSetCnt = -1;

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ReadintegerFromFile(pInFile);

			pModelData->m_pAnimationSets = std::make_shared<AnimationSets>();
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			nAnimationSetCnt++;

			int nBoneFrames = ReadintegerFromFile(pInFile);
			pModelData->m_pAnimationSets->m_nAnimatedBoneFrames = nBoneFrames;

			for (int i = 0; i < nBoneFrames; ++i)
			{
				ReadStringFromFile(pInFile, pstrToken);

				pModelData->m_pAnimationSets->m_vpAnimatedBoneFrameCaches.emplace_back(pModelData->m_pRootObject->FindFrame(pstrToken));
			}

		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			std::shared_ptr<AnimationSets> pAnimationSets = pModelData->m_pAnimationSets;

			// n��° �ִϸ��̼� ������
			int nAnimationSet = ReadintegerFromFile(pInFile);
			
			// �ִϸ��̼� ��Ī
			ReadStringFromFile(pInFile, pstrToken);

			float Length = ReadFloatFromFile(pInFile);				// �ִϸ��̼� ����
			int nFramesPerSecond = ReadintegerFromFile(pInFile);	// �ʴ� ������
			int nKeyFrames = ReadintegerFromFile(pInFile);			// �� Ű������ ��
			
			// AnimationSet ����
			std::shared_ptr<AnimationSet> tmpAnimationSet = std::make_shared<AnimationSet>();
			tmpAnimationSet->m_Length = Length;
			tmpAnimationSet->m_nFramesPerSecond = nFramesPerSecond;
			tmpAnimationSet->m_nKeyFrames = nKeyFrames;
			strcpy_s(tmpAnimationSet->m_strAnimationName, pstrToken);
			tmpAnimationSet->m_vKeyFrameTimes.resize(nKeyFrames);
			tmpAnimationSet->m_vvxmf4x4KeyFrameTransforms.resize(nKeyFrames);
			for (int i = 0; i < nKeyFrames; ++i)
				tmpAnimationSet->m_vvxmf4x4KeyFrameTransforms[i].resize(pAnimationSets->m_nAnimatedBoneFrames);

			pAnimationSets->m_vpAnimationSets.emplace_back(tmpAnimationSet);


			// n��° �ִϸ��̼� Set ����
			std::shared_ptr<AnimationSet> pAnimationSet = pAnimationSets->m_vpAnimationSets[nAnimationSet];

			// KeyFrame ����ŭ ��ȸ�ϸ� ��ȯ��� ����
			for (int i = 0; i < nKeyFrames; i++)
			{
				::ReadStringFromFile(pInFile, pstrToken);
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKey = ReadintegerFromFile(pInFile);		// n��° Ű	== i
					float KeyTime = ReadFloatFromFile(pInFile);		// n��° Ű �ð�

#ifdef _WITH_ANIMATION_SRT
					m_pfKeyFrameScaleTimes[i] = fKeyTime;
					m_pfKeyFrameRotationTimes[i] = fKeyTime;
					m_pfKeyFrameTranslationTimes[i] = fKeyTime;
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameScales[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4KeyFrameRotations[i], sizeof(XMFLOAT4), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameTranslations[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
#else
					// n��° Ű������ �ð� ����
					pAnimationSet->m_vKeyFrameTimes[i] = KeyTime;
					// n��° Ű������ ��ȯ ��� ���� (Bone ������ŭ)
					nReads = (UINT)::fread(&(pAnimationSet->m_vvxmf4x4KeyFrameTransforms[nKey][0]), sizeof(XMFLOAT4X4), pAnimationSets->m_nAnimatedBoneFrames, pInFile);
#endif
				}
			}

		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			pModelData->m_pAnimationSets->m_nAnimationSets = nAnimationSets;
			break;
		}
	}
}

void Object::SetChild(std::shared_ptr<Object> pChild)
{
	if (m_pChild != NULL)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void Object::SetMesh(std::shared_ptr<Mesh> pMesh)
{
	m_pMesh = pMesh;
}

void Object::SetMaterials(std::vector<std::shared_ptr<Material>> vpMaterial)
{
	m_vpMaterials = vpMaterial;
}


std::shared_ptr<Object> Object::FindFrame(char* pstrFrameName)
{
	std::shared_ptr<Object> pObject = NULL;
	if (!strcmp(GetName(), pstrFrameName)) return shared_from_this();

	if (m_pSibling) if (pObject = m_pSibling->FindFrame(pstrFrameName)) return pObject;
	if (m_pChild) if (pObject = m_pChild->FindFrame(pstrFrameName)) return pObject;

	return NULL;
}

void Object::FindAndSetSkinnedMesh(std::vector<std::shared_ptr<SkinnedMesh>>* ppSkinnedMeshes)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) 
		ppSkinnedMeshes->push_back(std::static_pointer_cast<SkinnedMesh>(m_pMesh));

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes);
}

void Object::IsFalling()
{
	if (m_xmf3RenderPosition.y > 0)
	{
		m_bIsFalling = true;
	}
	else if(m_xmf3RenderPosition.y < 0)
	{
		m_xmf3RenderPosition.y = 0;
		m_bIsFalling = false;
	}
}

void Object::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection)
{
	if (m_nObjectType == ObjectType::Object_World)
		return;

	if (m_bInvincible || m_bDestroying)
		return;
	// �ǰ� ����
	m_bInvincible = true;

	// ü�� ����
	m_HP -= power;

	if (m_HP <= 0)
	{
		m_bDestroying = true;
		// ������ �ı� ���� ���
		//Sound::PlaySoundFile(m_Death_SoundFileName, true);

		// �ı��� ��ġ�� ������ ����
		int tmp = rand() % 10;
		if(tmp > 2)
			CreateScoreItems(m_MaxHP / 10);
		else
			CreateHealItems(1);

		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&xmf3CuttingDirection))))
			Cutting(xmf3CuttingDirection);
		else
			DegradedBroken();

		return;
	}

	// �Ϲ����� �����
	Scene::EmitHitSound(m_nSoundType, false);


	XMVECTOR damageDirection = XMLoadFloat3(&xmf3DamageDirection);
	damageDirection = XMVector3Normalize(damageDirection);
	XMVECTOR deltaVelocity = damageDirection * power * 5;

	XMFLOAT3 xmf3DeltaVelocity;
	XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);

	m_pBody->AddVelocity(xmf3DeltaVelocity);
}

void Object::SetRotate(const XMFLOAT3& Rotate) 
{
	SetRotate(Rotate.x, Rotate.y, Rotate.z);

	XMMATRIX xmmatRotate = XMMatrixRotationY(XMConvertToRadians(Rotate.y));
	XMVECTOR l = XMVectorSet(0,0,1,0);
	XMVECTOR r = XMVectorSet(1,0,0,0);

	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(l, xmmatRotate));
	XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(r, xmmatRotate));

	m_pBody->SetRotate(Rotate);
}

void Object::SetObjectRotation(const XMFLOAT3& xmf3Rotation)
{
	SetRotate(xmf3Rotation.x, xmf3Rotation.y, xmf3Rotation.z);

	XMMATRIX xmmatRotate = XMMatrixRotationY(XMConvertToRadians(xmf3Rotation.y));
	XMVECTOR l = XMVectorSet(0, 0, 1, 0);
	XMVECTOR r = XMVectorSet(1, 0, 0, 0);

	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(l, xmmatRotate));
	XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(r, xmmatRotate));

}
