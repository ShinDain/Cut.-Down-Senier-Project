#pragma once

// 장면에 삽입될 오브젝트 초기화 및 관리

#include <algorithm>
#include "Camera.h"
#include "MathHelper.h"
#include "Global.h"
#include "Shader.h"
#include "Object.h"


class Scene
{
public:
	Scene();
	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;
	virtual ~Scene();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnResize(float aspectRatio);
	virtual void Update(const GameTimer& gt);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	void ProcessInput(UCHAR* pKeybuffer);

	std::shared_ptr<Object> CreateObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
										const char* pstrFileName, int nAnimationTracks, RenderLayer renderLayer);
private:

	XMFLOAT4X4 m_xmf4x4ViewProj = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4ImgObjMat = MathHelper::identity4x4();

	// 프레임마다 넘겨줄 상수 버퍼
	std::unique_ptr<UploadBuffer<PassConstant>> m_pPassCB = nullptr;

	// 오브젝트 객체들
	std::vector<std::shared_ptr<Object>> m_vpAllObjs;
	std::vector<std::shared_ptr<Object>> m_vObjectLayer[(int)RenderLayer::Count];

	// 씬을 렌더링할 메인 카메라
	std::unique_ptr<Camera> m_pCamera = nullptr;


	POINT m_LastMousePos = { 0,0 };
public:
	void SetViewProjMatrix(XMFLOAT4X4 viewProj) { m_xmf4x4ViewProj = viewProj; }


private:
	std::map<const char*, std::shared_ptr<ModelDataInfo>> m_LoadedModelData;


#if defined(_DEBUG) | defined(DEBUG)
public:

	int m_refCnt = 0;
	
#endif
};






