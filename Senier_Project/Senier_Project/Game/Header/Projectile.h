#ifndef PROJECTILE_H

#define PROJECTILE_H

#include "../../DirectXRendering/Header/Object.h"
#include "Item.h"

enum ProjectileProperty
{
	Projectile_Player,
	Projectile_Enemy,
	Projectile_Cnt
};

class Projectile : public Item
{
public:
	Projectile();
	Projectile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Projectile(const Projectile& rhs) = delete;
	Projectile& operator=(const Projectile& rhs) = delete;
	virtual ~Projectile();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Animate(float elapsedTime);
	virtual void Update(float elapsedTime);

	virtual void Intersect(float elapsedTime);

	void ChasingPlayer(float elapsedTime);

protected:
	ProjectileProperty m_ProjectileProperty = ProjectileProperty::Projectile_Cnt;

	float m_ProjectileLifeTime = 10.0f;
	float m_ElapsedLifeTime = 0.0f;

	bool m_bChasePlayer = false;

	float m_ProjectilePower = 10.f;
	float m_ProjectileSpeed = 10.0f;
	XMFLOAT3 m_xmf3ProjectileDirection = XMFLOAT3(0, 0, 0);

public:
	ProjectileProperty GetProjectileProperty() { return m_ProjectileProperty; }
	bool GetChasePlayer() { return m_bChasePlayer; }
	float GetProjectileSpeed() { return m_ProjectileSpeed; }
	XMFLOAT3 GetProjectileDirection() { return m_xmf3ProjectileDirection; }

	void SetProjectileProperty(ProjectileProperty nProperty) { m_ProjectileProperty = nProperty; }
	void SetChasePlayer(bool bChasePlayer) { m_bChasePlayer = bChasePlayer; }
	void SetProjectilePower(float projectilePower) { m_ProjectilePower = projectilePower; }
	void SetProjectileSpeed(float projectileSpeed) { m_ProjectileSpeed = projectileSpeed; }
	void SetProjectileDirection(XMFLOAT3 xmf3ProjectileDirection) { m_xmf3ProjectileDirection = xmf3ProjectileDirection; }

};


#endif

