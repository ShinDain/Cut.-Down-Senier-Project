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

	std::shared_ptr<Object> tmpObj = std::make_shared<Object>();
	if (!tmpObj->Initialize(pd3dDevice, pd3dCommandList, NULL))
		return false;

	mObjs.emplace_back(std::make_shared<Object>());
	mObjs.emplace_back(std::make_shared<Object>());
	
	for (int i = 0; i < mObjs.size(); ++i)
	{
		mObjs[i]->BuildConstantBuffers(pd3dDevice);
	}

	mObjs[0]->SetChild(tmpObj);
	mObjs[1]->SetChild(tmpObj);

	mObjs[0]->SetPosition(XMFLOAT3(-10.f, 0.0f, 0.0f));
	mObjs[1]->SetPosition(XMFLOAT3(10.f, 0.0f, 0.0f));

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

	for (int i = 0; i < mObjs.size(); ++i)
	{
		mObjs[i]->Update(gt);
	}
}

void Scene::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < mShaders.size(); ++i)
	{
		mShaders[i]->OnPrepareRender(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, mPassCB->Resource()->GetGPUVirtualAddress());

		mShaders[i]->Render(gt, pd3dCommandList);
	}

	for (int i = 0; i < mObjs.size(); ++i)
	{
		mObjs[i]->PrepareRender(gt, pd3dCommandList);
		mObjs[i]->Render(gt, pd3dCommandList);
	}
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

		mObjs[0]->SetPitch(mObjs[0]->GetPitch() - dy);
		mObjs[1]->SetYaw(mObjs[1]->GetYaw() - dx);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{

	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
