#include "../../gr_fstream.h"
#include "../../gr_memstream.h"
#include "grfwr_texture_builder.h"
#include "..\..\Formats/DDS/grf_dds.h"
#include "..\grfwu_exception.h"
#include <algorithm>
#include "../../d3dx12.h"
#include <wrl/client.h>

using namespace Graphouny::Framework;
using namespace Graphouny::Formats;

void _buildTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* heap, 
	const D3D12_RESOURCE_DESC& desc, pointer data, ID3D12Resource*& outTexture, ID3D12Resource*& outTextureUpload)
{
	ThrowOnFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&outTexture)), L"Failed to create comitted resource [0]");

	const u64 uploadBufferSize = GetRequiredIntermediateSize(outTexture, 0, 1);

	// Create the GPU upload buffer.
	ThrowOnFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&outTextureUpload)), L"Failed to create comitted resource [1]");


	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = data;
	textureData.RowPitch = desc.Width * DDS::BitsPerPixel(desc.Format) / 8;
	textureData.SlicePitch = textureData.RowPitch * desc.Height;

	UpdateSubresources(cmdList, outTexture, outTextureUpload, 0, 0, 1, &textureData);
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(outTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// Describe and create a SRV for the texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(outTexture, &srvDesc, heap->GetCPUDescriptorHandleForHeapStart());
}

void _readDDS(InputStream* stream, DDS::DDS_HEADER& header, pointer& outData, u64& outDataSize, bool& dx10ext)
{
	auto fileSize = stream->GetLength();
	if (fileSize > 0xffffffff)
		throw GrBasicException(L"Texture`s too big");
	auto magic = stream->Read<u32>();
	if (magic != DDS::DDS_MAGIC)
		throw GrBasicException(L"Texture`s not a DDS file");

	if (!stream->Read<DDS::DDS_HEADER>(&header))
		throw GrBasicException(L"Texture`s an invalid DDS file - failed to read header [0]");

	if (header.size != sizeof(DDS::DDS_HEADER) || header.ddspf.size != sizeof(DDS::DDS_PIXELFORMAT))
		throw GrBasicException(L"Texture`s an invalid DDS file - failed to read header [1]");

	// Check for DX10 extension
	if ((header.ddspf.flags & DDS_FOURCC) && (MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourCC))
	{
		// Must be long enough for both headers and magic value
		if (fileSize < (stream->GetPosition() + sizeof(DDS::DDS_HEADER_DXT10)))
			throw GrBasicException(L"Texture`s an invalid DDS file - failed to read header [2]");
		dx10ext = true;
	}
	else dx10ext = false;

	auto dataOffset = stream->GetPosition() + (dx10ext ? sizeof(DDS::DDS_HEADER_DXT10) : 0);
	outDataSize = fileSize - dataOffset;
	outData = new u8[outDataSize];
	if (stream->ReadBytes(outData, outDataSize) != outDataSize)
		throw GrBasicException(L"Texture`s an invalid DDS file - unexpected end of file");
}

