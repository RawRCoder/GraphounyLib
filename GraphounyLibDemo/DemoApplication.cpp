#include "DemoApplication.h"
#include <Utils/gru_dictionary.h>
#include <Framework/grfwu_exception.h>
#include <Framework/grfwu_logging.h>
#include <Utils/gru_filesystem.h>

using namespace Graphouny::Framework;

DemoApplication::DemoApplication()
{
}

void DemoApplication::Initialize()
{
}

void DemoApplication::LoadContent()
{
}

DemoApplication::~DemoApplication()
{
}

void DemoApplication::UnloadContent()
{
}

void DemoApplication::OnKeyPressed(KeyEventArgs args)
{
	//ReportInfo(L"KeyPressed event");
}

void DemoApplication::OnChar(wchar_t c)
{
	if (c == L'\r' || c==L'\n')
	{
		ReportInfo(L"WString buffer test: " + m_wsText);
		m_wsText = L"";
		return;
	}
	m_wsText.append(&c, 1);
}

void DemoApplication::Render(f32 delta)
{
	static f32 h = 0.0f;
	h += delta*2;
	if (h >= 1) h = 0.0f;
	m_clrBackGround.r = (f32)Saturate(0, std::abs(h * 6 - 3.0) - 1.0, 1);
	m_clrBackGround.g = (f32)Saturate(0, 2.0f - std::abs(h * 6 - 2.0), 1);
	m_clrBackGround.b = (f32)Saturate(0, 2.0f - std::abs(h * 6 - 4.0), 1);
}