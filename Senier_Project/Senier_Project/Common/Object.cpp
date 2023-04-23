#include "Shader.h"
#include "Object.h"

Object::Object()
{
}

Object::Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	SetChild(pModelData->m_pRootObject);
	if(nAnimationTracks > 0)
		m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pModelData);

#if defined(_DEBUG)
	m_pCollider = std::make_shared<RigidCollider>(m_xmf3ColliderCenter, m_xmf3ColliderExtents, Collider_Type_Box, pd3dDevice, pd3dCommandList);
#endif
}

Object::~Object()
{
}

bool Object::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	BuildConstantBuffers(pd3dDevice);

	return true;
}

void Object::Animate(const GameTimer& gt)
{
	if (m_pAnimationController)
		m_pAnimationController->AdvanceTime(gt.DeltaTime(), this);

	if (m_pSibling) { m_pSibling->Animate(gt); }
	if (m_pChild) {	m_pChild->Animate(gt); }
}

void Object::Update(const GameTimer& gt)
{
	// Object의 Value를 갱신

	// 속도에 의한 위치변화 계산 후 갱신
	CalculatePositionByVelocity(gt.DeltaTime());
	CalculateRotateByAngleVelocity(gt.DeltaTime());

	ObjConstant objConstant;
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if(m_pObjectCB) m_pObjectCB->CopyData(0, objConstant);

	if (m_pSibling) {
		m_pSibling->Update(gt);
	}
	if (m_pChild) {
		m_pChild->Update(gt);
	}
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
		XMMATRIX xmmatScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
		XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), XMConvertToRadians(m_Roll));
		XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		// S * R * T
		XMStoreFloat4x4(&m_xmf4x4LocalTransform, XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatRotate, xmmatTranslate)));

		m_xmf4x4World = m_xmf4x4LocalTransform;
	}

	if (m_pSibling) {
		m_pSibling->UpdateTransform(pxmf4x4Parent);
	}
	if (m_pChild) {
		m_pChild->UpdateTransform(&m_xmf4x4World);
	}
}

void Object::OnPrepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 셰이더로 전달될 Bone 행렬 버퍼를 변경한다.
	if (m_pAnimationController) m_pAnimationController->ChangeBoneTransformCB(pd3dCommandList);
	
}

void Object::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(gt, pd3dCommandList);

	if (m_pMesh)
	{
		XMFLOAT4X4 xmf4x4World;
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

		for (int i = 0; i < m_vpMaterials.size(); ++i)
		{
			m_vpMaterials[i]->MaterialSet(pd3dCommandList);

			m_pMesh->Render(gt, pd3dCommandList);
		}
	}

#if defined(_DEBUG)
	if (m_pCollider)
	{
		/*char tmpstr[64] = "Character1_LeftUpLeg";
		Object* tmpobj = FindFrame(tmpstr).get();

		if (tmpobj)
		{
			m_pCollider->SetWorld(tmpobj->GetWorld());
		}*/
		m_pCollider->SetWorld(this->GetWorld());
		m_pCollider->Update(gt.DeltaTime());

		g_Shaders[RenderLayer::Collider]->ChangeShader(pd3dCommandList);
		m_pCollider->Render(gt.DeltaTime(), pd3dCommandList);
	}
#endif

	if (m_pSibling) m_pSibling->Render(gt, pd3dCommandList);
	if (m_pChild) m_pChild->Render(gt, pd3dCommandList);
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

std::shared_ptr<ModelDataInfo> Object::LoadModelDataFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
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
				pModelData->m_pRootObject = Object::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile, &nSkinnedMeshes, NULL);
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

std::shared_ptr<Object> Object::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, int* pnSkinnedMeshes, Object* pRootObject)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	std::shared_ptr<Object> pObject = std::make_shared<Object>();
	pObject->Initialize(pd3dDevice, pd3dCommandList, NULL);

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
			nReads = (UINT)fread(&pObject->m_Pitch, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_Yaw, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_Roll, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf4Quaternion, sizeof(float), 4, pInFile);
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
				pObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
			else
				pObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pInFile, pObject.get());
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
						std::shared_ptr<Object> pChild = LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile, pnSkinnedMeshes, pRootObject);
						if (pChild) pObject->SetChild(pChild);
					}
					else
					{
						std::shared_ptr<Object> pChild = LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile, pnSkinnedMeshes, pObject.get());
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

