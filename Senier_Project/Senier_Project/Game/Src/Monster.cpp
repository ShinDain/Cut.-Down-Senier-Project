#include "../Header/Monster.h"

Monster::Monster()
{
}

Monster::Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
				 ObjectInitData objData,
				 std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Monster::~Monster()
{
	Destroy();
}

void Monster::Update(float elapsedTime)
{
}

void Monster::Destroy()
{
	Character::Destroy();
}

void Monster::Move(DWORD dwDirection)
{
}

void Monster::Attack()
{
}

void Monster::OnHit()
{
}

void Monster::OnDeath()
{
}
