#include "stdafx.h"
#include "dxThunderboltDescRender.h"

void dxThunderboltDescRender::Copy(IThunderboltDescRender& _in)
{
    auto& in{*smart_cast<const dxThunderboltDescRender*>(&_in)};

    l_model = in.l_model;
}

void dxThunderboltDescRender::CreateModel(LPCSTR m_name)
{
    IReader* F = FS.r_open("$game_meshes$", m_name);
    ASSERT_FMT(F, "Cannot open 'lightning_model' path=[%s].", m_name);

    l_model = ::RImplementation.model_CreateDM(F);
    FS.r_close(F);
}

void dxThunderboltDescRender::DestroyModel() { ::RImplementation.model_Delete(l_model); }
