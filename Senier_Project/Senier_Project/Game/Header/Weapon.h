#ifndef WEAPON_H

#define WEAPON_H

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Object.h"



class Weapon : public Object
{
public:
	Weapon() = delete;
	Weapon(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData, char* pstrFollowObject, std::shared_ptr<Object> pFollowObject,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Weapon(const Weapon& rhs) = delete;
	Weapon& operator=(const Weapon& rhs) = delete;
	virtual ~Weapon();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData, 
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Animate(float elapsedTime) {};
	virtual void Update(float elapsedTime);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Destroy();

protected:
	bool m_bActive = false;

	char m_FollowObjName[64];
	std::shared_ptr<Object> m_pFollowObject = nullptr;

public:
	void Active();



public:
	bool GetActive() { return m_bActive = false; }


	void SetActive(bool bActive) {
		m_bActive = bActive;
		m_pCollider->SetIsActive(bActive);
	}
	void SetFollowObject(std::shared_ptr<Object> pFollowObject) { m_pFollowObject = pFollowObject; }

};





#endif