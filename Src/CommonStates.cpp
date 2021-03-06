//--------------------------------------------------------------------------------------
// File: CommonStates.cpp
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

#include "pch.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"

using namespace DirectX;

// --------------------------------------------------------------------------
// Blend States
// --------------------------------------------------------------------------

const D3D12_BLEND_DESC CommonStates::Opaque =
{
    FALSE, // AlphaToCoverageEnable
    FALSE, // IndependentBlendEnable
    {
        FALSE, // BlendEnable
        FALSE, // LogicOpEnable
        D3D12_BLEND_ONE, // SrcBlend
        D3D12_BLEND_ZERO, // DestBlend
        D3D12_BLEND_OP_ADD, // BlendOp
        D3D12_BLEND_ONE, // SrcBlendAlpha
        D3D12_BLEND_ZERO, // DestBlendAlpha
        D3D12_BLEND_OP_ADD, // BlendOpAlpha
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
    }
};

const D3D12_BLEND_DESC CommonStates::AlphaBlend =
{
    FALSE, // AlphaToCoverageEnable
    FALSE, // IndependentBlendEnable
    {
        TRUE, // BlendEnable
        FALSE, // LogicOpEnable
        D3D12_BLEND_ONE, // SrcBlend
        D3D12_BLEND_INV_SRC_ALPHA, // DestBlend
        D3D12_BLEND_OP_ADD, // BlendOp
        D3D12_BLEND_ONE, // SrcBlendAlpha
        D3D12_BLEND_INV_SRC_ALPHA, // DestBlendAlpha
        D3D12_BLEND_OP_ADD, // BlendOpAlpha
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
    }
};

const D3D12_BLEND_DESC CommonStates::Additive =
{
    FALSE, // AlphaToCoverageEnable
    FALSE, // IndependentBlendEnable
    {
        TRUE, // BlendEnable
        FALSE, // LogicOpEnable
        D3D12_BLEND_SRC_ALPHA, // SrcBlend
        D3D12_BLEND_ONE, // DestBlend
        D3D12_BLEND_OP_ADD, // BlendOp
        D3D12_BLEND_SRC_ALPHA, // SrcBlendAlpha
        D3D12_BLEND_ONE, // DestBlendAlpha
        D3D12_BLEND_OP_ADD, // BlendOpAlpha
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
    }
};

const D3D12_BLEND_DESC CommonStates::NonPremultiplied =
{
    FALSE, // AlphaToCoverageEnable
    FALSE, // IndependentBlendEnable
    {
        TRUE, // BlendEnable
        FALSE, // LogicOpEnable
        D3D12_BLEND_SRC_ALPHA, // SrcBlend
        D3D12_BLEND_INV_SRC_ALPHA, // DestBlend
        D3D12_BLEND_OP_ADD, // BlendOp
        D3D12_BLEND_SRC_ALPHA, // SrcBlendAlpha
        D3D12_BLEND_INV_SRC_ALPHA, // DestBlendAlpha
        D3D12_BLEND_OP_ADD, // BlendOpAlpha
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
    }
};


// --------------------------------------------------------------------------
// Depth-Stencil States
// --------------------------------------------------------------------------

const D3D12_DEPTH_STENCIL_DESC CommonStates::DepthNone =
{
    FALSE, // DepthEnable
    D3D12_DEPTH_WRITE_MASK_ZERO,
    D3D12_COMPARISON_FUNC_LESS_EQUAL, // DepthFunc
    FALSE, // StencilEnable
    D3D12_DEFAULT_STENCIL_READ_MASK,
    D3D12_DEFAULT_STENCIL_WRITE_MASK,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    }, // FrontFace,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    } // BackFace
};

const D3D12_DEPTH_STENCIL_DESC CommonStates::DepthDefault =
{
    TRUE, // DepthEnable
    D3D12_DEPTH_WRITE_MASK_ALL,
    D3D12_COMPARISON_FUNC_LESS_EQUAL, // DepthFunc
    FALSE, // StencilEnable
    D3D12_DEFAULT_STENCIL_READ_MASK,
    D3D12_DEFAULT_STENCIL_WRITE_MASK,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    }, // FrontFace,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    } // BackFace
};

const D3D12_DEPTH_STENCIL_DESC CommonStates::DepthRead =
{
    TRUE, // DepthEnable
    D3D12_DEPTH_WRITE_MASK_ZERO,
    D3D12_COMPARISON_FUNC_LESS_EQUAL, // DepthFunc
    FALSE, // StencilEnable
    D3D12_DEFAULT_STENCIL_READ_MASK,
    D3D12_DEFAULT_STENCIL_WRITE_MASK,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    }, // FrontFace,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    } // BackFace
};


