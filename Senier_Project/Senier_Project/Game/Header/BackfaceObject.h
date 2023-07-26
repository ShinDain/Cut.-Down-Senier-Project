#ifndef BACKFACEOBJECT_H

#define BACKFACEOBJECT_H

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Global.h"
#include "../../DirectXRendering/Header/Object.h"
#include "../../Game/Header/Player.h"

class BackfaceObject : public Object
{
public:
	BackfaceObject();
	BackfaceObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	BackfaceObject(const BackfaceObject& rhs) = delete;
	virtual ~BackfaceObject();

protected:
	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Update(float elapsedTime);
	virtual void DestroyRunTime();

	//virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	//virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	std::shared_ptr<Object> m_pOriginalObject = nullptr;

public:
	void SetOriginalObject(std::shared_ptr<Object> pObject) {
		m_pOriginalObject = pObject;
	}
};


#endif