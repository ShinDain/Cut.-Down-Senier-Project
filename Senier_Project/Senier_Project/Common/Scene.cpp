#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	mPassCB = std::make_unique<UploadBuffer<tmpPassConstant>>(pd3dDevice, 1, true);

	std::unique_ptr<Shader> defaultShader = std::make_unique<Shader>();
	if (!defaultShader->Initialize(pd3dDevice, pd3dCommandList, NULL))
		return false;
	mShaders.push_back(move(defaultShader));

	mObj = std::make_unique<Object>();
	if (!mObj->Initialize(pd3dDevice, pd3dCommandList, NULL))
		return false;

	mCamera = std::make_unique<Camera>();
	mCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	mCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

	return true;
}

void Scene::OnResize(float aspectRatio)
{
	mCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);

}

void Scene::Update(const GameTimer& gt)
{
	for (int i = 0; i < mShaders.size(); ++i)
	{
		mShaders[i]->Update(gt);
	}

	
	XMFLOAT3 camPos3f = XMFLOAT3(0.0f, 0.0f, -100.f);
	XMVECTOR camPos = XMLoadFloat3(&camPos3f);
	camPos = XMVector3Transform(camPos, XMMatrixRotationY(gt.TotalTime()));

	XMStoreFloat3(&camPos3f, camPos);

	mCamera->LookAt(camPos3f, XMFLOAT3(0.0f, 0.0f, 0.0f), mCamera->GetUp3f());
	mCamera->UpdateViewMatrix();

	XMMATRIX viewProj = XMMatrixMultiply(mCamera->GetView(), mCamera->GetProj());

	tmpPassConstant passConstant;

	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	mPassCB->CopyData(0, passConstant);

	mObj->Update(gt);
}

void Scene::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < mShaders.size(); ++i)
	{
		mShaders[i]->OnPrepareRender(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, mPassCB->Resource()->GetGPUVirtualAddress());

		mShaders[i]->Render(gt, pd3dCommandList);
	}

	mObj->Render(gt, pd3dCommandList);

}

void Scene::OnWinKeyboardInput(WPARAM wParam)
{
	for (int i = 0; i < mShaders.size(); ++i)
		mShaders[i]->OnWinKeyboardInput(wParam);
}
