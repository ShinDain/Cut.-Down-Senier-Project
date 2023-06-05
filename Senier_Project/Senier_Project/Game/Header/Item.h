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
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	void Intersect(float elapsedTime);
	void TracePlayer(float elapsedTime);

	virtual void Destroy();

protected:
	BoundingSphere m_ItemCollider;
	BoundingSphere m_TraceCollider;
	float m_ItemColliderRadius = 1.0f;
	float m_TraceColliderRadius = 30.0f;

	bool m_bIsActive = false;
	bool m_bTrace = false;

	float m_ActiveTime = 0.5f;
	float m_ElapsedActiveTime = 0.0f;

	float m_PlayerTraceSpeed = 100.0f;

	UINT m_nItemType = 0;

};




#endif