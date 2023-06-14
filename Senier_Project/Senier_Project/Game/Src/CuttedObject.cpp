#include "../Header/CuttedObject.h"


CuttedStaticObject::CuttedStaticObject()
{
}

CuttedStaticObject::CuttedStaticObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

CuttedStaticObject::~CuttedStaticObject()
{
	Object::Destroy();
}

bool CuttedStaticObject::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Object::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	return true;
}

void CuttedStaticObject::Update(float elapsedTime)
{
	Object::Update(elapsedTime);

	UpdateCuttedCB();
}

void CuttedStaticObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	Object::UpdateTransform(NULL);
}

void CuttedStaticObject::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootConstantBufferView(5, m_pCuttedCB->Resource()->GetGPUVirtualAddress());

	Object::Render(elapsedTime, pd3dCommandList);
}

void CuttedStaticObject::UpdateCuttedCB()
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
	float planeDistance_1 = XMVectorGetX(XMVectorSum(planeDistanceVec_1));
	cuttedConstant.PlaneDistance_1 = planeDistance_1;

	XMVECTOR planeNormal_2 = XMLoadFloat3(&m_PlaneNormal[1]);
	planeNormal_2 = XMVector3TransformNormal(planeNormal_2, xmmatRotation);
	planeNormal_2 = XMVector3Normalize(planeNormal_2);
	XMFLOAT3 xmf3PlaneNormal_2;
	XMStoreFloat3(&xmf3PlaneNormal_2, planeNormal_2);
	cuttedConstant.xmf3PlaneNormal_2 = xmf3PlaneNormal_2;
	XMVECTOR planeDistanceVec_2 = thisPosition * planeNormal_2;
	float planeDistance_2 = XMVectorGetX(XMVectorSum(planeDistanceVec_2));
	cuttedConstant.PlaneDistance_2 = planeDistance_2;

	XMVECTOR planeNormal_3 = XMLoadFloat3(&m_PlaneNormal[2]);
	planeNormal_3 = XMVector3TransformNormal(planeNormal_3, xmmatRotation);
	planeNormal_3 = XMVector3Normalize(planeNormal_3);
	XMFLOAT3 xmf3PlaneNormal_3;
	XMStoreFloat3(&xmf3PlaneNormal_3, planeNormal_3);
	cuttedConstant.xmf3PlaneNormal_3 = xmf3PlaneNormal_3;
	XMVECTOR planeDistanceVec_3 = thisPosition * planeNormal_3;
	float planeDistance_3 = XMVectorGetX(XMVectorSum(planeDistanceVec_3));
	cuttedConstant.PlaneDistance_3 = planeDistance_3;



	cuttedConstant.PlaneDirection_1 = m_PlaneDirection[0];
	cuttedConstant.PlaneDirection_2 = m_PlaneDirection[1];
	cuttedConstant.PlaneDirection_3 = m_PlaneDirection[2];
	
	m_pCuttedCB->CopyData(0, cuttedConstant);
}

void CuttedStaticObject::BuildConstantBuffers(ID3D12Device* pd3dDevice)
{
	m_pObjectCB = std::make_unique<UploadBuffer<ObjConstant>>(pd3dDevice, 1, true);
	m_ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjConstant));

	m_pCuttedCB = std::make_unique<UploadBuffer<CuttedConstant>>(pd3dDevice, 1, true);
	m_CuttedCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(CuttedConstant));
}
