#include "grfwr_blob.h"
#include <d3dcompiler.h>

void Blob::Load(std::wstring path)
{
	Graphouny::Framework::ThrowOnFailed(
		D3DReadFileToBlob(path.c_str(), &m_pBlob), 
		L"Failed to load blob '" + path + L"'");
}

void Blob::PasteAsShader(D3D12_SHADER_BYTECODE& p) const
{
	p = { reinterpret_cast<u8*>(m_pBlob->GetBufferPointer()), m_pBlob->GetBufferSize() };
}