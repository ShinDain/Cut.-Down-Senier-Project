#pragma once

// ��鿡 ���Ե� ������Ʈ �ʱ�ȭ �� ����

#include <algorithm>
#include "Camera.h"
#include "../../Common/Header/MathHelper.h"
#include "Global.h"
#include "Shader.h"
#include "Object.h"
#include "../../Game/Header/Character.h"
#include "../../Game/Header/Weapon.h"
#include "../../Game/Header/Player.h"
#include "../../Game/Header/Monster.h"

#include "../../Physics/Header/Collider.h"
#include "../../Physics/Header/CollisionDetact.h"

class Scene
{
public:
	Scene();
	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;
	virtual ~Scene();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnResize(float aspectRatio);
	virtual void Update(float elapsedTime);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	void ProcessInput(UCHAR* pKeybuffer);
	void KeyDownEvent(WPARAM wParam);

	std::shared_ptr<Object> CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
										const ObjectInitData& objInitData, const char* pstrFilePath,int nAnimationTracks, RenderLayer renderLayer);

	void ClearObjectLayer();

	void Intersect();

private:

	XMFLOAT4X4 m_xmf4x4ViewProj = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4ImgObjMat = MathHelper::identity4x4();

	// �����Ӹ��� �Ѱ��� ��� ����
	std::unique_ptr<UploadBuffer<PassConstant>> m_pPassCB = nullptr;

	// ������Ʈ ��ü��
	std::vector<std::shared_ptr<Object>> m_vpAllObjs;
	std::vector<std::shared_ptr<Object>> m_vObjectLayer[(int)RenderLayer::Render_Count];

	// ���� �������� ���� ī�޶�
	std::unique_ptr<Camera> m_pCamera = nullptr;
	//std::shared_ptr<Character> m_pCharacter = nullptr;

	POINT m_LastMousePos = { 0,0 };
public:
	void SetViewProjMatrix(XMFLOAT4X4 viewProj) { m_xmf4x4ViewProj = viewProj; }


#if defined(_DEBUG) | defined(DEBUG)
public:

	int m_refCnt = 0;
	int m_size = 0;
	
	float m_tTime = 0;
#endif
};






