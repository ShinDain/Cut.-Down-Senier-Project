#ifndef EVENT_H

#define EVENT_H

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Global.h"
#include "../../DirectXRendering/Header/Scene.h"
#include "../../DirectXRendering/Header/Object.h"
#include "../../Game/Header/Player.h"

class CEvent : public Object
{
public:
	CEvent();
	CEvent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	CEvent(const CEvent& rhs) = delete;
	CEvent& operator=(const CEvent& rhs) = delete;
	virtual ~CEvent();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update(float elapsedTime);

	virtual void Intersect();

protected:
	bool m_bIsActive = false;
	bool m_bIntersect = false;

	std::shared_ptr<BoundingOrientedBox> m_pOBB = nullptr;

public:
	void SetIsActive(bool bIsActive) { m_bIsActive = bIsActive; }

	bool GetIntersect() { return m_bIntersect; }
};


#endif