void Object::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, Object* pRootObject)
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
					vpMat[nMatcnt]->SetShader(g_Shaders[RenderLayer::Skinned]);
				}
				else
				{
					vpMat[nMatcnt]->SetShader(g_Shaders[RenderLayer::Static]);
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
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			vpMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile, pRootObject);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}

	//for (int i = 0; i < vpMat.size(); ++i)
	//	vpMat[i]->BuildDescriptorHeap(pd3dDevice);

	SetMaterials(vpMat);
}

void Object::Impulse(XMFLOAT3 xmf3Impulse, XMFLOAT3 xmf3CollisionNormal, XMFLOAT3 xmf3CollisionPoint)
{
	// 충격량에 따른 속도 변화 갱신
	CalculateDeltaVelocityByImpulse(xmf3Impulse);
	CalculateDeltaAngleVelocityByImpulse(xmf3Impulse, xmf3CollisionNormal, xmf3CollisionPoint);
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
	if (!strncmp(GetName(), pstrFrameName, strlen(pstrFrameName))) return shared_from_this();

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

void Object::AddForce(XMVECTOR direction, float distance)
{
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR v = XMLoadFloat3(&m_xmf3Velocity);

	XMStoreFloat3(&m_xmf3Velocity, XMVectorMultiplyAdd(direction, s, v));
}

void Object::Move(DWORD dwDirection, float distance)
{
	XMVECTOR direction = XMVectorZero();
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);

	/*if (dwDirection & DIR_FORWARD)
	{
		direction = XMVectorAdd(direction, l);
	}
	if (dwDirection & DIR_BACKWARD)
	{
		direction = XMVectorAdd(direction, -l);
	}
	if (dwDirection & DIR_LEFT)
	{
		direction = XMVectorAdd(direction, -r);
	}
	if (dwDirection & DIR_RIGHT)
	{
		direction = XMVectorAdd(direction, r);
	}

	direction = XMVector3Normalize(direction);

	AddForce(direction, distance);*/

	if (dwDirection & DIR_FORWARD)
	{
		Impulse(XMFLOAT3(0, 1, 0), XMFLOAT3(0, -1, 0), XMFLOAT3(-5.0f, -5.0f, 5.0f));
	}
	if (dwDirection & DIR_BACKWARD)
	{
		Impulse(XMFLOAT3(0, -1, 0), XMFLOAT3(0, 1, 0), XMFLOAT3(-5.0f, 5.0f, 5.0f));
	}
	if (dwDirection & DIR_LEFT)
	{
		Impulse(XMFLOAT3(-10, 0, 0), XMFLOAT3(-1, 0, 0),XMFLOAT3(5, 5, -5));
	}
	if (dwDirection & DIR_RIGHT)
	{
		Impulse(XMFLOAT3(10, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(-5, 5, -5));
	}
}

void Object::Rotate(float x, float y, float z)
{
	// x : Pitch, y : Yaw, z : Roll
	// 3인칭 카메라 기준
	if (x != 0.0f)
	{
		m_Pitch += x;
		if (m_Pitch > +89.0f) { x -= (m_Pitch - 89.0f); m_Pitch = +89.0f; }
		if (m_Pitch < -89.0f) { x -= (m_Pitch + 89.0f); m_Pitch = -89.0f; }
	}
	if (y != 0.0f)
	{
		m_Yaw += y;
		if (m_Yaw > 360.0f) m_Yaw -= 360.0f;
		if (m_Yaw < 0.0f) m_Yaw += 360.0f;
	}
	if (z != 0.0f)
	{
		m_Roll += z;
		//if (m_Roll > +20.0f) { z -= (m_Roll - 20.0f); m_Roll = +20.0f; }
		//if (m_Roll < -20.0f) { z -= (m_Roll + 20.0f); m_Roll = -20.0f; }
	}
	if (y != 0.0f)
	{
		XMMATRIX xmmatRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
		XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR r = XMLoadFloat3(&m_xmf3Right);

		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(l, xmmatRotate));
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(r, xmmatRotate));
	}
}

void Object::Walk(float delta)
{
	// Look vec 획득 후 해당 방향으로 가속
	XMVECTOR s = XMVectorReplicate(m_Acceleration * delta);
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR v = XMLoadFloat3(&m_xmf3Velocity);
	// Look 방향으로 Velocity 값을 더해주기만
	// 실제 이동은 Update에서 처리될거임.
	XMStoreFloat3(&m_xmf3Velocity, XMVectorMultiplyAdd(s, l, v));

}