// --------------------------------------------------------------------------
// Rasterizer States
// --------------------------------------------------------------------------

const D3D12_RASTERIZER_DESC CommonStates::CullNone =
{
    D3D12_FILL_MODE_SOLID,
    D3D12_CULL_MODE_NONE,
    FALSE, // FrontCounterClockwise
    D3D12_DEFAULT_DEPTH_BIAS,
    D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE, // DepthClipEnable
    TRUE, // MultisampleEnable
    FALSE, // AntialiasedLineEnable
    0, // ForcedSampleCount
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
};

const D3D12_RASTERIZER_DESC CommonStates::CullClockwise =
{
    D3D12_FILL_MODE_SOLID,
    D3D12_CULL_MODE_FRONT,
    FALSE, // FrontCounterClockwise
    D3D12_DEFAULT_DEPTH_BIAS,
    D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE, // DepthClipEnable
    TRUE, // MultisampleEnable
    FALSE, // AntialiasedLineEnable
    0, // ForcedSampleCount
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
};

const D3D12_RASTERIZER_DESC CommonStates::CullCounterClockwise =
{
    D3D12_FILL_MODE_SOLID,
    D3D12_CULL_MODE_BACK,
    FALSE, // FrontCounterClockwise
    D3D12_DEFAULT_DEPTH_BIAS,
    D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE, // DepthClipEnable
    TRUE, // MultisampleEnable
    FALSE, // AntialiasedLineEnable
    0, // ForcedSampleCount
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
};

const D3D12_RASTERIZER_DESC CommonStates::Wireframe =
{
    D3D12_FILL_MODE_WIREFRAME,
    D3D12_CULL_MODE_NONE,
    FALSE, // FrontCounterClockwise
    D3D12_DEFAULT_DEPTH_BIAS,
    D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE, // DepthClipEnable
    TRUE, // MultisampleEnable
    FALSE, // AntialiasedLineEnable
    0, // ForcedSampleCount
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
};


// --------------------------------------------------------------------------
// Sampler States
// --------------------------------------------------------------------------

const D3D12_SAMPLER_DESC CommonStates::PointWrap = 
{
    D3D12_FILTER_MIN_MAG_MIP_POINT,
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
    0, // MipLODBias
    D3D12_MAX_MAXANISOTROPY,
    D3D12_COMPARISON_FUNC_NEVER,
    { 0, 0, 0, 0 }, // BorderColor
    0, // MinLOD
    FLT_MAX // MaxLOD
};

const D3D12_SAMPLER_DESC CommonStates::PointClamp =
{
    D3D12_FILTER_MIN_MAG_MIP_POINT,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
    0, // MipLODBias
    D3D12_MAX_MAXANISOTROPY,
    D3D12_COMPARISON_FUNC_NEVER,
    { 0, 0, 0, 0 }, // BorderColor
    0, // MinLOD
    FLT_MAX // MaxLOD
};

const D3D12_SAMPLER_DESC CommonStates::LinearWrap = 
{
    D3D12_FILTER_MIN_MAG_MIP_LINEAR,
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
    0, // MipLODBias
    D3D12_MAX_MAXANISOTROPY,
    D3D12_COMPARISON_FUNC_NEVER,
    { 0, 0, 0, 0 }, // BorderColor
    0, // MinLOD
    FLT_MAX // MaxLOD
};

const D3D12_SAMPLER_DESC CommonStates::LinearClamp =
{
    D3D12_FILTER_MIN_MAG_MIP_LINEAR,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
    0, // MipLODBias
    D3D12_MAX_MAXANISOTROPY,
    D3D12_COMPARISON_FUNC_NEVER,
    { 0, 0, 0, 0 }, // BorderColor
    0, // MinLOD
    FLT_MAX // MaxLOD
};

const D3D12_SAMPLER_DESC CommonStates::AnisotropicWrap = 
{
    D3D12_FILTER_ANISOTROPIC,
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
    D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
    0, // MipLODBias
    D3D12_MAX_MAXANISOTROPY,
    D3D12_COMPARISON_FUNC_NEVER,
    { 0, 0, 0, 0 }, // BorderColor
    0, // MinLOD
    FLT_MAX // MaxLOD
};

const D3D12_SAMPLER_DESC CommonStates::AnisotropicClamp =
{
    D3D12_FILTER_ANISOTROPIC,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
    0, // MipLODBias
    D3D12_MAX_MAXANISOTROPY,
    D3D12_COMPARISON_FUNC_NEVER,
    { 0, 0, 0, 0 }, // BorderColor
    0, // MinLOD
    FLT_MAX // MaxLOD
};


// --------------------------------------------------------------------------
// Static sampler States
// --------------------------------------------------------------------------

