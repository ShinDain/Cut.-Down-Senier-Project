#pragma once

#include "Header/DirectXApp.h"
#include "../Common/Header/MathHelper.h"
#include "Header/Scene.h"
#include "Header/DWriteText.h"
#include "../Game/Header/ObjectDefaultData.h"


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

protected:
	virtual void OnResize() override;
	virtual void Update(float elapsedTime) override;
	virtual void Render(float elapsedTime) override;

	virtual void ProcessInput() override;
	virtual void KeyDownEvent(WPARAM wParam) override;

private:

	std::unique_ptr<Scene> m_Scene;
	//std::unique_ptr<Player> m_Player;

#if defined(_DEBUG) | defined(DEBUG)
	std::unique_ptr<DWriteText> m_DebugText;

#endif

	POINT mLastMousePos = { 0,0 };
};
