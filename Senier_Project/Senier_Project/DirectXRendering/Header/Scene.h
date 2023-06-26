#ifndef SCENE_H

#define SCENE_H
// 장면에 삽입될 오브젝트 초기화 및 관리

#include <algorithm>
#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"
#include "../../Common/Header/GameTimer.h"
#include "Camera.h"
#include "Global.h"
#include "Shader.h"
#include "Object.h"
#include "DepthMap.h"
#include "DWriteText.h"

#include "../../Game/Header/ObjectDefaultData.h"
#include "../../Game/Header/Character.h"
#include "../../Game/Header/Weapon.h"
#include "../../Game/Header/Player.h"
#include "../../Game/Header/Monster.h"
#include "../../Game/Header/Item.h"
#include "../../Game/header/CuttedObject.h"

#include "../../Physics/Header/Contact.h"
#include "../../Physics/Header/Collider.h"
#include "../../Physics/Header/CollisionDetact.h"
#include "../../Physics/Header/CollisionResolver.h"

#define PLAYER_TRACK_CNT 7
#define ZOMBIE_TRACK_CNT 4

class Scene
{
public:
	Scene();
	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;
	virtual ~Scene();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<DWriteText> pDWriteText);
	void BuildDescriptorHeap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);


	virtual void OnResize(float aspectRatio, float newWidth, float newHeight);
	virtual void Update(float totalTime, float elapsedTime);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);
	void RenderSceneToShadowMap(ID3D12GraphicsCommandList* pd3dCommandList);

	void ChangeShader(ShaderType nShaderType, ID3D12GraphicsCommandList* pd3dCommandList);

	void UpdatePassCB(float totalTime, float elapsedTime);
	void UpdateShadowPassCB(float totalTime, float elapsedTime);

	void ProcessInput(UCHAR* pKeybuffer);
	void KeyDownEvent(WPARAM wParam);
	void KeyUpEvent(WPARAM wParam);
	void LeftButtonDownEvent();
	void RightButtonDownEvent();

	void LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName);

	static std::shared_ptr<Object> CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale, const char* pstrFileName, int nAnimationTracks);

	static std::shared_ptr<Object> CreateCuttedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		Object* pObject, float direction, XMFLOAT3 planeNormal, bool bIsCutted);

	void GenerateContact();
	void ProcessPhysics(float elapsedTime);

	void ClearObjectLayer();

private:
	XMFLOAT4X4 m_xmf4x4ViewProj = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4ImgObjMat = MathHelper::identity4x4();

	// 프레임마다 넘겨줄 상수 버퍼
	std::unique_ptr<UploadBuffer<PassConstant>> m_pPassCB = nullptr;
	std::unique_ptr<UploadBuffer<PassConstant>> m_pShadowPassCB = nullptr;
	UINT m_nPassCBParameterIdx = 1;

	//std::shared_ptr<Player> m_pPlayer = nullptr;

	// 씬을 렌더링할 메인 카메라
	std::unique_ptr<Camera> m_pCamera = nullptr;

	// 오브젝트 객체들
	static std::vector<std::shared_ptr<Object>> m_vObjectLayer[(int)RenderLayer::Render_Count];
	
	// UI 표시에 사용될 ImageObject들
	std::unique_ptr<ImgObject> m_pPlayerHPBar = nullptr;
	std::unique_ptr<ImgObject> m_pEnemyHPBar = nullptr;
	std::unique_ptr<ImgObject> m_pPlayerAim = nullptr;

	float m_tmptmp = 100;

	// Text로 표시될 UI의 전체 총괄 객체
	std::shared_ptr<DWriteText> m_pTextUIs = nullptr;

	static std::unique_ptr<CollisionResolver> m_pCollisionResolver;
	static CollisionData m_CollisionData;

	POINT m_LastMousePos = { 0,0 };

	// ================================================================
	// 조명, 그림자 관련
	// ===============================================================

	XMFLOAT3 m_BaseLightDirections[3] = {
	//XMFLOAT3(-0.57735f, -0.57735f, -0.57735f),
	XMFLOAT3(0.0f, -1.0f, 0.01f),
	XMFLOAT3(-0.57735f, -0.57735f, 0.57735f),
	XMFLOAT3(0.0f, -0.707f, -0.707f)
	};

	XMFLOAT4X4 m_xmf4x4ShadowTransform = MathHelper::identity4x4();

	std::unique_ptr<DepthMap> m_ShadowMap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_DsvDescriptorHeap = nullptr;
	UINT m_ShadowMapHeapIndex = 0;
	UINT m_nDescTableParameterIdx = 4;

	float m_FadeInValue = 1.0f;

public:
	void SetViewProjMatrix(XMFLOAT4X4 viewProj) { m_xmf4x4ViewProj = viewProj; }

	static ID3D12Device* m_pd3dDevice;
	static ID3D12GraphicsCommandList* m_pd3dCommandList;

#if defined(_DEBUG) | defined(DEBUG)
public:

	float m_refCnt = 0;
	float m_DebugValue = 0;

	float m_tTime = 0;
#endif
};




#endif