void _createTextureFromDDS(ID3D12Device* d3dDevice, ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* heap,
	const DDS::DDS_HEADER& header,
	const pointer bitData,
	u64 bitSize,
	D3D12_RESOURCE_FLAGS flags,
	ID3D12Resource*& texture,
	ID3D12Resource*& texture2)
{
	auto width = header.width;
	auto height = header.height;
	auto depth = header.depth;
	auto resDim = D3D12_RESOURCE_DIMENSION_UNKNOWN;
	u32 arraySize = 1;
	auto format = DXGI_FORMAT_UNKNOWN;
	bool isCubeMap = false;
	u32 mipCount = std::max<u32>(header.mipMapCount, 1);

	if ((header.ddspf.flags & DDS_FOURCC) && (MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourCC))
	{
		auto d3d10ext = reinterpret_cast<const DDS::DDS_HEADER_DXT10*>((const char*)&header + sizeof(DDS::DDS_HEADER));

		arraySize = d3d10ext->arraySize;
		if (arraySize == 0)
			throw GrBasicException(L"DDS: array size == 0");

		switch (d3d10ext->dxgiFormat)
		{
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P8:
		case DXGI_FORMAT_A8P8:
			throw GrBasicException(L"DDS: not supported format [0]");

		default:
			if (!DDS::BitsPerPixel(d3d10ext->dxgiFormat))
				throw GrBasicException(L"DDS: not supported format [1]");
		}

		format = d3d10ext->dxgiFormat;

		switch (d3d10ext->resourceDimension)
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if ((header.flags & DDS_HEIGHT) && height != 1)			
				throw GrBasicException(L"DDS: D3D12_RESOURCE_DIMENSION_TEXTURE1D assumes HEIGHT to be 1");
			
			height = depth = 1;
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (d3d10ext->miscFlag & 4/*D3D11_RESOURCE_MISC_TEXTURECUBE*/)
			{
				arraySize *= 6;
				isCubeMap = true;
			}
			depth = 1;
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			if (!(header.flags & DDS_HEADER_FLAGS_VOLUME))
				throw GrBasicException(L"DDS: Volume flag should be set for TEXTURE3D");

			if (arraySize > 1)
				throw GrBasicException(L"DDS: ArraySize shoulde be <= 1 for TEXTURE3D");
			break;

		default:
			throw GrBasicException(L"DDS: Speified D3D12_RESOURCE_DIMENSION is not supported");
		}

		resDim = static_cast<D3D12_RESOURCE_DIMENSION>(d3d10ext->resourceDimension);
	}
	else
	{
		format = GetDXGIFormat(header.ddspf);

		if (format == DXGI_FORMAT_UNKNOWN)
			throw GrBasicException(L"DDS: Unknown/unsupported DXGI format");

		if (header.flags & DDS_HEADER_FLAGS_VOLUME)
		{
			resDim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}
		else
		{
			if (header.caps2 & DDS_CUBEMAP)
			{
				// We require all six faces to be defined
				if ((header.caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
					throw GrBasicException(L"DDS: All 6 faces are required for cubemap");

				arraySize = 6;
				isCubeMap = true;
			}

			depth = 1;
			resDim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

			// Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
		}

		//assert(BitsPerPixel(format) != 0);
	}

	// Bound sizes (for security purposes we don't trust DDS file metadata larger than the D3D 11.x hardware requirements)
	if (mipCount > D3D12_REQ_MIP_LEVELS)
		throw GrBasicException(L"DDS: Too much mip levels");

	switch (resDim)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		if ((arraySize > D3D12_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION) || (width > D3D12_REQ_TEXTURE1D_U_DIMENSION))
			throw GrBasicException(L"DDS: Unsopported/Invalid dimensions [0]");
		break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		if (isCubeMap)
		{
			// This is the right bound because we set arraySize to (NumCubes*6) above
			if ((arraySize > D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
				(width > D3D12_REQ_TEXTURECUBE_DIMENSION) ||
				(height > D3D12_REQ_TEXTURECUBE_DIMENSION))
				throw GrBasicException(L"DDS: Unsopported/Invalid dimensions [1]");
		}
		else if ((arraySize > D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
			(width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION) ||
			(height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION))
			throw GrBasicException(L"DDS: Unsopported/Invalid dimensions [2]");
		break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		if ((arraySize > 1) ||
			(width > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
			(height > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
			(depth > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION))
			throw GrBasicException(L"DDS: Unsopported/Invalid dimensions [3]");
		break;

	default:
		throw GrBasicException(L"DDS: Unsopported/Invalid dimensions [4]");
	}

	// TODO: MIPMAP autogen
	// TODO: Max dimensions per feature level

	// Describe and create a Texture2D.
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = mipCount;
	textureDesc.Format = format;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Flags = flags;
	textureDesc.DepthOrArraySize = arraySize;
	textureDesc.SampleDesc.Count = 1;//?
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = resDim;

	_buildTexture(d3dDevice, cmdList, heap, textureDesc, bitData, texture, texture2);
	return;
}



void TextureBuilder::BuildFromDDS_Memory(pointer data, const D3D12_RESOURCE_DESC& desc, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture)
{
	_buildTexture(m_pDevice, m_pCmdList, m_pDescHeap, desc, data, outTexture, outTmpTexture);
}

void TextureBuilder::BuildFromDDS_Stream(std::ifstream& stream, u32 size, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture)
{
	FileInputStream s{ &stream };
	BuildFromDDS_Stream(&s, s.GetLength(), outTexture, outTmpTexture);
}

void TextureBuilder::BuildFromDDS_File(const wchar_t* szwFileName, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture)
{
	FileInputStream stream{ szwFileName };
	BuildFromDDS_Stream(&stream, stream.GetLength(), outTexture, outTmpTexture);
}

void TextureBuilder::BuildFromDDS_File(const std::wstring& wsFileName, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture)
{
	FileInputStream stream{ wsFileName };
	BuildFromDDS_Stream(&stream, stream.GetLength(), outTexture, outTmpTexture);
}

void TextureBuilder::BuildFromDDS_Stream(InputStream* stream, u32 size, ID3D12Resource*& outTexture, ID3D12Resource*& outTmpTexture)
{
	DDS::DDS_HEADER header;
	pointer textureData = nullptr;
	u64 textureDataSize = 0;
	auto dx10ext = false;
	_readDDS(stream, header, textureData, textureDataSize, dx10ext);
	_createTextureFromDDS(m_pDevice, m_pCmdList, m_pDescHeap, header, textureData, textureDataSize, D3D12_RESOURCE_FLAG_NONE, outTexture, outTmpTexture);
}