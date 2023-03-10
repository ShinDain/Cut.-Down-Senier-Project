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

	mShaders.emplace_back(defaultShader);

	mObj = std::make_unique<Object>();
	if (!mObj->Initialize(pd3dDevice, pd3dCommandList, NULL))
		return false;

	return true;
}

void Scene::OnResize(float aspectRatio)
{
}

void Scene::Update(const GameTimer& gt)
{
	for (int i = 0; i < mShaders.size(); ++i)
	{
		mShaders[i]->Update(gt);
	}

	XMMATRIX viewProj = XMLoadFloat4x4(&mViewProj);

	tmpPassConstant passConstant;
	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	mPassCB->CopyData(0, passConstant);
}

void Scene::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < mShaders.size(); ++i)
	{
		mShaders[i]->OnPrepareRender(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, mPassCB->Resource()->GetGPUVirtualAddress());

		mShaders[i]->Render(gt, pd3dCommandList);
	}

}

void Scene::OnWinKeyboardInput(WPARAM wParam)
{
	for (int i = 0; i < mShaders.size(); ++i)
		mShaders[i]->OnWinKeyboardInput(wParam);
}
