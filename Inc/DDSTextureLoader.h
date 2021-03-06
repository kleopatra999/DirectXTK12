//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.h
//
// Functions for loading a DDS texture and creating a Direct3D runtime resource for it
//
// Note these functions are useful as a light-weight runtime loader for DDS files. For
// a full-featured DDS file reader, writer, and texture processing pipeline see
// the 'Texconv' sample and the 'DirectXTex' library.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d12_x.h>
#else
#include <d3d12.h>
#endif

#include <memory>
#include <vector>
#include <stdint.h>


namespace DirectX
{
    class ResourceUploadBatch;

    enum DDS_ALPHA_MODE
    {
        DDS_ALPHA_MODE_UNKNOWN       = 0,
        DDS_ALPHA_MODE_STRAIGHT      = 1,
        DDS_ALPHA_MODE_PREMULTIPLIED = 2,
        DDS_ALPHA_MODE_OPAQUE        = 3,
        DDS_ALPHA_MODE_CUSTOM        = 4,
    };

    // Standard version
    HRESULT __cdecl LoadDDSTextureFromMemory(
        _In_ ID3D12Device* d3dDevice,
        _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
        size_t ddsDataSize,
        _Outptr_ ID3D12Resource** texture,
        _Inout_ std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
        size_t maxsize = 0,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);

    HRESULT __cdecl LoadDDSTextureFromFile(
        _In_ ID3D12Device* d3dDevice,
        _In_z_ const wchar_t* szFileName,
        _Outptr_ ID3D12Resource** texture,
        _Inout_ std::unique_ptr<uint8_t[]>& ddsData,
        _Inout_ std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
        size_t maxsize = 0,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);

    HRESULT __cdecl CreateDDSTextureFromMemory(
        _In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
        size_t ddsDataSize,
        _Outptr_ ID3D12Resource** texture,
        bool generateMipsIfMissing = false,
        size_t maxsize = 0,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);

    HRESULT __cdecl CreateDDSTextureFromFile(
        _In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        _In_z_ const wchar_t* szFileName,
        _Outptr_ ID3D12Resource** texture,
        bool generateMipsIfMissing = false,
        size_t maxsize = 0,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);

    // Extended version
    HRESULT __cdecl LoadDDSTextureFromMemoryEx(
        _In_ ID3D12Device* d3dDevice,
        _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
        size_t ddsDataSize,
        size_t maxsize,
        D3D12_RESOURCE_FLAGS flags,
        bool forceSRGB,
        bool reserveFullMipChain,
        _Outptr_ ID3D12Resource** texture,
        std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);

    HRESULT __cdecl LoadDDSTextureFromFileEx(
        _In_ ID3D12Device* d3dDevice,
        _In_z_ const wchar_t* szFileName,
        size_t maxsize,
        D3D12_RESOURCE_FLAGS flags,
        bool forceSRGB,
        bool reserveFullMipChain,
        _Outptr_ ID3D12Resource** texture,
        std::unique_ptr<uint8_t[]>& ddsData,
        std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);

    HRESULT __cdecl CreateDDSTextureFromMemoryEx(
        _In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
        size_t ddsDataSize,
        size_t maxsize,
        D3D12_RESOURCE_FLAGS flags,
        bool forceSRGB,
        bool generateMipsIfMissing,
        _Outptr_ ID3D12Resource** texture,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);

    HRESULT __cdecl CreateDDSTextureFromFileEx(
        _In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        _In_z_ const wchar_t* szFileName,
        size_t maxsize,
        D3D12_RESOURCE_FLAGS flags,
        bool forceSRGB,
        bool generateMipsIfMissing,
        _Outptr_ ID3D12Resource** texture,
        _Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr,
        _Out_opt_ bool* isCubeMap = nullptr);
}
