#include "stdafx.h"
#include "r4.h"

int CRender::translateSector(IRender_Sector* pSector)
{
    if (!pSector)
        return -1;

    for (u32 i = 0; i < dsgraph.Sectors.size(); ++i)
    {
        if (dsgraph.Sectors[i] == pSector)
            return i;
    }

    FATAL("Sector was not found!");
    NODEFAULT;

#ifdef DEBUG
    return (-1);
#endif // #ifdef DEBUG
}

IRender_Sector* CRender::detectSector(const Fvector& P)
{
    IRender_Sector* S = NULL;
    Fvector dir;

    dir.set(0, -1, 0);
    S = detectSector(P, dir);
    if (NULL == S)
    {
        dir.set(0, 1, 0);
        S = detectSector(P, dir);
    }
    return S;
}

IRender_Sector* CRender::detectSector(const Fvector& P, Fvector& dir)
{
    // Portals model
    int id1 = -1;
    float range1 = 500.f;
    if (rmPortals)
    {
        dsgraph.Sectors_xrc.ray_query(CDB::OPT_ONLYNEAREST, rmPortals, P, dir, range1);
        if (dsgraph.Sectors_xrc.r_count())
        {
            CDB::RESULT* RP1 = dsgraph.Sectors_xrc.r_begin();
            id1 = RP1->id;
            range1 = RP1->range;
        }
    }

    // Geometry model
    int id2 = -1;
    float range2 = range1;
    dsgraph.Sectors_xrc.ray_query(CDB::OPT_ONLYNEAREST, g_pGameLevel->ObjectSpace.GetStaticModel(), P, dir, range2);
    if (dsgraph.Sectors_xrc.r_count())
    {
        CDB::RESULT* RP2 = dsgraph.Sectors_xrc.r_begin();
        id2 = RP2->id;
        range2 = RP2->range;
    }

    // Select ID
    int ID;
    if (id1 >= 0)
    {
        if (id2 >= 0)
            ID = (range1 <= range2 + EPS) ? id1 : id2; // both was found
        else
            ID = id1; // only id1 found
    }
    else if (id2 >= 0)
        ID = id2; // only id2 found
    else
        return 0;

    if (ID == id1)
    {
        // Take sector, facing to our point from portal
        CDB::TRI* pTri = rmPortals->get_tris() + ID;
        CPortal* pPortal = dsgraph.Portals[pTri->dummy];
        return pPortal->getSectorFacing(P);
    }
    else
    {
        // Take triangle at ID and use it's Sector
        CDB::TRI* pTri = g_pGameLevel->ObjectSpace.GetStaticTris() + ID;
        return getSector(pTri->sector);
    }
}
