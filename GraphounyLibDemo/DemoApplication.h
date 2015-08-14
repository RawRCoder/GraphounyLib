#pragma once
#include <Framework/grfw_shared.h>
#include <Framework/grfw_basicapplication.h>

class DemoApplication : public Graphouny::Framework::BasicApplication
{
protected:
	void Initialize() override;
	void LoadContent() override;
	void UnloadContent() override;
	void OnKeyPressed(Graphouny::Framework::KeyEventArgs args) override;
	void OnChar(wchar_t c) override;
	void Render(f32 delta) override;
public:
	DemoApplication();
	~DemoApplication();
private:
	std::wstring m_wsText = L"";
};

