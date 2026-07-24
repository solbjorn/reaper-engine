#include "stdafx.h"

#include "xrStripify.h"

#include "NvTriStrip.h"
#include "VertexCache.h"

int xrSimulate(xr_vector<u16>& indices, int iCacheSize)
{
    VertexCache C(iCacheSize);

    int count = 0;
    for (int id : indices)
    {
        if (C.InCache(id))
            continue;
        count++;
        C.AddEntry(id);
    }
    return count;
}

void xrStripify(xr_vector<u16>& indices, xr_vector<u16>& perturb, int iCacheSize, int iMinStripLength)
{
    SetCacheSize(iCacheSize);
    SetMinStripSize(iMinStripLength);
    SetListsOnly(true);

    // Generate strips
    xr_vector<PrimitiveGroup> PGROUP;
    GenerateStrips(indices.data(), indices.size(), PGROUP);
    XR_ASSERT(PGROUP.size() == 1 && PGROUP[0].type == PT_LIST, "", PGROUP.size());

    // Remap indices
    xr_vector<PrimitiveGroup> xPGROUP;
    RemapIndices(PGROUP, perturb.size(), xPGROUP);
    XR_ASSERT(xPGROUP.size() == 1 && xPGROUP[0].type == PT_LIST, "", xPGROUP.size());

    // Build perturberation table
    XR_ASSERT(PGROUP[0].numIndices == indices.size() && xPGROUP[0].numIndices == PGROUP[0].numIndices, "", indices.size(), PGROUP[0].numIndices,
              xPGROUP[0].numIndices);

    for (auto [xpg, pg] : std::views::zip(std::span{xPGROUP[0].indices, xPGROUP[0].numIndices}, std::span{PGROUP[0].indices, PGROUP[0].numIndices}))
        perturb[xpg] = XR_ASSERT_VAL(pg < perturb.size());

    // Copy indices
    std::memcpy(indices.data(), xPGROUP[0].indices, indices.size() * sizeof(u16));
}
