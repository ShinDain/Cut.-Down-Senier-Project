#ifndef ITEM_H

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Global.h"
#include "../../DirectXRendering/Header/Object.h"
#include "../../Game/Header/Player.h"

#define ITEM_H

class Item : public Object
{
public:
	Item();
	Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Item(const Item& rhs) = delete;
	Item& operator=(const Item& rhs) = delete;
	virtual ~Item();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Animate(float elapsedTime);
	virtual void Update(float elapsedTime);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void Intersect(float elapsedTime);
	void TracePlayer(float elapsedTime);

public:
	enum ItemType
	{
		Score_1,
		Score_2,
		Score_3,
		Score_4,
		Score_5,
		Heal_1
	};

protected:
	BoundingSphere m_IntersectCollider;
	BoundingSphere m_TraceCollider;
	float m_IntersectColliderRadius = 1.0f;
	float m_TraceColliderRadius = 30.0f;

	bool m_bIsActive = false;
	bool m_bTrace = false;

	float m_ActiveTime = 1.0f;
	float m_ElapsedActiveTime = 0.0f;

	float m_Speed = 100.0f;

	UINT m_nItemType = 0;

private:
	XMFLOAT3 m_xmf3StartPosition = XMFLOAT3(0, 0, 0);

public:
	void SetItemType(UINT nItemType) { m_nItemType = nItemType; }

};




#endif