void Object::Strafe(float delta)
{
	// Right vec 획득 후 해당 방향으로 가속
	XMVECTOR s = XMVectorReplicate(m_Acceleration * delta);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR v = XMLoadFloat3(&m_xmf3Velocity);
	// Look 방향으로 Velocity 값을 더해주기만
	// 실제 이동은 Update에서 처리될거임.
	XMStoreFloat3(&m_xmf3Velocity, XMVectorMultiplyAdd(s, r, v));
}


void Object::CalculatePositionByVelocity(float Etime)
{
	// velocity의 크기 체크
	XMVECTOR EtimeVec = XMVectorReplicate(Etime);											// 경과 시간

	XMFLOAT3 xmf3VelXZ = XMFLOAT3(m_xmf3Velocity.x, 0, m_xmf3Velocity.z);
	XMFLOAT3 xmf3VelY = XMFLOAT3(0, m_xmf3Velocity.y, 0);
	XMVECTOR velXZ = XMLoadFloat3(&xmf3VelXZ);												// XZ - 속도
	XMVECTOR velY = XMLoadFloat3(&xmf3VelY);												// Y - 속도

	XMVECTOR speedXZ = XMVector3Length(velXZ);
	XMVECTOR speedY = XMVector3Length(velY);
	XMFLOAT3 xmf3SpeedXZ = XMFLOAT3(0, 0, 0);
	XMFLOAT3 xmf3SpeedY = XMFLOAT3(0, 0, 0);
	XMStoreFloat3(&xmf3SpeedXZ, speedXZ);									// XZ - 속력
	XMStoreFloat3(&xmf3SpeedY, speedY);										// Y - 속력

	if (xmf3SpeedXZ.x > m_MaxSpeedXZ)	// XZ - 속력이 MaxSpeedXZ 초과
	{
		velXZ = XMVector3Normalize(velXZ);
		velXZ = velXZ * m_MaxSpeedXZ;
		XMStoreFloat3(&xmf3VelXZ, velXZ);
		m_xmf3Velocity.x = xmf3VelXZ.x;
		m_xmf3Velocity.z = xmf3VelXZ.z;
	}
	if (xmf3SpeedY.x > m_MaxSpeedY)		// Y - 속력이 MaxSpeedY 초과
	{
		velY = XMVector3Normalize(velY);
		velY = velY * m_MaxSpeedY;
		XMStoreFloat3(&xmf3VelY, velY);
		m_xmf3Velocity.y = xmf3VelY.y;
	}

	if (xmf3SpeedXZ.x <= FLT_EPSILON)		// XZ - 속력이 EPSILON 이하
	{
		velXZ = XMVectorZero();
		m_xmf3Velocity.x = 0;
		m_xmf3Velocity.z = 0;
	}
	if (xmf3SpeedY.x <= FLT_EPSILON)		// Y - 속력이 EPSILON 이하
	{
		velY = XMVectorZero();
		m_xmf3Velocity.y = 0;
	}

	// 경과 시간 비례 이동 거리 계산, 적용
	XMFLOAT3 xmf3TranslatePos;
	XMStoreFloat3(&xmf3TranslatePos, XMVectorMultiply(velXZ, EtimeVec));
	XMStoreFloat3(&xmf3TranslatePos, XMVectorMultiplyAdd(velY, EtimeVec,XMLoadFloat3(&xmf3TranslatePos)));
	AddPosition(xmf3TranslatePos);

	// 멤버 변수 수정 후의 속도 계산
	XMVECTOR velocity = XMLoadFloat3(&m_xmf3Velocity);

	// 중력에 따른 하락
	XMVECTOR gravity = XMLoadFloat3(&m_xmf3Gravity);
	gravity = XMVectorMultiply(gravity, EtimeVec);
	XMVECTOR gravityLength = XMVector3Length(gravity);

	// 중력의 경우 높이가 지형보다 높다면 항상 작용한다.
	// 
	//if (XMVector3LessOrEqual(speedY, gravityLength))
	if (m_xmf3Position.y <= 0)
	{
		m_xmf3Velocity = XMFLOAT3(m_xmf3Velocity.x, 0, m_xmf3Velocity.z);
	}
	else if (m_xmf3Position.y > 0)
	{
		XMStoreFloat3(&m_xmf3Velocity, (velocity + gravity));

		// 지면에서 떨어져 있는 경우
		return;
	}

	// 마찰에 따른 속도 감소
	XMVECTOR dirXZ = XMVector3Normalize(velXZ);
	XMVECTOR friction = XMVectorReplicate(m_Friction);

	friction = XMVectorMultiply(XMVectorMultiply(-dirXZ, friction), EtimeVec);		 // 반대 방향을 향하는 마찰력 비례 속도
	XMVECTOR frictionLength = XMVector3Length(friction);

	// 감소하는 속력보다 기존 속력이 작은 경우
	if (XMVector3LessOrEqual(speedXZ, frictionLength))
	{
		m_xmf3Velocity = XMFLOAT3(0, m_xmf3Velocity.y, 0);
	}
	else
	{
		XMStoreFloat3(&m_xmf3Velocity, (velocity + friction));
	}
}

