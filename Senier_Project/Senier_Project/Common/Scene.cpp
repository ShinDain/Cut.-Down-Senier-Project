#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pPassCB = std::make_unique<UploadBuffer<tmpPassConstant>>(pd3dDevice, 1, true);

	std::unique_ptr<Shader> defaultShader = std::make_unique<Shader>();
	if (!defaultShader->Initialize(pd3dDevice, pd3dCommandList, NULL))
		return false;
	m_ppShaders.push_back(move(defaultShader));

	char strFileName[64] = "Model/Mi24.bin";

	std::shared_ptr<Object> tmpObj; 
	tmpObj = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, strFileName);

	m_ppObjs.emplace_back(std::make_shared<Object>());

	for (int i = 0; i < m_ppObjs.size(); ++i)
	{
		m_ppObjs[i]->BuildConstantBuffers(pd3dDevice);
		m_ppObjs[i]->SetChild(tmpObj);
	}
	
	m_pCamera = std::make_unique<Camera>();
	m_pCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -100.f));

	m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

	return true;
}

void Scene::OnResize(float aspectRatio)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);

}

void Scene::Update(const GameTimer& gt)
{
	for (int i = 0; i < m_ppShaders.size(); ++i)
	{
		m_ppShaders[i]->Update(gt);
	}

	XMFLOAT3 camPos3f = XMFLOAT3(0.0f, 0.0f, -100.f);
	XMVECTOR camPos = m_pCamera->GetPosition();
	XMStoreFloat3(&camPos3f, camPos);

	m_pCamera->LookAt(camPos3f, XMFLOAT3(0.0f, 0.0f, 0.0f), m_pCamera->GetUp3f());
	m_pCamera->UpdateViewMatrix();

	XMMATRIX viewProj = XMMatrixMultiply(m_pCamera->GetView(), m_pCamera->GetProj());

	tmpPassConstant passConstant;

	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	m_pPassCB->CopyData(0, passConstant);

	for (int i = 0; i < m_ppObjs.size(); ++i)
	{
		m_ppObjs[i]->Update(gt);
	}
}

void Scene::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_ppShaders.size(); ++i)
	{
		m_ppShaders[i]->OnPrepareRender(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());

		m_ppShaders[i]->Render(gt, pd3dCommandList);
	}

	for (int i = 0; i < m_ppObjs.size(); ++i)
	{
		m_ppObjs[i]->Render(gt, pd3dCommandList);
	}
}

void Scene::OnWinKeyboardInput(WPARAM wParam)
{
	for (int i = 0; i < m_ppShaders.size(); ++i)
		m_ppShaders[i]->OnWinKeyboardInput(wParam);

	float delta = 1.0f;

	if (wParam == 'W')
	{
		m_pCamera->Walk(delta);
	}

	if (wParam == 'S')
	{
		m_pCamera->Walk(-delta);
	}

	if (wParam == 'A')
	{
		m_pCamera->Strafe(-delta);
	}

	if (wParam == 'D')
	{
		m_pCamera->Strafe(delta);
	}
}

void Scene::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void Scene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void Scene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

		m_ppObjs[0]->SetPitch(m_ppObjs[0]->GetPitch() - dy);
		m_ppObjs[0]->SetYaw(m_ppObjs[0]->GetYaw() - dx);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{

	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}
