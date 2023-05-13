#include "../Header/Weapon.h"

Weapon::Weapon(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
			   ObjectInitData objData, char* pstrFollowObject, std::shared_ptr<Object> pFollowObject,
			   std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pFollowObject = pFollowObject->FindFrame(pstrFollowObject);
}

Weapon::~Weapon()
{
	Object::Destroy();
}

bool Weapon::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Object::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pCollider->SetOffsetPosition(XMFLOAT3(0, objData.xmf3Extents.y, 0));
	//m_pCollider->SetIsActive(false);

	return true;
}

void Weapon::Update(float elapsedTime)
{
	Object::Update(elapsedTime);
}

void Weapon::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	if(m_pFollowObject) m_xmf4x4World = m_pFollowObject->GetWorld();

	XMMATRIX xmmatWorld = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX xmmatScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	//XMMATRIX xmmatOrientation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
	XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotate.x), XMConvertToRadians(m_xmf3Rotate.y), XMConvertToRadians(m_xmf3Rotate.z));
	XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	// S * R * T
	//XMStoreFloat4x4(&m_xmf4x4LocalTransform, XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatOrientation, xmmatTranslate)));
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatRotate, xmmatTranslate)), xmmatWorld));
	//m_xmf4x4World = m_xmf4x4LocalTransform;

	Object::UpdateTransform(&m_xmf4x4World);
}

void Weapon::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Object::Render(elapsedTime, pd3dCommandList);
}

void Weapon::Destroy()
{
	if (m_pFollowObject) m_pFollowObject.reset();
}

void Weapon::Active()
{
}
