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

	mMesh = std::make_unique<Mesh>();
	if (mMesh == nullptr)
		return false;
	else 
		mMesh->BuildMesh(pd3dDevice, pd3dCommandList);

	return true;
}

void Object::Update(const GameTimer& gt)
{
	mWorld._41 = mPosition.x;
	mWorld._42 = mPosition.y;
	mWorld._43 = mPosition.z;

	XMMATRIX world = XMLoadFloat4x4(&mWorld);

	tmpObjConstant objConstant;
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(world));
	mObjectCB->CopyData(0, objConstant);

}

void Object::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, mObjectCB->Resource()->GetGPUVirtualAddress());

	mMesh->OnprepareRender(gt, pd3dCommandList);
	mMesh->Render(gt, pd3dCommandList);
}

void Object::BuildConstantBuffers(ID3D12Device* pd3dDevice)
{
	mObjectCB = std::make_unique<UploadBuffer<tmpObjConstant>>(pd3dDevice, 1, true);
	mObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(tmpObjConstant));
}
