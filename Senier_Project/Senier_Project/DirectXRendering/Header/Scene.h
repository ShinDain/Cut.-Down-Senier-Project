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

#include "../../Game/Header/Cinematic.h"
#include "../../Game/Header/ObjectDefaultData.h"
#include "../../Game/Header/Character.h"
#include "../../Game/Header/Weapon.h"
#include "../../Game/Header/Player.h"
#include "../../Game/Header/Monster.h"
#include "../../Game/Header/Item.h"
#include "../../Game/header/CuttedObject.h"
#include "../../Game/Header/Projectile.h"
#include "../../Game/Header/Event.h"
#include "../../Game/Header/Sound.h"
#include "../../DataDefine/SoundFilePath.h"

#include "../../Physics/Header/Contact.h"
#include "../../Physics/Header/Collider.h"
#include "../../Physics/Header/CollisionDetact.h"
#include "../../Physics/Header/CollisionResolver.h"

// UI 표시 사이즈 
#define PLAYER_HP_BAR_WIDTH 256
#define PLAYER_HP_BAR_HEIGHT 24
#define ENEMY_HP_BAR_WIDTH 256
#define ENEMY_HP_BAR_HEIGHT 24
#define ENEMY_NAME_WIDTH 256
#define ENEMY_NAME_HEIGHT 48
#define PLAYER_AIM_WIDTH 32
#define PLAYER_AIM_HEIGHT 32

#define HP_BACK_WIDTH 400
#define HP_BACK_HEIGHT 40
#define SCORE_BACK_WIDTH 320
#define SCORE_BACK_HEIGHT 40

// Text UI Index
enum TextUIIdx
{
	Text_UI_Idx_HP,
	Text_UI_Idx_Score,
	Text_UI_Idx_Monster_Name,
	Text_UI_Idx_Loading
};

class Scene
{
public:
	Scene();
	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;
	virtual ~Scene();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<DWriteText> pDWriteText);

	bool InitUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<DWriteText> pDWriteText);
	bool InitCinematic();
	bool InitEvent(UINT nMapNum);

	void BuildDescriptorHeap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void StageStart(UINT nMapNum);

	virtual void OnResize(float aspectRatio, float newWidth, float newHeight);
	virtual void Update(float totalTime, float elapsedTime);
	void UpdateObject(float elapsedTime);
	void UpdatePlayerData(float elapsedTime);
	void UpdateSceneCamera(float elapsedTime);
	void UpdateUI(float elapsedTime);
	void UpdateSound();
	void UpdateEvent(float elapsedTime);
	void UpdateFadeInOut(float elapsedTime);
	void ChangeStage();

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

	void PlayCinematic(UINT nCinematicNum);
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
	std::shared_ptr<Camera> m_pCinematicCamera = nullptr;

	UINT m_nCurCinematicNum = 0;
	std::vector<std::shared_ptr<Cinematic>> m_vpCinematics;
	bool m_bInCinematic = false;

	// 오브젝트 객체들
	static std::vector<std::shared_ptr<Object>> m_vObjectLayer[(int)RenderLayer::Render_Count];
	
	// UI 표시에 사용될 ImageObject들
	std::unique_ptr<ImgObject> m_pPlayerHP_Bar = nullptr;
	std::unique_ptr<ImgObject> m_pPlayerHP_Frame = nullptr;
	std::unique_ptr<ImgObject> m_pEnemyHP_Bar = nullptr;
	std::unique_ptr<ImgObject> m_pEnemyHP_Frame = nullptr;
	std::unique_ptr<ImgObject> m_pPlayer_Aim = nullptr;
	std::unique_ptr<ImgObject> m_pEnemyName_Back = nullptr;
	std::unique_ptr<ImgObject> m_pHP_Back = nullptr;
	std::unique_ptr<ImgObject> m_pScore_Back = nullptr;


	// Text로 표시될 UI의 전체 총괄 객체
	std::shared_ptr<DWriteText> m_pTextUIs = nullptr;

	static UINT m_nStageNum;
	static std::unique_ptr<CollisionResolver> m_pCollisionResolver;
	static CollisionData m_CollisionData;

	POINT m_LastMousePos = { 0,0 };

	// ================================================================
	// 조명, 그림자 관련
	// ===============================================================

	XMFLOAT3 m_BaseLightDirections[3] = {
	XMFLOAT3(-0.57735f, -0.57735f, -0.57735f),
	XMFLOAT3(0.0f, -1.0f, 0.01f),
	XMFLOAT3(0.0f, -1.0f, 0.01f)
	};

	XMFLOAT4X4 m_xmf4x4ShadowTransform = MathHelper::identity4x4();

	std::unique_ptr<DepthMap> m_ShadowMap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_DsvDescriptorHeap = nullptr;
	UINT m_ShadowMapHeapIndex = 0;
	UINT m_nDescTableParameterIdx = 4;

	float m_FadeInValue = 1.0f;
	UINT m_FadeState = 2;		// 0 : fadeOut , 0 : fadeIn, 0 : normal
	float m_FadeTimer = 2.0f;
	bool m_bFadeTimer = false;
	float m_ElapsedFadeTimer = 0.0f;

	bool m_bNextStage = false;
	
public:
	void SetViewProjMatrix(XMFLOAT4X4 viewProj) { m_xmf4x4ViewProj = viewProj; }

// 사운드
public:
	static void EmitSound(const char* pstrFilePath, bool bLoop);
	static void EmitSound(const char* pstrFilePath, bool bLoop, float pitch, float volume);
	static void EmitHitSound(SoundType nType, bool bLoop);
	static void EmitCutSound(SoundType nType, bool bLoop);
	static void EmitBrokenSound(SoundType nType, bool bLoop);

protected:
	static std::shared_ptr<CSound> m_pMainBGM;
	static std::vector<std::shared_ptr<CSound>> m_vpSounds;


#if defined(_DEBUG) | defined(DEBUG)
public:

	float m_refCnt = 0;
	float m_DebugValue = 0;

	float m_tTime = 0;
#endif
};




#endif

