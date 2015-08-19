#pragma once
#include <d3d12.h>
#include "..\..\gr_shared.h"

class InputStream;

class TextureBuilder
{
public:
	TextureBuilder(ID3D12Device* device, ID3D12DescriptorHeap* descHeap, ID3D12GraphicsCommandList* cmdList)
		: m_pDevice(device), m_pDescHeap(descHeap), m_pCmdList(cmdList){}

	void BuildFromDDS_Memory(pointer data, const D3D12_RESOURCE_DESC& desc, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture);
	void BuildFromDDS_Stream(std::ifstream& stream, u32 size, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture);
	void BuildFromDDS_File(const wchar_t* szwFileName, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture);
	void BuildFromDDS_File(const std::wstring& wsFileName, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture);

	void BuildFromDDS_Memory(pointer data, const D3D12_RESOURCE_DESC& desc, ID3D12Resource** outTexture, ID3D12Resource** outTmpTexture)
	{ BuildFromDDS_Memory(data, desc, *outTexture, *outTmpTexture); }
	void BuildFromDDS_Stream(std::ifstream& stream, u32 size, ID3D12Resource** outTexture, ID3D12Resource** outTmpTexture)
	{ BuildFromDDS_Stream(stream, size, *outTexture, *outTmpTexture); }
	void BuildFromDDS_File(const wchar_t* szwFileName, ID3D12Resource** outTexture, ID3D12Resource** outTmpTexture)
	{ BuildFromDDS_File(szwFileName, *outTexture, *outTmpTexture); }
	void BuildFromDDS_File(const std::wstring& wsFileName, ID3D12Resource** outTexture, ID3D12Resource** outTmpTexture)
	{ BuildFromDDS_File(wsFileName, *outTexture, *outTmpTexture); }
private:
	ID3D12Device* m_pDevice;
	ID3D12DescriptorHeap* m_pDescHeap;
	ID3D12GraphicsCommandList* m_pCmdList;
	void BuildFromDDS_Stream(InputStream* stream, u32 size, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture);
};