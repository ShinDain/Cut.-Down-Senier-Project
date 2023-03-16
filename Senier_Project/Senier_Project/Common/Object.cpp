#include "Object.h"

Object::Object()
{
}

Object::~Object()
{
}

bool Object::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	BuildConstantBuffers(pd3dDevice);

	mMesh = std::make_shared<Mesh>();
	if (mMesh == nullptr)
		return false;
	else 
		mMesh->BuildMesh(pd3dDevice, pd3dCommandList);

	mMaterial = std::make_shared<Material>();
	if (mMaterial == nullptr)
		return false;
	else
	{
		mMaterial->LoadTexture(pd3dDevice, pd3dCommandList, L"Textures\\bricks.dds");
		mMaterial->BuildDescriptorHeap(pd3dDevice);
	}

	return true;
}

void Object::Update(const GameTimer& gt)
{
	mWorld._41 = mPosition.x;
	mWorld._42 = mPosition.y;
	mWorld._43 = mPosition.z;
	
	XMMATRIX world = XMMatrixMultiply(XMMatrixRotationY(mYaw), XMMatrixRotationAxis(XMLoadFloat3(&mRight), mPitch));
	world = XMMatrixMultiply(world, XMLoadFloat4x4(&mWorld));

	tmpObjConstant objConstant;
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(world));
	if(mObjectCB) mObjectCB->CopyData(0, objConstant);

	if (mpSibling) mpSibling->Update(gt);
	if (mpChild) {
		mpChild->SetParentWorld(mWorld);
		mpChild->Update(gt);
	}
}

void Object::PrepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (mObjectCB) pd3dCommandList->SetGraphicsRootConstantBufferView(0, mObjectCB->Resource()->GetGPUVirtualAddress());
}

void Object::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if(mMaterial) mMaterial->OnPrepareRender(pd3dCommandList);

	if (mMesh)
	{
		mMesh->OnprepareRender(gt, pd3dCommandList);
		mMesh->Render(gt, pd3dCommandList);
	}

	if (mpSibling) mpSibling->Render(gt, pd3dCommandList);
	if (mpChild) mpChild->Render(gt, pd3dCommandList);
}

void Object::BuildConstantBuffers(ID3D12Device* pd3dDevice)
{
	mObjectCB = std::make_unique<UploadBuffer<tmpObjConstant>>(pd3dDevice, 1, true);
	mObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(tmpObjConstant));
}

void Object::SetChild(std::shared_ptr<Object> pChild)
{
	if (mpChild != NULL)
	{
		if (pChild) pChild->mpSibling = mpChild->mpSibling;
		mpChild->mpSibling = pChild;
	}
	else
	{
		mpChild = pChild;
	}
}

void Object::SetMesh(std::shared_ptr<Mesh> pMesh)
{
}

void Object::SetMaterial(std::shared_ptr<Material> pMesh)
{
}
