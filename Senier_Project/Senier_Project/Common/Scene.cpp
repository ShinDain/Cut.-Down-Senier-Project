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
	mCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -100.f));

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
	XMVECTOR camPos = mCamera->GetPosition();
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

	float delta = 1.0f;

	if (wParam == 'W')
	{
		mCamera->Walk(delta);
	}

	if (wParam == 'S')
	{
		mCamera->Walk(-delta);
	}

	if (wParam == 'A')
	{
		mCamera->Strafe(-delta);
	}

	if (wParam == 'D')
	{
		mCamera->Strafe(delta);
	}
}

void Scene::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Scene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void Scene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mObj->SetPitch(mObj->GetPitch() - dy);
		mObj->SetYaw(mObj->GetYaw() - dx);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{

	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
