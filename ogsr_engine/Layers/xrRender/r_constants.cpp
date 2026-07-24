#include "stdafx.h"

#include "r_constants.h"

#include "ResourceManager.h"

#include "../../xrCore/xrPool.h"

R_constant_table::~R_constant_table() { RImplementation.Resources->_DeleteConstantTable(this); }

ref_constant R_constant_table::get(LPCSTR S) const
{
    // assumption - sorted by name
    const auto I = std::lower_bound(table.cbegin(), table.cend(), S, [](const ref_constant& C, const char* S) { return std::is_lt(xr_strcmp(C->name, S)); });
    if (I == table.cend() || std::is_neq(xr_strcmp((*I)->name, S)))
        return ref_constant{};

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

    return ref_constant{};
}

void R_constant_table::merge(const R_constant_table* T)
{
    if (T == nullptr)
        return;

    XR_TRACY_ZONE_SCOPED();

    // Real merge
    xr_vector<ref_constant> table_tmp;
    table_tmp.reserve(table.size());

    for (const auto& src : T->table)
    {
        ref_constant C = get(src->name);
        if (!C)
        {
            auto cc = xr_new<R_constant>();

            cc->name = src->name;
            cc->destination = src->destination;
            cc->type = src->type;
            cc->ps = src->ps;
            cc->vs = src->vs;
            cc->gs = src->gs;
            cc->hs = src->hs;
            cc->ds = src->ds;
            cc->cs = src->cs;
            cc->samp = src->samp;
            cc->handler = src->handler;

            table_tmp.emplace_back(cc);
        }
        else
        {
            XR_ASSERT(!(C->destination & src->destination & RC_dest_sampler), "", C->destination, src->destination);
            C->destination |= src->destination;
            XR_ASSERT(C->type == src->type);

            R_constant_load& sL = src->get_load(src->destination);
            R_constant_load& dL = C->get_load(src->destination);
            dL.index = sL.index;
            dL.cls = sL.cls;
        }
    }

    if (!table_tmp.empty())
    {
        // Append
        table.append_range(std::views::as_rvalue(table_tmp));
        // Sort
        std::ranges::sort(table, {}, [] [[nodiscard]] (const auto& constant) { return std::string_view{constant->name}; });
    }

    //	TODO:	DX10:	Implement merge with validity check
    for (auto [tbl, that] : std::views::zip(m_CBTable, T->m_CBTable))
        tbl.append_range(that);
}

void R_constant_table::clear()
{
    for (auto& c : table)
        c._set(nullptr);

    table.clear();

    for (auto& tbl : m_CBTable)
        tbl.clear();
}

bool R_constant_table::equal(const R_constant_table& C) const
{
    if (table.size() != C.table.size())
        return false;

    for (auto [c, that] : std::views::zip(table, C.table))
    {
        if (!c->equal(*that))
            return false;
    }

    return true;
}
