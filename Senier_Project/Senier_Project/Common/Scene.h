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

	// virtual void CreateRtvAndDsvDescriptorHeap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	std::vector<std::unique_ptr<Shader>> mShaders;
	std::unique_ptr<UploadBuffer<tmpPassConstant>> mPassCB = nullptr;

	XMFLOAT4X4 mViewProj = MathHelper::identity4x4();


	std::unique_ptr<Object> mObj = nullptr;

public:
	void SetViewProjMatrix(XMFLOAT4X4 viewProj) { mViewProj = viewProj; }
};






