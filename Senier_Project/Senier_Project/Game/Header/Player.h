#ifndef PLAYER_H

#define PLAYER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"
#include "Weapon.h"


class Player : public Character
{
public:
	Player();
	Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks);

	Player(const Player& rhs) = delete;
	Player& operator=(const Player& rhs) = delete;
	virtual ~Player();

	virtual void Update(float elapsedTime);

	virtual void Destroy();

protected:
	std::shared_ptr<Weapon> m_pWeapon = nullptr;

public:
	std::shared_ptr<Weapon> GetWeapon() { return m_pWeapon; }

	void SetWeapon(std::shared_ptr<Weapon> pWeapon) { m_pWeapon = pWeapon; }

};









#endif