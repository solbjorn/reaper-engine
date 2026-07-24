#include "stdafx.h"
#include "dxThunderboltDescRender.h"

void dxThunderboltDescRender::Copy(IThunderboltDescRender& _in)
{
    auto& in{*smart_cast<const dxThunderboltDescRender*>(&_in)};

    l_model = in.l_model;
}

void dxThunderboltDescRender::CreateModel(LPCSTR m_name)
{
    const auto F = XR_ASSERT_VAL(absl::WrapUnique(FS.r_open("$game_meshes$", m_name)), "lightning model not found", m_name);
    l_model = ::RImplementation.model_CreateDM(F.get());
}

void dxThunderboltDescRender::DestroyModel() { ::RImplementation.model_Delete(l_model); }
