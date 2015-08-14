#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "../grfwu_exception.h"

class Blob
{
public:
	void Load(std::wstring path);

	inline ID3DBlob* GetBlob() const { return m_pBlob.Get(); }
	void PasteAsShader(D3D12_SHADER_BYTECODE& sbc) const;
private:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBlob;
};

