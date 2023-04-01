#pragma once

#include "Global.h"

struct TextUI
{
	std::wstring strText;
	float PosX;
	float PosY;

	TextUI(const wchar_t* pstr, float posX, float posY) : strText(pstr), PosX(posX), PosY(posY) {}
};


class DWriteText
{
public:
	DWriteText();
	DWriteText(const DWriteText& rhs) = delete;
	DWriteText& operator=(const DWriteText& rhs) = delete;
	virtual ~DWriteText();

public:
	bool Initialize(ID2D1DeviceContext* pd2dDeviceContext, IDWriteFactory* pdwFactory, UINT nFontSize, D2D1::ColorF fontColor);
	bool CreateTextFormat(ID2D1DeviceContext* pd2dDeviceContext, IDWriteFactory* pdwFactory);

	void AddTextUI(const wchar_t* pstrText, float PosX, float PosY);

	virtual void Render(ID3D11On12Device* pd3d11On12Device, ID2D1Bitmap1* pd2dBackBuffer, ID2D1DeviceContext* pd2dDeviceContext,
		ID3D11DeviceContext* pd3d11DeviceContext, ID3D11Resource* pWrappedBuffer);

protected:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_SolidColorBrush = nullptr;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_TextFormat = nullptr;

	UINT m_nFontSize = 1;
	D2D1::ColorF m_FontColor = D2D1::ColorF::Black;

	// 출력할 폰트를 저장하는 vector
	std::vector<TextUI> m_vTextUIs;

};
