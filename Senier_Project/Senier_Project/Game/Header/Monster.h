#ifndef MONSTER_H

#define MONSTER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"

class Monster : public Character
{
public:
	Monster();
	Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Monster(const Monster& rhs) = delete;
	Monster& operator=(const Monster& rhs) = delete;
	virtual ~Monster();

	virtual void Update(float elapsedTime);

	virtual void Destroy();

public:
	virtual void ProcessInput(UCHAR* pKeybuffer) {}
	virtual void KeyDownEvent(WPARAM wParam) {}

	virtual void Move(DWORD dwDirection);
	virtual void Jump() {}

	virtual void Attack();
	virtual void OnHit();
	virtual void OnDeath();


};



#endif // !MONSTER_H
