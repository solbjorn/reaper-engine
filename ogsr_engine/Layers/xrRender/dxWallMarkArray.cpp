#include "stdafx.h"

#include "dxUIShader.h"
#include "dxWallMarkArray.h"

void dxWallMarkArray::Copy(IWallMarkArray& _in)
{
    auto& in{*smart_cast<const dxWallMarkArray*>(&_in)};

    m_CollideMarks = in.m_CollideMarks;
}

dxWallMarkArray::~dxWallMarkArray()
{
    for (ShaderIt it = m_CollideMarks.begin(); it != m_CollideMarks.end(); ++it)
        it->destroy();
}

void dxWallMarkArray::AppendMark(LPCSTR s_textures)
{
    ref_shader s;
    LPCSTR sh_name = "effects\\wallmark";

    if (RImplementation.o.ssfx_blood)
    {
        // Use the blood shader for any texture with the name wm_blood_*
        if (strstr(s_textures, "wm_blood_"))
            sh_name = "effects\\wallmark_blood";
    }

    s.create(sh_name, s_textures);
    m_CollideMarks.push_back(s);
}

void dxWallMarkArray::clear() { return m_CollideMarks.clear(); }

bool dxWallMarkArray::empty() { return m_CollideMarks.empty(); }

wm_shader dxWallMarkArray::GenerateWallmark()
{
    wm_shader res;

    if (!m_CollideMarks.empty())
        ((dxUIShader*)&*res)->hShader = m_CollideMarks[::Random.randI(0, m_CollideMarks.size())];

    return res;
}

ref_shader* dxWallMarkArray::dxGenerateWallmark() { return m_CollideMarks.empty() ? nullptr : &m_CollideMarks[::Random.randI(0, m_CollideMarks.size())]; }
