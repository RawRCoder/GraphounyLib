#include "DemoApplication.h"
#include <d3dx12.h>
#include <Framework/Render/grfwr_blob.h>
#include <Framework/grfw_content.h>

using namespace Graphouny::Framework;

DemoApplication::DemoApplication()
{
}

void DemoApplication::Initialize()
{
}

void DemoApplication::LoadContent()
{
	// Load shaders
	Blob vs{}; vs.Load(GetContent()->GetPath_ShadersDir() / L"vs_straight.cso");
	Blob ps{}; ps.Load(GetContent()->GetPath_ShadersDir() / L"ps_straight.cso");

	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Define pipeline state description
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = GetRootSignature();
	vs.PasteAsShader(psoDesc.VS);
	ps.PasteAsShader(psoDesc.PS);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	// Create a pipeline
	CreatePipeLine(L"main", psoDesc);

	// Select it
	SetPipeLine(L"main");

	m_bufTriangle.Push({ { 0.0f, 0.25f, 0.0f }, RGBA_RED });
	m_bufTriangle.Push({ { 0.25f, -0.25f, 0.0f }, RGBA_GREEN });
	m_bufTriangle.Push({ { -0.25f, -0.25f, 0.0f }, RGBA_BLUE });

	m_bufTriangle.Push(0);
	m_bufTriangle.Push(1);
	m_bufTriangle.Push(2);

	m_bufTriangle.Build(GetDevice());
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
	m_clrBackGround.r = 0.1f;
	m_clrBackGround.g = 0.2f;
	m_clrBackGround.b = 0.3f;

	// Set necessary state.
	auto cmd = GetCommandList();
	cmd->SetGraphicsRootSignature(GetRootSignature());
	cmd->RSSetViewports(1, &GetViewPort());
	cmd->RSSetScissorRects(1, &GetScissorRect());
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Clear();
	
	// Draw stuff
	m_bufTriangle.Draw(cmd);
}