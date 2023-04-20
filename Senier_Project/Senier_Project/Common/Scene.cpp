#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 패스 버퍼 생성
	m_pPassCB = std::make_unique<UploadBuffer<PassConstant>>(pd3dDevice, 1, true);

	// static Shader 초기화

	// 모델 데이터 로드
	char strFileName1[64] = "Model/Angrybot.bin";
	char strFileName2[64] = "Model/unitychan.bin";

	std::shared_ptr<ModelDataInfo> tmpModel1; 
	tmpModel1 = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, strFileName1);
	//std::shared_ptr<ModelDataInfo> tmpModel2;
	//tmpModel2 = Object::LoadModelDataFromFile(pd3dDevice, pd3dCommandList, strFileName2);


	// 오브젝트 추가
	m_vpAllObjs.emplace_back(std::make_shared<Object>(pd3dDevice, pd3dCommandList, tmpModel1, 1));
	m_vpAllObjs[0]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	m_vpAllObjs[0]->m_pAnimationController->SetTrackPosition(0, 0.2f);
	m_vpAllObjs[0]->SetPosition(10, 0.0f, 0.0f);
	m_vpAllObjs[0]->SetScale(10.0f, 10.0f, 10.0f);

	// 카메라 초기화
	m_pCamera = std::make_unique<Third_Person_Camera>(m_vpAllObjs[0]);

	return true;
}

void Scene::OnResize(float aspectRatio)
{
	m_pCamera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 10000.0f);
}

void Scene::Update(const GameTimer& gt)
{
	m_pCamera->Update(gt.DeltaTime());

	XMMATRIX view = m_pCamera->GetView();
	XMMATRIX viewProj = XMMatrixMultiply(view, m_pCamera->GetProj());

	// 패스 버퍼 : 뷰 * 투영 변환 행렬 업데이트
	PassConstant passConstant;
	XMStoreFloat4x4(&passConstant.ViewProj, XMMatrixTranspose(viewProj));
	m_pPassCB->CopyData(0, passConstant);

	// ImageObject 렌더를 위한  직교 투영행렬 업데이트
	m_xmf4x4ImgObjMat = m_pCamera->GetOrtho4x4f();

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		m_vpAllObjs[i]->Animate(gt);
		if (!m_vpAllObjs[i]->m_pAnimationController)
			m_vpAllObjs[i]->UpdateTransform(NULL);
		m_vpAllObjs[i]->Update(gt);
	}
}

void Scene::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// img 오브젝트 렌더링 수정 필
	{
		g_Shaders[RenderLayer::Image]->ChangeShader(pd3dCommandList);
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4ImgObjMat, 0);
	}

	g_Shaders[RenderLayer::Static]->ChangeShader(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_pPassCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < m_vpAllObjs.size(); ++i)
	{
		m_vpAllObjs[i]->Render(gt, pd3dCommandList);
	}
}

void Scene::ProcessInput(UCHAR* pKeybuffer)
{
	float dx, dy;
	dx = dy = 0;

	POINT ptCursorPos;

	if (pKeybuffer[VK_LBUTTON] & 0xF0)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
		dx = (float)(ptCursorPos.x - CLIENT_WIDTH / 2) / 3.0f;
		dy = (float)(ptCursorPos.y - CLIENT_HEIGHT / 2) / 3.0f;
		//m_LastMousePos = ptCursorPos;
		SetCursorPos(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);
	}

	DWORD dwDirection = 0;
	if (pKeybuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeybuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeybuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeybuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;

	if(dwDirection != 0 || dx != 0 || dy != 0)
	{
		if (dx != 0 || dy != 0)
		{
			m_vpAllObjs[0]->Rotate(0,-dx, 0);
		}

		m_vpAllObjs[0]->Move(dwDirection, m_vpAllObjs[0]->GetSpeed());
	}

}
