#include "DWriteText.h"

DWriteText::DWriteText()
{
}

DWriteText::~DWriteText()
{
}

bool DWriteText::Initialize(ID2D1DeviceContext* pd2dDeviceContext, IDWriteFactory* pdwFactory, UINT nFontSize, D2D1::ColorF fontColor)
{
	m_nFontSize = nFontSize;
	m_FontColor = fontColor;

	if (!CreateTextFormat(pd2dDeviceContext, pdwFactory))
		return false;

	return true;
}

bool DWriteText::CreateTextFormat(ID2D1DeviceContext* pd2dDeviceContext, IDWriteFactory* pdwFactory)
{
	ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(m_FontColor, m_SolidColorBrush.GetAddressOf()));
	ThrowIfFailed(pdwFactory->CreateTextFormat(L"Default", nullptr, 
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		m_nFontSize, L"en-us", m_TextFormat.GetAddressOf()));

	m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	return true;
}

void DWriteText::AddTextUI(const wchar_t* pstrText, float PosX, float PosY)
{
	// TextUI 구조체 생성 후 vector에 추가

	m_vTextUIs.emplace_back(pstrText, PosX, PosY);

}

void DWriteText::Render(ID3D11On12Device* pd3d11On12Device, ID2D1Bitmap1* pd2dBackBuffer, ID2D1DeviceContext* pd2dDeviceContext, ID3D11DeviceContext* pd3d11DeviceContext, ID3D11Resource* pWrappedBuffer)
{
	D2D1_SIZE_F rtSize = pd2dBackBuffer->GetSize();
	D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);

	pd3d11On12Device->AcquireWrappedResources(&pWrappedBuffer, 1);

	pd2dDeviceContext->SetTarget(pd2dBackBuffer);
	pd2dDeviceContext->BeginDraw();
	for (int i = 0; i < m_vTextUIs.size(); ++i)
	{
		D2D1::Matrix3x2F mat3x2Ftrans = D2D1::Matrix3x2F::Translation(m_vTextUIs[i].PosX, m_vTextUIs[i].PosY);

		pd2dDeviceContext->SetTransform(mat3x2Ftrans);
		pd2dDeviceContext->DrawTextW(m_vTextUIs[i].strText.c_str(), wcslen(m_vTextUIs[i].strText.c_str()),
			m_TextFormat.Get(), &textRect, m_SolidColorBrush.Get());
	}
	pd2dDeviceContext->EndDraw();

	// WrappedResource를 Release하며 state가 Present(설정한 state)로 변경된다.
	pd3d11On12Device->ReleaseWrappedResources(&pWrappedBuffer, 1);

	pd3d11DeviceContext->Flush();
}
