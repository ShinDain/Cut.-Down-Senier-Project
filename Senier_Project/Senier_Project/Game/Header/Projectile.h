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

protected:
	ProjectileProperty m_ProjectileProperty = ProjectileProperty::Projectile_Cnt;

public:
	ProjectileProperty GetProjectileProperty() { return m_ProjectileProperty; }
	
	void SetProjectileProperty(ProjectileProperty nProperty) { m_ProjectileProperty = nProperty; }

};


#endif

