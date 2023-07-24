#include "../Header/CuttedObject.h"
#include "../../DirectXRendering/Header/Scene.h"


CuttedObject::CuttedObject()
{
}

CuttedObject::CuttedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, UINT nPlaneCnt, float direction[], XMFLOAT3 planeNormal[], void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, 0, pContext);

	for (int i = 0; i < nPlaneCnt; ++i)
	{
		m_PlaneDirection[i] = direction[i];
		m_PlaneNormal[i] = planeNormal[i];
	}

	m_nPlaneCnt = nPlaneCnt;
}

CuttedObject::CuttedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, UINT nPlaneCnt, float direction[], XMFLOAT3 planeNormal[],
	UINT nAnimationSet, float trackPosition, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, 0, pContext);

	for (int i = 0; i < nPlaneCnt; ++i)
	{
		m_PlaneDirection[i] = direction[i];
		m_PlaneNormal[i] = planeNormal[i];
	}

	m_nPlaneCnt = nPlaneCnt;

	m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, 1, pModel);
	m_pAnimationController->SetTrackAnimationSet(0, nAnimationSet);
	m_pAnimationController->SetTrackPosition(0, trackPosition);
}

CuttedObject::~CuttedObject()
{
	Object::Destroy();
}

bool CuttedObject::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Object::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	// 생성 후 삭제까지
	m_DestroyTime = 3.0f;
	m_bDestroying = true;
	m_DissolveTime = 2.0f;

	return true;
}

void CuttedObject::Animate(float elapsedTime)
{
	if (m_pAnimationController)
		m_pAnimationController->AdvanceTime(0.0f, this);

	if (m_pSibling) { m_pSibling->Animate(0.0f); }
	if (m_pChild) { m_pChild->Animate(0.0f); }
}

void CuttedObject::Update(float elapsedTime)
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

	if (m_nObjectType != ObjectType::Object_World)
		UpdateToRigidBody(elapsedTime);

	if (m_pObjectCB) UpdateObjectCB();

	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}
	UpdateCuttedCB();
}

void CuttedObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	//Object::UpdateTransform(NULL);

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
		XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotation.x), XMConvertToRadians(m_xmf3Rotation.y), XMConvertToRadians(m_xmf3Rotation.z));
		XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		// S * R * T
		xmmatRotate = XMMatrixMultiply(xmmatRotate, xmmatOrientation);
		//world = XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatOrientation, xmmatTranslate));
		world = XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatRotate, xmmatTranslate));

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

void CuttedObject::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootConstantBufferView(m_CuttedCBIdx, m_pCuttedCB->Resource()->GetGPUVirtualAddress());

	Object::Render(elapsedTime, pd3dCommandList);
}

void CuttedObject::Cutting(XMFLOAT3 xmf3PlaneNormal)
{
	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList, this, 1, XMFLOAT3(1, 1, 0), true);
//	Scene::CreateCuttedObject(g_pd3dDevice, g_pd3dCommandList, this, -1, XMFLOAT3(1, 1, 0), true);
}

void CuttedObject::UpdateCuttedCB()
{
	CuttedConstant cuttedConstant;
	cuttedConstant.PlaneCnt = m_nPlaneCnt;
	
	XMVECTOR thisPosition = XMLoadFloat3(&m_xmf3Position);
	XMMATRIX xmmatRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
	

	XMVECTOR planeNormal_1 = XMLoadFloat3(&m_PlaneNormal[0]);
	planeNormal_1 = XMVector3TransformNormal(planeNormal_1, xmmatRotation);
	planeNormal_1 = XMVector3Normalize(planeNormal_1);
	XMFLOAT3 xmf3PlaneNormal_1;
	XMStoreFloat3(&xmf3PlaneNormal_1, planeNormal_1);
	cuttedConstant.xmf3PlaneNormal_1 = xmf3PlaneNormal_1;
	XMVECTOR planeDistanceVec_1 = thisPosition * planeNormal_1;
	float planeDistance_1 = XMVectorGetX(XMVectorSum(planeDistanceVec_1)) + m_PlaneDistance[0];
	cuttedConstant.PlaneDistance_1 = planeDistance_1;

	XMVECTOR planeNormal_2 = XMLoadFloat3(&m_PlaneNormal[1]);
	planeNormal_2 = XMVector3TransformNormal(planeNormal_2, xmmatRotation);
	planeNormal_2 = XMVector3Normalize(planeNormal_2);
	XMFLOAT3 xmf3PlaneNormal_2;
	XMStoreFloat3(&xmf3PlaneNormal_2, planeNormal_2);
	cuttedConstant.xmf3PlaneNormal_2 = xmf3PlaneNormal_2;
	XMVECTOR planeDistanceVec_2 = thisPosition * planeNormal_2;
	float planeDistance_2 = XMVectorGetX(XMVectorSum(planeDistanceVec_2)) + m_PlaneDistance[1];
	cuttedConstant.PlaneDistance_2 = planeDistance_2;

	XMVECTOR planeNormal_3 = XMLoadFloat3(&m_PlaneNormal[2]);
	planeNormal_3 = XMVector3TransformNormal(planeNormal_3, xmmatRotation);
	planeNormal_3 = XMVector3Normalize(planeNormal_3);
	XMFLOAT3 xmf3PlaneNormal_3;
	XMStoreFloat3(&xmf3PlaneNormal_3, planeNormal_3);
	cuttedConstant.xmf3PlaneNormal_3 = xmf3PlaneNormal_3;
	XMVECTOR planeDistanceVec_3 = thisPosition * planeNormal_3;
	float planeDistance_3 = XMVectorGetX(XMVectorSum(planeDistanceVec_3)) + m_PlaneDistance[2];
	cuttedConstant.PlaneDistance_3 = planeDistance_3;



	cuttedConstant.PlaneDirection_1 = m_PlaneDirection[0];
	cuttedConstant.PlaneDirection_2 = m_PlaneDirection[1];
	cuttedConstant.PlaneDirection_3 = m_PlaneDirection[2];
	
	m_pCuttedCB->CopyData(0, cuttedConstant);
}

void CuttedObject::BuildConstantBuffers(ID3D12Device* pd3dDevice)
{
	m_pObjectCB = std::make_unique<UploadBuffer<ObjConstant>>(pd3dDevice, 1, true);
	m_ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjConstant));

	m_pCuttedCB = std::make_unique<UploadBuffer<CuttedConstant>>(pd3dDevice, 1, true);
	m_CuttedCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(CuttedConstant));
}
