#pragma once

// 장면에 삽입될 오브젝트 초기화 및 관리

#include "Camera.h"
#include "MathHelper.h"
#include "Global.h"
#include "Shader.h"
#include "Object.h"

// descriptorheap 정의
// rtv, dsv 생성

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

	void OnKeyboardInput(const GameTimer& gt) {}
	void OnWinKeyboardInput(WPARAM wParam);
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	// virtual void CreateRtvAndDsvDescriptorHeap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	std::vector<std::unique_ptr<Shader>> m_vpShaders;
	std::unique_ptr<UploadBuffer<tmpPassConstant>> m_pPassCB = nullptr;

	XMFLOAT4X4 m_xmf4x4ViewProj = MathHelper::identity4x4();


	std::vector<std::shared_ptr<Object>> m_vpObjs;
	std::unique_ptr<Camera> m_pCamera = nullptr;

	POINT m_LastMousePos = { 0,0 };

public:
	void SetViewProjMatrix(XMFLOAT4X4 viewProj) { m_xmf4x4ViewProj = viewProj; }
};






