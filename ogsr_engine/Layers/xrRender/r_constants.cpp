#include "stdafx.h"

#include "ResourceManager.h"

#include "../../xrCore/xrPool.h"
#include "r_constants.h"

#include "../xrRender/dxRenderDeviceRender.h"

// pool
//.static	poolSS<R_constant,512>			g_constant_allocator;

// R_constant_table::~R_constant_table	()	{	dxRenderDeviceRender::Instance().Resources->_DeleteConstantTable(this);	}

R_constant_table::~R_constant_table()
{
    // dxRenderDeviceRender::Instance().Resources->_DeleteConstantTable(this);
    DEV->_DeleteConstantTable(this);
}

void R_constant_table::fatal(LPCSTR S) { FATAL(S); }

ref_constant R_constant_table::get(LPCSTR S) const
{
    // assumption - sorted by name
    c_table::const_iterator I = std::lower_bound(table.cbegin(), table.cend(), S, [](const ref_constant& C, const char* S) { return xr_strcmp(*C->name, S) < 0; });

    if (I == table.cend() || (0 != xr_strcmp((*I)->name.c_str(), S)))
        return nullptr;
    return *I;
}
ref_constant R_constant_table::get(const shared_str& S) const
{
    // linear search, but only ptr-compare
    for (const ref_constant& C : table)
    {
        if (C->name.equal(S))
            return C;
    }

    return nullptr;
}

/// !!!!!!!!FIX THIS FOR DX11!!!!!!!!!
void R_constant_table::merge(R_constant_table* T)
{
    if (0 == T)
        return;

    // Real merge
    xr_vector<ref_constant> table_tmp;
    table_tmp.reserve(table.size());

    for (u32 it = 0; it < T->table.size(); it++)
    {
        ref_constant src = T->table[it];
        ref_constant C = get(*src->name);
        if (!C)
        {
            C = xr_new<R_constant>(); //.g_constant_allocator.create();
            C->name = src->name;
            C->destination = src->destination;
            C->type = src->type;
            C->ps = src->ps;
            C->vs = src->vs;
            C->gs = src->gs;
            C->hs = src->hs;
            C->ds = src->ds;
            C->cs = src->cs;
            C->samp = src->samp;
            table_tmp.push_back(C);
        }
        else
        {
            VERIFY2(!(C->destination & src->destination & RC_dest_sampler), "Can't have samplers or textures with the same name for PS, VS and GS.");
            C->destination |= src->destination;
            VERIFY(C->type == src->type);
            R_constant_load& sL = src->get_load(src->destination);
            R_constant_load& dL = C->get_load(src->destination);
            dL.index = sL.index;
            dL.cls = sL.cls;
        }
    }

    if (!table_tmp.empty())
    {
        // Append
        std::move(table_tmp.begin(), table_tmp.end(), std::back_inserter(table));

        // Sort
        std::sort(table.begin(), table.end(), [](const ref_constant& C1, const ref_constant& C2) { return xr_strcmp(C1->name, C2->name) < 0; });
    }

    //	TODO:	DX10:	Implement merge with validity check
    m_CBTable.reserve(m_CBTable.size() + T->m_CBTable.size());
    for (u32 i = 0; i < T->m_CBTable.size(); ++i)
        m_CBTable.push_back(T->m_CBTable[i]);
}

void R_constant_table::clear()
{
    //.
    for (u32 it = 0; it < table.size(); it++)
        table[it] = 0; //.g_constant_allocator.destroy(table[it]);
    table.clear();
    m_CBTable.clear();
}

BOOL R_constant_table::equal(R_constant_table& C)
{
    if (table.size() != C.table.size())
        return FALSE;
    const size_t size = table.size();
    for (size_t it = 0; it < size; it++)
    {
        if (!table[it]->equal(*C.table[it]))
            return FALSE;
    }

    return TRUE;
}
