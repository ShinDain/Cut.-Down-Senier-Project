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

	m_DestroyTime = 0.0f;
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
	// 충돌체 타입에 따라 
	m_nColliderType = objData.colliderType;
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
		pBody = nullptr;
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

	// 오브젝트 파괴 타이머
	if (m_bDestroying)
	{
		if (m_bDissolveStart)
		{
			m_ElapsedDestroyTime += elapsedTime;
			m_DissolveValue = m_ElapsedDestroyTime / (m_DestroyTime);
			if (m_ElapsedDestroyTime >= m_DestroyTime)
			{
				//Cutting(XMFLOAT3(0, 1, 0));
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

	// 무적 시간 경과 누적
	if (m_bInvincible)
	{
		m_ElapsedInvincibleTime += elapsedTime;
		if (m_InvincibleTime <= m_ElapsedInvincibleTime)
		{
			m_ElapsedInvincibleTime = 0.0f;
			m_bInvincible = false;
		}
	}

	if(m_nObjectType != ObjectType::Object_World)
		UpdateToRigidBody(elapsedTime);

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
	// RigidBody를 기준으로 위치를 갱신한다.
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
		m_xmf4Orientation = m_pBody->GetOrientation();
		m_xmf3Rotation = m_pBody->GetRotate();
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
	// Animate 후에 호출되어 Bone 행렬을 갱신
	// 
	if (pxmf4x4Parent)
	{
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4LocalTransform), XMLoadFloat4x4(pxmf4x4Parent)));
	}
	else
	{
		// RootObject인 경우
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
	// 셰이더로 전달될 Bone 행렬 버퍼를 변경한다.
	if (m_pAnimationController) m_pAnimationController->ChangeBoneTransformCB(pd3dCommandList);
	if (m_pObjectCB) pd3dCommandList->SetGraphicsRootConstantBufferView(m_nObjectCBParameterIdx, m_pObjectCB->Resource()->GetGPUVirtualAddress());
}

void Object::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
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

			m_pMesh->Render(elapsedTime, pd3dCommandList);
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
			m_pMesh->Render(elapsedTime, pd3dCommandList);
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

std::shared_ptr<ModelDataInfo> Object::LoadModelDataFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName, const char* pstrTexPath)
{
	FILE* pInFile = NULL;

	char pstrFilePath[64] = { '\0' };
	strcpy_s(pstrFilePath, 64, "Model/");
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
	m_bIsAlive = false;
	Scene::CreateCuttedObject(Scene::m_pd3dDevice, Scene::m_pd3dCommandList, this, 1, xmf3PlaneNormal, false);
	Scene::CreateCuttedObject(Scene::m_pd3dDevice, Scene::m_pd3dCommandList, this, -1, xmf3PlaneNormal, false);
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
				if (!strcmp(pstrToken, "Vampire"))
				{
					float a = 100;
				}
				pModelData->m_pAnimationSets->m_vpAnimatedBoneFrameCaches.emplace_back(pModelData->m_pRootObject->FindFrame(pstrToken));
			}

		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			std::shared_ptr<AnimationSets> pAnimationSets = pModelData->m_pAnimationSets;

			// n번째 애니메이션 데이터
			int nAnimationSet = ReadintegerFromFile(pInFile);
			
			// 애니메이션 명칭
			ReadStringFromFile(pInFile, pstrToken);

			float Length = ReadFloatFromFile(pInFile);				// 애니메이션 길이
			int nFramesPerSecond = ReadintegerFromFile(pInFile);	// 초당 프레임
			int nKeyFrames = ReadintegerFromFile(pInFile);			// 총 키프레임 수
			
			// AnimationSet 생성
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


			// n번째 애니메이션 Set 참조
			std::shared_ptr<AnimationSet> pAnimationSet = pAnimationSets->m_vpAnimationSets[nAnimationSet];

			// KeyFrame 수만큼 순회하며 변환행렬 저장
			for (int i = 0; i < nKeyFrames; i++)
			{
				::ReadStringFromFile(pInFile, pstrToken);
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKey = ReadintegerFromFile(pInFile);		// n번째 키	== i
					float KeyTime = ReadFloatFromFile(pInFile);		// n번째 키 시간

#ifdef _WITH_ANIMATION_SRT
					m_pfKeyFrameScaleTimes[i] = fKeyTime;
					m_pfKeyFrameRotationTimes[i] = fKeyTime;
					m_pfKeyFrameTranslationTimes[i] = fKeyTime;
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameScales[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4KeyFrameRotations[i], sizeof(XMFLOAT4), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameTranslations[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
#else
					// n번째 키프레임 시간 저장
					pAnimationSet->m_vKeyFrameTimes[i] = KeyTime;
					// n번째 키에서의 변환 행렬 저장 (Bone 개수만큼)
					nReads = (UINT)::fread(&(pAnimationSet->m_vvxmf4x4KeyFrameTransforms[nKey][0]), sizeof(XMFLOAT4X4), pAnimationSets->m_nAnimatedBoneFrames, pInFile);
#endif
				}
			}

		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
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

void Object::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	if (m_bInvincible)
		return;
	// 피격 무적
	m_bInvincible = true;

	// 체력 감소
	m_HP -= power;
	if (m_HP <= 0)
	{
		m_bDestroying = true;
		// 파괴된 위치에 아이템 생성
		Scene::CreateObject(Scene::m_pd3dDevice, Scene::m_pd3dCommandList, m_xmf3Position, 
			XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), ITEM_MODEL_NAME, 0);
	}

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