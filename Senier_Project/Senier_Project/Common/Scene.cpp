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

	//std::unique_ptr<Shader> defaultShader = std::make_unique<Shader>();
	std::unique_ptr<Shader> defaultShader = std::make_unique<SkinnedMeshShader>();
	if (!defaultShader->Initialize(pd3dDevice, pd3dCommandList, NULL))
		return false;
	m_vpShaders.push_back(move(defaultShader));


	// ModelData 로드 미완성
	//char strFileName[64] = "Model/Ethan.bin";
	char strFileName[64] = "Model/Angrybot.bin";

	std::shared_ptr<ModelDataInfo> tmpModel; 
	tmpModel = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, strFileName);

	m_vpObjs.emplace_back(std::make_shared<Object>(pd3dDevice, pd3dCommandList, tmpModel, 1));
	m_vpObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 0);

	m_pCamera = std::make_unique<Camera>();
	m_pCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));

	m_pCamera->SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

	return true;
}

void Scene::OnResize(float aspectRatio)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);

}

void Scene::Update(const GameTimer& gt)
{
	for (int i = 0; i < m_vpShaders.size(); ++i)
	{
		m_vpShaders[i]->Update(gt);
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

	for (int i = 0; i < m_vpObjs.size(); ++i)
	{
		m_vpObjs[i]->Animate(gt);
		m_vpObjs[i]->Update(gt);
	}
}

void Scene::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_vpShaders.size(); ++i)
	{
		m_vpShaders[i]->OnPrepareRender(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootConstantBufferView(m_vpShaders[i]->GetPassBufferNum(), m_pPassCB->Resource()->GetGPUVirtualAddress());

		m_vpShaders[i]->Render(gt, pd3dCommandList);
	}

	for (int i = 0; i < m_vpObjs.size(); ++i)
	{
		m_vpObjs[i]->Render(gt, pd3dCommandList);
	}
}

void Scene::OnWinKeyboardInput(WPARAM wParam)
{
	for (int i = 0; i < m_vpShaders.size(); ++i)
		m_vpShaders[i]->OnWinKeyboardInput(wParam);

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

		m_vpObjs[0]->SetPitch(m_vpObjs[0]->GetPitch() - dy);
		m_vpObjs[0]->SetYaw(m_vpObjs[0]->GetYaw() - dx);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{

	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}
