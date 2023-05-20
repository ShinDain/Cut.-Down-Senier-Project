#include "../Header/Weapon.h"

Weapon::Weapon(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
			   ObjectInitData objData, char* pstrFollowObject, std::shared_ptr<Object> pFollowObject,
			   std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pFollowObject = pFollowObject->FindFrame(pstrFollowObject);

	pFollowObject->m_pAnimationController->SetCallbackKey(0, 0, 0.0f, (void*)(this));
	//m_pAnimationController->SetCallbackKey(2, 1, 0.0f, (void*)(m_pWeapon.get()));
}

Weapon::~Weapon()
{
	Object::Destroy();
}

bool Weapon::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	SetChild(pModelData->m_pRootObject);
	if (nAnimationTracks > 0)
		m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pModelData);

	std::shared_ptr<Collider> pCollider;
	std::shared_ptr<RigidBody> pBody;

	pBody = nullptr;

	// 충돌체 타입에 따라 
	switch (objData.colliderType)
	{
	case Collider_Plane:
	{
		std::shared_ptr<ColliderPlane> pColliderPlane;
		XMFLOAT3 direction = XMFLOAT3(objData.xmf4Orientation.x, objData.xmf4Orientation.y, objData.xmf4Orientation.z);
		pColliderPlane = std::make_shared<ColliderPlane>(pBody, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), direction, objData.xmf3Extents.x);
		//g_ppColliderPlanes.emplace_back(pColliderPlane);
		pCollider = std::static_pointer_cast<Collider>(pColliderPlane);
	}
	break;

	case Collider_Box:
	{
		std::shared_ptr<ColliderBox> pColliderBox;
		pColliderBox = std::make_shared<ColliderBox>(pBody, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), objData.xmf3Extents);
		//g_ppColliderBoxs.emplace_back(pColliderBox);
		pCollider = std::static_pointer_cast<Collider>(pColliderBox);
	}
	break;

	case Collider_Sphere:
	{
		std::shared_ptr<ColliderSphere> pColliderSphere;
		pColliderSphere = std::make_shared<ColliderSphere>(pBody, XMFLOAT3(0, 0, 0), objData.xmf3Extents.x);
		//g_ppColliderSpheres.emplace_back(pColliderSphere);
		pCollider = std::static_pointer_cast<Collider>(pColliderSphere);
	}
	break;

	default:
		break;
	}

	m_pBody = pBody;
	m_pCollider = pCollider;
	m_xmf3ColliderExtents = objData.xmf3Extents;

	m_xmf3Position = objData.xmf3Position;
	m_xmf3Rotation = objData.xmf3Rotation;
	m_xmf3Scale = objData.xmf3Scale;
	m_xmf4Orientation = objData.xmf4Orientation;
	m_Mass = objData.nMass;

#if defined(_DEBUG)
	if (m_pCollider) m_pCollider->BuildMesh(pd3dDevice, pd3dCommandList);
#endif

	BuildConstantBuffers(pd3dDevice);


	m_pCollider->SetOffsetPosition(XMFLOAT3(0, objData.xmf3Extents.y, 0));
	m_pCollider->SetIsActive(false);

	return true;
}

void Weapon::Update(float elapsedTime)
{
	UpdateToRigidBody(elapsedTime);

	ObjConstant objConstant;
	XMMATRIX world = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX inverseTransWorld = XMMatrixInverse(nullptr, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstant.InverseTransWorld, XMMatrixTranspose(inverseTransWorld));
	if (m_pObjectCB) m_pObjectCB->CopyData(0, objConstant);

	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}
}

void Weapon::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	if(m_pFollowObject) m_xmf4x4World = m_pFollowObject->GetWorld();

	XMMATRIX xmmatWorld = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX xmmatScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	//XMMATRIX xmmatOrientation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
	XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotation.x), XMConvertToRadians(m_xmf3Rotation.y), XMConvertToRadians(m_xmf3Rotation.z));
	XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	// S * R * T
	//XMStoreFloat4x4(&m_xmf4x4LocalTransform, XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatOrientation, xmmatTranslate)));
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatRotate, xmmatTranslate)), xmmatWorld));
	//m_xmf4x4World = m_xmf4x4LocalTransform;

	Object::UpdateTransform(&m_xmf4x4World);

	//m_pCollider->SetWorld(m_xmf4x4World);

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
