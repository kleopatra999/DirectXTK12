//--------------------------------------------------------------------------------------
// File: WICTextureLoader.cpp
//
// Function for loading a WIC image and creating a Direct3D runtime texture for it
// (auto-generating mipmaps if possible)
//
// Note: Assumes application has already called CoInitializeEx
//
// Warning: CreateWICTexture* functions are not thread-safe if given a d3dContext instance for
//          auto-gen mipmap support.
//
// Note these functions are useful for images created as simple 2D textures. For
// more complex resources, DDSTextureLoader is an excellent light-weight runtime loader.
// For a full-featured DDS file reader, writer, and texture processing pipeline see
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

// We could load multi-frame images (TIFF/GIF) into a texture array.
// For now, we just load the first frame (note: DirectXTex supports multi-frame images)

#include "pch.h"

#include "WICTextureLoader.h"

#include "DirectXHelpers.h"
#include "PlatformHelpers.h"
#include "LoaderHelpers.h"
#include "ResourceUploadBatch.h"

namespace DirectX
{
    uint32_t CountMips(uint32_t width, uint32_t height);
}

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace
{
    //-------------------------------------------------------------------------------------
    // WIC Pixel Format Translation Data
    //-------------------------------------------------------------------------------------
    struct WICTranslate
    {
        GUID                wic;
        DXGI_FORMAT         format;
    };

    WICTranslate g_WICFormats[] =
    {
        { GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

        { GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
        { GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

        { GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
        { GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
        { GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1

        { GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
        { GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },

        { GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
        { GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

        { GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
        { GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
        { GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
        { GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

        { GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },

        { GUID_WICPixelFormat96bppRGBFloat,         DXGI_FORMAT_R32G32B32_FLOAT }, // WIC 2
    };

    //-------------------------------------------------------------------------------------
    // WIC Pixel Format nearest conversion table
    //-------------------------------------------------------------------------------------

    struct WICConvert
    {
        GUID        source;
        GUID        target;
    };

    WICConvert g_WICConvert[] =
    {
        // Note target GUID in this conversion table must be one of those directly supported formats (above).

        { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

        { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

        { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
        { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 

        { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
        { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 

        { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

        { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

        { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

        { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

        { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 

        { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 

        { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

        { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM (WIC2)
        { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM (WIC2)
        { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT (WIC2)

        { GUID_WICPixelFormat96bppRGBFixedPoint,   GUID_WICPixelFormat96bppRGBFloat }, // DXGI_FORMAT_R32G32B32_FLOAT (WIC2)

        // We don't support n-channel formats
    };
}

//--------------------------------------------------------------------------------------
namespace DirectX
{
    IWICImagingFactory2* _GetWIC()
    {
        static IWICImagingFactory2* s_Factory = nullptr;

        if (s_Factory)
            return s_Factory;

        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&s_Factory));
        if (FAILED(hr))
        {
            s_Factory = nullptr;
            return nullptr;
        }
        return s_Factory;
    }

} // namespace DirectX


namespace
{
    //---------------------------------------------------------------------------------
    DXGI_FORMAT _WICToDXGI(const GUID& guid)
    {
        for (size_t i = 0; i < _countof(g_WICFormats); ++i)
        {
            if (memcmp(&g_WICFormats[i].wic, &guid, sizeof(GUID)) == 0)
                return g_WICFormats[i].format;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    //---------------------------------------------------------------------------------
    size_t _WICBitsPerPixel(REFGUID targetGuid)
    {
        auto pWIC = _GetWIC();
        if (!pWIC)
            return 0;

        ComPtr<IWICComponentInfo> cinfo;
        if (FAILED(pWIC->CreateComponentInfo(targetGuid, cinfo.GetAddressOf())))
            return 0;

        WICComponentType type;
        if (FAILED(cinfo->GetComponentType(&type)))
            return 0;

        if (type != WICPixelFormat)
            return 0;

        ComPtr<IWICPixelFormatInfo> pfinfo;
        if (FAILED(cinfo.As(&pfinfo)))
            return 0;

        UINT bpp;
        if (FAILED(pfinfo->GetBitsPerPixel(&bpp)))
            return 0;

        return bpp;
    }

    //---------------------------------------------------------------------------------
    HRESULT CreateTextureFromWIC(_In_ ID3D12Device* d3dDevice,
        ResourceUploadBatch& resourceUpload,
        _In_ IWICBitmapFrameDecode *frame,
        size_t maxsize,
        D3D12_RESOURCE_FLAGS flags,
        bool forceSRGB,
        bool generateMips,
        _Outptr_ ID3D12Resource** texture)
    {
        UINT width, height;
        HRESULT hr = frame->GetSize(&width, &height);
        if (FAILED(hr))
            return hr;

        assert(width > 0 && height > 0);

        if (!maxsize)
        {
            maxsize = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
        }

        assert(maxsize > 0);

        UINT twidth, theight;
        if (width > maxsize || height > maxsize)
        {
            float ar = static_cast<float>(height) / static_cast<float>(width);
            if (width > height)
            {
                twidth = static_cast<UINT>(maxsize);
                theight = static_cast<UINT>(static_cast<float>(maxsize) * ar);
            }
            else
            {
                theight = static_cast<UINT>(maxsize);
                twidth = static_cast<UINT>(static_cast<float>(maxsize) / ar);
            }
            assert(twidth <= maxsize && theight <= maxsize);
        }
        else
        {
            twidth = width;
            theight = height;
        }

        // Determine format
        WICPixelFormatGUID pixelFormat;
        hr = frame->GetPixelFormat(&pixelFormat);
        if (FAILED(hr))
            return hr;

        WICPixelFormatGUID convertGUID;
        memcpy(&convertGUID, &pixelFormat, sizeof(WICPixelFormatGUID));

        size_t bpp = 0;

        DXGI_FORMAT format = _WICToDXGI(pixelFormat);
        if (format == DXGI_FORMAT_UNKNOWN)
        {
            for (size_t i = 0; i < _countof(g_WICConvert); ++i)
            {
                if (memcmp(&g_WICConvert[i].source, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
                {
                    memcpy(&convertGUID, &g_WICConvert[i].target, sizeof(WICPixelFormatGUID));

                    format = _WICToDXGI(g_WICConvert[i].target);
                    assert(format != DXGI_FORMAT_UNKNOWN);
                    bpp = _WICBitsPerPixel(convertGUID);
                    break;
                }
            }

            if (format == DXGI_FORMAT_UNKNOWN)
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }
        else
        {
            bpp = _WICBitsPerPixel(pixelFormat);
        }

        if (!bpp)
            return E_FAIL;

        // Handle sRGB formats
        if (forceSRGB)
        {
            format = MakeSRGB(format);
        }
        else
        {
            ComPtr<IWICMetadataQueryReader> metareader;
            if (SUCCEEDED(frame->GetMetadataQueryReader(metareader.GetAddressOf())))
            {
                GUID containerFormat;
                if (SUCCEEDED(metareader->GetContainerFormat(&containerFormat)))
                {
                    // Check for sRGB colorspace metadata
                    bool sRGB = false;

                    PROPVARIANT value;
                    PropVariantInit(&value);

                    if (memcmp(&containerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
                    {
                        // Check for sRGB chunk
                        if (SUCCEEDED(metareader->GetMetadataByName(L"/sRGB/RenderingIntent", &value)) && value.vt == VT_UI1)
                        {
                            sRGB = true;
                        }
                    }
#if defined(_XBOX_ONE) && defined(_TITLE)
                    else if (memcmp(&containerFormat, &GUID_ContainerFormatJpeg, sizeof(GUID)) == 0)
                    {
                        if (SUCCEEDED(metareader->GetMetadataByName(L"/app1/ifd/exif/{ushort=40961}", &value)) && value.vt == VT_UI2 && value.uiVal == 1)
                        {
                            sRGB = true;
                        }
                    }
                    else if (memcmp(&containerFormat, &GUID_ContainerFormatTiff, sizeof(GUID)) == 0)
                    {
                        if (SUCCEEDED(metareader->GetMetadataByName(L"/ifd/exif/{ushort=40961}", &value)) && value.vt == VT_UI2 && value.uiVal == 1)
                        {
                            sRGB = true;
                        }
                    }
#else
                    else if (SUCCEEDED(metareader->GetMetadataByName(L"System.Image.ColorSpace", &value)) && value.vt == VT_UI2 && value.uiVal == 1)
                    {
                        sRGB = true;
                    }
#endif

                    PropVariantClear(&value);

                    if (sRGB)
                        format = MakeSRGB(format);
                }
            }
        }

        // Verify our target format is supported by the current device
        // (handles WDDM 1.0 or WDDM 1.1 device driver cases as well as DirectX 11.0 Runtime without 16bpp format support)
        D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {};
        support.Format = format;
        hr = d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support));
        if (FAILED(hr) || !(support.Support1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D))
        {
            // Fallback to RGBA 32-bit format which is supported by all devices
            memcpy(&convertGUID, &GUID_WICPixelFormat32bppRGBA, sizeof(WICPixelFormatGUID));
            format = DXGI_FORMAT_R8G8B8A8_UNORM;
            bpp = 32;
        }

        // Allocate temporary memory for image
        size_t rowPitch = (twidth * bpp + 7) / 8;
        size_t imageSize = rowPitch * theight;

        std::unique_ptr<uint8_t[]> temp(new (std::nothrow) uint8_t[imageSize]);
        if (!temp)
            return E_OUTOFMEMORY;

        // Load image data
        if (memcmp(&convertGUID, &pixelFormat, sizeof(GUID)) == 0
            && twidth == width
            && theight == height)
        {
            // No format conversion or resize needed
            hr = frame->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
            if (FAILED(hr))
                return hr;
        }
        else if (twidth != width || theight != height)
        {
            // Resize
            auto pWIC = _GetWIC();
            if (!pWIC)
                return E_NOINTERFACE;

            ComPtr<IWICBitmapScaler> scaler;
            hr = pWIC->CreateBitmapScaler(scaler.GetAddressOf());
            if (FAILED(hr))
                return hr;

            hr = scaler->Initialize(frame, twidth, theight, WICBitmapInterpolationModeFant);
            if (FAILED(hr))
                return hr;

            WICPixelFormatGUID pfScaler;
            hr = scaler->GetPixelFormat(&pfScaler);
            if (FAILED(hr))
                return hr;

            if (memcmp(&convertGUID, &pfScaler, sizeof(GUID)) == 0)
            {
                // No format conversion needed
                hr = scaler->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
                if (FAILED(hr))
                    return hr;
            }
            else
            {
                ComPtr<IWICFormatConverter> FC;
                hr = pWIC->CreateFormatConverter(FC.GetAddressOf());
                if (FAILED(hr))
                    return hr;

                BOOL canConvert = FALSE;
                hr = FC->CanConvert(pfScaler, convertGUID, &canConvert);
                if (FAILED(hr) || !canConvert)
                {
                    return E_UNEXPECTED;
                }

                hr = FC->Initialize(scaler.Get(), convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
                if (FAILED(hr))
                    return hr;

                hr = FC->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
                if (FAILED(hr))
                    return hr;
            }
        }
        else
        {
            // Format conversion but no resize
            auto pWIC = _GetWIC();
            if (!pWIC)
                return E_NOINTERFACE;

            ComPtr<IWICFormatConverter> FC;
            hr = pWIC->CreateFormatConverter(FC.GetAddressOf());
            if (FAILED(hr))
                return hr;

            BOOL canConvert = FALSE;
            hr = FC->CanConvert(pixelFormat, convertGUID, &canConvert);
            if (FAILED(hr) || !canConvert)
            {
                return E_UNEXPECTED;
            }

            hr = FC->Initialize(frame, convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
            if (FAILED(hr))
                return hr;

            hr = FC->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
            if (FAILED(hr))
                return hr;
        }

        // Count the number of mips
        uint32_t mipCount = (generateMips) ? CountMips(twidth, theight) : 1;

        // Create texture
        D3D12_RESOURCE_DESC desc = {};
        desc.Width = twidth;
        desc.Height = theight;
        desc.MipLevels = (uint16_t)mipCount;
        desc.DepthOrArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Flags = flags;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        ID3D12Resource* tex = nullptr;
        hr = d3dDevice->CreateCommittedResource(
            &defaultHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_GRAPHICS_PPV_ARGS(&tex));

        if (FAILED(hr))
        {
            return hr;
        }

        _Analysis_assume_(tex != 0);

        // Upload the texture data
        D3D12_SUBRESOURCE_DATA initData;
        initData.pData = temp.get();
        initData.RowPitch = static_cast<UINT>(rowPitch);
        initData.SlicePitch = static_cast<UINT>(imageSize);

        resourceUpload.Upload(
            tex,
            0,
            &initData,
            1);

        resourceUpload.Transition(
            tex,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // Generate mips?
        if (generateMips)
        {
            resourceUpload.GenerateMips(tex);
        }

        *texture = tex;
        return hr;
    }
} // anonymous namespace

//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::CreateWICTextureFromMemory( ID3D12Device* d3dDevice,
                                             ResourceUploadBatch& resourceUpload,
                                             const uint8_t* wicData,
                                             size_t wicDataSize,
                                             ID3D12Resource** texture,
                                             bool generateMips,
                                             size_t maxsize )
{
    return CreateWICTextureFromMemoryEx( d3dDevice, resourceUpload, wicData, wicDataSize, maxsize,
                                         D3D12_RESOURCE_FLAG_NONE, false, generateMips,
                                         texture );
}


_Use_decl_annotations_
HRESULT DirectX::CreateWICTextureFromMemoryEx( ID3D12Device* d3dDevice,
                                               ResourceUploadBatch& resourceUpload,
                                               const uint8_t* wicData,
                                               size_t wicDataSize,
                                               size_t maxsize,
                                               D3D12_RESOURCE_FLAGS flags,
                                               bool forceSRGB,
                                               bool generateMips,
                                               ID3D12Resource** texture )
{
    if ( texture )
    {
        *texture = nullptr;
    }

    if (!d3dDevice || !wicData  || !texture)
        return E_INVALIDARG;

    if ( !wicDataSize )
        return E_FAIL;

#ifdef _M_AMD64
    if ( wicDataSize > 0xFFFFFFFF )
        return HRESULT_FROM_WIN32( ERROR_FILE_TOO_LARGE );
#endif

    auto pWIC = _GetWIC();
    if ( !pWIC )
        return E_NOINTERFACE;

    // Create input stream for memory
    ComPtr<IWICStream> stream;
    HRESULT hr = pWIC->CreateStream( stream.GetAddressOf() );
    if ( FAILED(hr) )
        return hr;

    hr = stream->InitializeFromMemory( const_cast<uint8_t*>( wicData ), static_cast<DWORD>( wicDataSize ) );
    if ( FAILED(hr) )
        return hr;

    // Initialize WIC
    ComPtr<IWICBitmapDecoder> decoder;
    hr = pWIC->CreateDecoderFromStream( stream.Get(), 0, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf() );
    if ( FAILED(hr) )
        return hr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame( 0, frame.GetAddressOf() );
    if ( FAILED(hr) )
        return hr;

    hr = CreateTextureFromWIC( d3dDevice, resourceUpload,
                               frame.Get(), maxsize,
                               flags, forceSRGB,
                               generateMips, texture );
    if ( FAILED(hr)) 
        return hr;

    _Analysis_assume_(*texture != nullptr);
    SetDebugObjectName(*texture, L"WICTextureLoader");

    return hr;
}

//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::CreateWICTextureFromFile( ID3D12Device* d3dDevice,
                                           ResourceUploadBatch& resourceUpload,
                                           const wchar_t* fileName,
                                           ID3D12Resource** texture,
                                           bool generateMips,
                                           size_t maxsize )
{
    return CreateWICTextureFromFileEx( d3dDevice, resourceUpload, fileName, maxsize,
                                       D3D12_RESOURCE_FLAG_NONE, false, generateMips,
                                       texture );
}


_Use_decl_annotations_
HRESULT DirectX::CreateWICTextureFromFileEx( ID3D12Device* d3dDevice,
                                             ResourceUploadBatch& resourceUpload,
                                             const wchar_t* fileName,
                                             size_t maxsize,
                                             D3D12_RESOURCE_FLAGS flags,
                                             bool forceSRGB,
                                             bool generateMips,
                                             ID3D12Resource** texture )
{
    if ( texture )
    {
        *texture = nullptr;
    }

    if (!d3dDevice || !fileName || !texture )
        return E_INVALIDARG;

    auto pWIC = _GetWIC();
    if ( !pWIC )
        return E_NOINTERFACE;

    // Initialize WIC
    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = pWIC->CreateDecoderFromFilename( fileName, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf() );
    if ( FAILED(hr) )
        return hr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame( 0, frame.GetAddressOf() );
    if ( FAILED(hr) )
        return hr;

    hr = CreateTextureFromWIC( d3dDevice, resourceUpload,
                               frame.Get(), maxsize,
                               flags, forceSRGB,
                               generateMips, texture );

#if !defined(NO_D3D11_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
    if ( SUCCEEDED(hr) )
    {
        const wchar_t* pstrName = wcsrchr(fileName, '\\' );
        if (!pstrName)
        {
            pstrName = fileName;
        }
        else
        {
            pstrName++;
        }

        if (texture != 0 && *texture != 0)
        {
            (*texture)->SetName(pstrName);
        }
    }
#endif

    return hr;
}

