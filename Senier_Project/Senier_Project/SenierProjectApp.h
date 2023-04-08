#pragma once

#include "Common/DirectXApp.h"
#include "Common/MathHelper.h"
#include "Common/Scene.h"
#include "Common/DWriteText.h"


using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class SenierProjectApp : public DirectXApp
{
public:
	SenierProjectApp(HINSTANCE hInstance);
	SenierProjectApp(const SenierProjectApp& rhs) = delete;
	SenierProjectApp& operator=(const SenierProjectApp& rhs) = delete;
	~SenierProjectApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Render(const GameTimer& gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	virtual void ProcessInput() override;

private:

	std::unique_ptr<Scene> m_Scene;
	//std::unique_ptr<Player> m_Player;

#if defined(_DEBUG) | defined(DEBUG)
	std::unique_ptr<DWriteText> m_DebugText;

#endif

	POINT mLastMousePos = { 0,0 };
};