const D3D12_STATIC_SAMPLER_DESC CommonStates::StaticPointWrap(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility, unsigned int registerSpace)
{
    static const D3D12_STATIC_SAMPLER_DESC s_desc = {
        D3D12_FILTER_MIN_MAG_MIP_POINT,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
        0, // MipLODBias
        D3D12_MAX_MAXANISOTROPY,
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
        0, // MinLOD
        FLT_MAX, // MaxLOD
        0, // ShaderRegister
        0, // RegisterSpace
        D3D12_SHADER_VISIBILITY_ALL,
    };

    D3D12_STATIC_SAMPLER_DESC desc = s_desc;
    desc.ShaderRegister = shaderRegister;
    desc.ShaderVisibility = shaderVisibility;
    desc.RegisterSpace = registerSpace;
    return desc;
}

const D3D12_STATIC_SAMPLER_DESC CommonStates::StaticPointClamp(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility, unsigned int registerSpace)
{
    static const D3D12_STATIC_SAMPLER_DESC s_desc = {
        D3D12_FILTER_MIN_MAG_MIP_POINT,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
        0, // MipLODBias
        D3D12_MAX_MAXANISOTROPY,
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
        0, // MinLOD
        FLT_MAX, // MaxLOD
        0, // ShaderRegister
        0, // RegisterSpace
        D3D12_SHADER_VISIBILITY_ALL,
    };

    D3D12_STATIC_SAMPLER_DESC desc = s_desc;
    desc.ShaderRegister = shaderRegister;
    desc.ShaderVisibility = shaderVisibility;
    desc.RegisterSpace = registerSpace;
    return desc;
};
const D3D12_STATIC_SAMPLER_DESC CommonStates::StaticLinearWrap(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility, unsigned int registerSpace)
{
    static const D3D12_STATIC_SAMPLER_DESC s_desc = {
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
        0, // MipLODBias
        D3D12_MAX_MAXANISOTROPY,
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
        0, // MinLOD
        FLT_MAX, // MaxLOD
        0, // ShaderRegister
        0, // RegisterSpace
        D3D12_SHADER_VISIBILITY_ALL,
    };

    D3D12_STATIC_SAMPLER_DESC desc = s_desc;
    desc.ShaderRegister = shaderRegister;
    desc.ShaderVisibility = shaderVisibility;
    desc.RegisterSpace = registerSpace;
    return desc;
};

const D3D12_STATIC_SAMPLER_DESC CommonStates::StaticLinearClamp(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility, unsigned int registerSpace)
{
    static const D3D12_STATIC_SAMPLER_DESC s_desc = {
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
        0, // MipLODBias
        D3D12_MAX_MAXANISOTROPY,
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
        0, // MinLOD
        FLT_MAX, // MaxLOD
        0, // ShaderRegister
        0, // RegisterSpace
        D3D12_SHADER_VISIBILITY_ALL,
    };

    D3D12_STATIC_SAMPLER_DESC desc = s_desc;
    desc.ShaderRegister = shaderRegister;
    desc.ShaderVisibility = shaderVisibility;
    desc.RegisterSpace = registerSpace;
    return desc;
};

const D3D12_STATIC_SAMPLER_DESC CommonStates::StaticAnisotropicWrap(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility, unsigned int registerSpace)
{
    static const D3D12_STATIC_SAMPLER_DESC s_desc = {
        D3D12_FILTER_ANISOTROPIC,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
        0, // MipLODBias
        D3D12_MAX_MAXANISOTROPY,
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
        0, // MinLOD
        FLT_MAX, // MaxLOD
        0, // ShaderRegister
        0, // RegisterSpace
        D3D12_SHADER_VISIBILITY_ALL,
    };

    D3D12_STATIC_SAMPLER_DESC desc = s_desc;
    desc.ShaderRegister = shaderRegister;
    desc.ShaderVisibility = shaderVisibility;
    desc.RegisterSpace = registerSpace;
    return desc;
};

const D3D12_STATIC_SAMPLER_DESC CommonStates::StaticAnisotropicClamp(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility, unsigned int registerSpace)
{
    static const D3D12_STATIC_SAMPLER_DESC s_desc = {
        D3D12_FILTER_ANISOTROPIC,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
        0, // MipLODBias
        D3D12_MAX_MAXANISOTROPY,
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
        0, // MinLOD
        FLT_MAX, // MaxLOD
        0, // ShaderRegister
        0, // RegisterSpace
        D3D12_SHADER_VISIBILITY_ALL,
    };

    D3D12_STATIC_SAMPLER_DESC desc = s_desc;
    desc.ShaderRegister = shaderRegister;
    desc.ShaderVisibility = shaderVisibility;
    desc.RegisterSpace = registerSpace;
    return desc;
};
