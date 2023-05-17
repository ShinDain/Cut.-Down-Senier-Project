#ifndef PLAYER_H

#define PLAYER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"
#include "Weapon.h"
#include "../../DirectXRendering/Header/AnimationController.h"


class Player : public Character
{
public:
	Player();
	Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Player(const Player& rhs) = delete;
	Player& operator=(const Player& rhs) = delete;
	virtual ~Player();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Update(float elapsedTime);

	virtual void Destroy();

public:
	virtual void ProcessInput(UCHAR* pKeybuffer);
	virtual void KeyDownEvent(WPARAM wParam);

	virtual void Move(DWORD dwDirection);
	virtual void Jump();
	
	virtual void Attack();
	virtual void OnHit();
	virtual void OnDeath();

protected:
	std::shared_ptr<Weapon> m_pWeapon = nullptr;

public:
	std::shared_ptr<Weapon> GetWeapon() { return m_pWeapon; }

	void SetWeapon(std::shared_ptr<Weapon> pWeapon) { m_pWeapon = pWeapon; }

};

class AttackCallbackHandler :public AnimationCallbackHandler
{
public:
	AttackCallbackHandler() {}
	virtual ~AttackCallbackHandler() {}

public:
	virtual void HandleCallback(void* pCallbackData, float TrackPosition);
};







#endif