void Object::CalculateRotateByAngleVelocity(float Etime)
{
	// AngleVelocity의 크기 체크
	XMVECTOR EtimeVec = XMVectorReplicate(Etime);					// 경과 시간
	XMVECTOR AngleVelocity = XMLoadFloat3(&m_xmf3AngleVelocity);	// 각속도
	XMVECTOR AngleSpeed = XMVector3Length(AngleVelocity);			// 각속력

	XMFLOAT3 xmf3AngleSpeed = XMFLOAT3(0, 0, 0);
	XMStoreFloat3(&xmf3AngleSpeed, AngleSpeed);						// 각속력

	if (xmf3AngleSpeed.x > m_MaxAngleSpeed)							// 각속력이 MaxAngleSpeed 초과
	{
		AngleVelocity = XMVector3Normalize(AngleVelocity);
		AngleVelocity = AngleVelocity * m_MaxAngleSpeed;
		XMStoreFloat3(&m_xmf3AngleVelocity, AngleVelocity);
	}

	if (xmf3AngleSpeed.x <= FLT_EPSILON)							// 각속력이 EPSILON 이하
	{
		AngleVelocity = XMVectorZero();
		m_xmf3AngleVelocity = XMFLOAT3(0, 0, 0);
	}

	// 경과 시간 비례 이동 거리 계산, 적용
	XMFLOAT3 xmf3Rotate; 
	XMStoreFloat3(&xmf3Rotate, XMVectorMultiply(AngleVelocity, EtimeVec));
	XMStoreFloat3(&xmf3Rotate, XMVectorMultiplyAdd(AngleVelocity, EtimeVec, XMLoadFloat3(&xmf3Rotate)));
	this->AddRotate(xmf3Rotate);

	// 마찰에 따른 각속도 감소
	XMVECTOR dirRotate = XMVector3Normalize(AngleVelocity);
	XMVECTOR damping = XMVectorReplicate(m_AngleDamping);

	damping = XMVectorMultiply(XMVectorMultiply(-dirRotate, damping), EtimeVec);		 // 반대 방향을 향하는 마찰력 비례 속도
	XMVECTOR dampingLength = XMVector3Length(damping);

	// 감소하는 속력보다 기존 속력이 작은 경우
	if (XMVector3LessOrEqual(AngleSpeed, dampingLength))
	{
		m_xmf3AngleVelocity = XMFLOAT3(0, 0, 0);
	}
	else
	{
		XMStoreFloat3(&m_xmf3AngleVelocity, (AngleVelocity + damping));
	}

}

void Object::CalculateDeltaVelocityByImpulse(XMFLOAT3 xmf3Impulse)
{
	XMFLOAT3 deltavelocity = xmf3Impulse;
	deltavelocity.x /= m_Mass;
	deltavelocity.y /= m_Mass;
	deltavelocity.z /= m_Mass;

	AddVelocity(deltavelocity);
}

void Object::CalculateDeltaAngleVelocityByImpulse(XMFLOAT3 xmf3Impulse, XMFLOAT3 xmf3CollisionNormal, XMFLOAT3 xmf3CollisionPoint)
{
	// 회전 관성(관성 모멘트) 행렬을 획득
	XMMATRIX rotateInertia = m_pCollider->GetRotateInertiaMatrix();
	// 행렬의 역
	XMMATRIX inverseRotateInertia = XMMatrixInverse(nullptr, rotateInertia);

	XMVECTOR collisionPoint = XMLoadFloat3(&xmf3CollisionPoint);
	XMVECTOR dirImpulse = XMVector3Normalize(XMLoadFloat3(&xmf3Impulse));

	// 각속도 변화 계산
	XMVECTOR deltaAngleVelocity = XMVector3TransformCoord(XMVector3Cross(collisionPoint, dirImpulse), inverseRotateInertia);
	XMFLOAT3 xmf3DeltaAngleVelocity = XMFLOAT3(0, 0, 0);
	XMStoreFloat3(&xmf3DeltaAngleVelocity, deltaAngleVelocity);

	AddAngleVelocity(xmf3DeltaAngleVelocity);

}
