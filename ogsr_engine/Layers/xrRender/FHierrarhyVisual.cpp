// FHierrarhyVisual.cpp: implementation of the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "FHierrarhyVisual.h"

#include "../../xr_3da/Fmesh.h"
#include "../../xr_3da/Render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FHierrarhyVisual::FHierrarhyVisual() : dxRender_Visual() { bDontDelete = FALSE; }

FHierrarhyVisual::~FHierrarhyVisual()
{
    if (!bDontDelete)
    {
        for (auto& child : children)
            RImplementation.model_Delete((IRenderVisual*&)child, false);
    }
    children.clear();
}

void FHierrarhyVisual::Release()
{
    if (!bDontDelete)
    {
        for (u32 i = 0; i < children.size(); i++)
            children[i]->Release();
    }
}

void FHierrarhyVisual::Load(const char* N, IReader* data, u32 dwFlags)
{
    dxRender_Visual::Load(N, data, dwFlags);
    if (data->find_chunk(OGF_CHILDREN_L))
    {
        // From Link
        u32 cnt = data->r_u32();
        children.resize(cnt);
        for (u32 i = 0; i < cnt; i++)
        {
            u32 ID = data->r_u32();
            children[i] = (dxRender_Visual*)RImplementation.getVisual(ID);
        }
        bDontDelete = TRUE;
    }
    else
    {
        if (data->find_chunk(OGF_CHILDREN))
        {
            // From stream
            IReader* OBJ = data->open_chunk(OGF_CHILDREN);
            if (OBJ)
            {
                IReader* O = OBJ->open_chunk(0);
                for (int count = 1; O; count++)
                {
                    std::string_view short_name{N};

                    if (const auto pos = short_name.rfind('.'); pos != std::string_view::npos)
                        short_name = short_name.substr(0, pos);

                    children.push_back((dxRender_Visual*)RImplementation.model_CreateChild(xr::format("{}:{}", short_name, count).c_str(), O));

                    O->close();
                    O = OBJ->open_chunk(count);
                }
                OBJ->close();
            }
            bDontDelete = FALSE;
        }
        else
        {
            FATAL("Invalid visual");
        }
    }
}

void FHierrarhyVisual::Copy(dxRender_Visual* pSrc)
{
    dxRender_Visual::Copy(pSrc);

    FHierrarhyVisual* pFrom = (FHierrarhyVisual*)pSrc;

    children.clear();
    children.reserve(pFrom->children.size());
    for (u32 i = 0; i < pFrom->children.size(); i++)
    {
        dxRender_Visual* p = (dxRender_Visual*)RImplementation.model_Duplicate(pFrom->children[i]);
        children.push_back(p);
    }
    bDontDelete = FALSE;
}
