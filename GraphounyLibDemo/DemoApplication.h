#pragma once
#include <Framework/grfw_shared.h>
#include <Framework/grfw_basicapplication.h>
#include <Framework/Render/grfwr_buffer_vertex.h>
#include <Framework/Render/grfwr_vertex_types.h>

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
	VertexBuffer<vertex_colored_s> m_bufTriangle;
};