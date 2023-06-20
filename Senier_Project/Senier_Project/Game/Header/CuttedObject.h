#ifndef CUTTEDOBJECT_H

#define CUTTEDOBJECT_H

#include "../../Common/Header/Struct.h"
#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Object.h"

#include "../../DirectXRendering//Header/Global.h"
#include "../../DirectXRendering/Header/Shader.h"

#define CUTTED_CB_STATIC_IDX 5
#define CUTTED_CB_TEXTURE_IDX 6
#define CUTTED_CB_SKINNED_IDX 8

class CuttedObject : public Object
{
public:
	CuttedObject();
	CuttedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,	std::shared_ptr<ModelDataInfo> pModel, UINT nPlaneCnt,
		float direction[], XMFLOAT3 planeNormal[], void* pContext);
	CuttedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, UINT nPlaneCnt,
		float direction[], XMFLOAT3 planeNormal[], UINT nAnimationSet, float trackPosition, void* pContext);

	CuttedObject(const CuttedObject& rhs) = delete;
	CuttedObject& operator=(const CuttedObject& rhs) = delete;
	virtual ~CuttedObject();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Update(float elapsedTime);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);
	
	virtual void Cutting(XMFLOAT3 xmf3PlaneNormal);

	void UpdateCuttedCB();

protected:
	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);

protected:
	std::unique_ptr<UploadBuffer<CuttedConstant>> m_pCuttedCB = nullptr;
	UINT m_CuttedCBByteSize = 0;

	UINT m_nPlaneCnt = 0;
	float m_PlaneDirection[3] = { 1,0,0 };
	XMFLOAT3 m_PlaneNormal[3] = { XMFLOAT3(1,0,0), XMFLOAT3(0,1,0) ,XMFLOAT3(0,0,1) };

	UINT m_CuttedCBIdx = CUTTED_CB_STATIC_IDX;

public:
	UINT GetPlaneCnt() { return m_nPlaneCnt; }
	float GetPlaneDirection(UINT idx) { return m_PlaneDirection[idx]; }
	XMFLOAT3 GetPlaneNormal(UINT idx) { return m_PlaneNormal[idx]; }


	void SetCuttedCBIdx(UINT nIdx) { m_CuttedCBIdx = nIdx; }

	void AddPlane(float direction, XMFLOAT3 planeNormal) { 
		if (m_nPlaneCnt >= 3)
			return;

		m_PlaneDirection[m_nPlaneCnt] = direction; 
		m_PlaneNormal[m_nPlaneCnt] = planeNormal;
		m_nPlaneCnt++;
	}
};


#endif