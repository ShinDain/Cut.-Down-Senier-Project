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
	bool Initialize(ID2D1DeviceContext* pd2dDeviceContext, IDWriteFactory* pdwFactory, float fontSize, D2D1::ColorF fontColor,
		DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, DWRITE_TEXT_ALIGNMENT textAlignment);
	bool CreateTextFormat(ID2D1DeviceContext* pd2dDeviceContext, IDWriteFactory* pdwFactory);

	void AddTextUI(const wchar_t* pstrText, float PosX, float PosY);
	bool UpdateTextUI(const wchar_t* pstrText, float posX, float posY, int nIndex);

	virtual void Render(ID3D11On12Device* pd3d11On12Device, ID2D1Bitmap1* pd2dBackBuffer, ID2D1DeviceContext* pd2dDeviceContext,
		ID3D11DeviceContext* pd3d11DeviceContext, ID3D11Resource* pWrappedBuffer);



protected:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_SolidColorBrush = nullptr;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_TextFormat = nullptr;

	float m_FontSize = 1;
	D2D1::ColorF m_FontColor = D2D1::ColorF::Black;

	DWRITE_FONT_WEIGHT m_FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	DWRITE_FONT_STYLE m_FontStyle = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_FONT_STRETCH m_FontStretch = DWRITE_FONT_STRETCH_NORMAL;

	DWRITE_TEXT_ALIGNMENT m_TextAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;

	// 출력할 폰트를 저장하는 vector
	std::vector<TextUI> m_vTextUIs;

	bool m_bVisible = true;
public:
	const float GetTextUIPosX(int nIndex) {
		return m_vTextUIs.at(nIndex).PosX;
	}
	const float GetTextUIPosY(int nIndex) {
		return m_vTextUIs.at(nIndex).PosY;
	}
	const std::wstring GetTextUIstrText(int nIndex) {
		return m_vTextUIs.at(nIndex).strText;
	}
	size_t GetTextUICount() { return m_vTextUIs.size(); }

	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
};
