#ifndef CUTTEDOBJECT_H

#define CUTTEDOBJECT_H

#include "../../Common/Header/Struct.h"
#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Object.h"

#include "../../DirectXRendering//Header/Global.h"
#include "../../DirectXRendering/Header/Shader.h"

class CuttedStaticObject : public Object
{
public:
	CuttedStaticObject();
	CuttedStaticObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	CuttedStaticObject(const CuttedStaticObject& rhs) = delete;
	CuttedStaticObject& operator=(const CuttedStaticObject& rhs) = delete;
	virtual ~CuttedStaticObject();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Update(float elapsedTime);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	void UpdateCuttedCB();

protected:
	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);

protected:
	std::unique_ptr<UploadBuffer<CuttedConstant>> m_pCuttedCB = nullptr;
	UINT m_CuttedCBByteSize = 0;

	UINT m_nPlaneCnt = 0;
	float m_PlaneDirection[3] = { 1,0,0 };
	XMFLOAT3 m_PlaneNormal[3] = { XMFLOAT3(1,0,0), XMFLOAT3(0,1,0) ,XMFLOAT3(0,0,1) };
};


#endif