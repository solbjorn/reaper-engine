#include "stdafx.h"
#include "r4.h"
#include "../xrRender/ResourceManager.h"
#include "../xrRender/fbasicvisual.h"
#include "../../xr_3da/fmesh.h"
#include "../../xr_3da/xrLevel.h"
#include "../../xr_3da/x_ray.h"
#include "../../xr_3da/IGame_Persistent.h"
#include "../../xrCore/stream_reader.h"

#include "../xrRender/dxRenderDeviceRender.h"

#include "../xrRenderDX10/dx10BufferUtils.h"
#include "../xrRenderDX10/3DFluid/dx103DFluidVolume.h"

#include "../xrRender/FHierrarhyVisual.h"

#include <Utilities/FlexibleVertexFormat.h>

void CRender::level_Load(IReader* fs)
{
    R_ASSERT(g_pGameLevel);
    R_ASSERT(!b_loaded);

    u32 m_base, c_base, m_lmaps, c_lmaps;
    RImplementation.ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources load...");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    // Begin
    pApp->LoadBegin();
    Resources->DeferredLoad(TRUE);

    IReader* chunk;

    // Shaders
    g_pGamePersistent->LoadTitle("st_loading_shaders");
    {
        chunk = fs->open_chunk(fsL_SHADERS);
        R_ASSERT2(chunk, "Level doesn't builded correctly.");
        u32 count = chunk->r_u32();
        Shaders.resize(count);
        for (u32 i = 0; i < count; i++) // skip first shader as "reserved" one
        {
            string512 n_sh, n_tlist;
            LPCSTR n = LPCSTR(chunk->pointer());
            chunk->skip_stringZ();
            if (0 == n[0])
                continue;
            xr_strcpy(n_sh, n);
            LPSTR delim = strchr(n_sh, '/');
            *delim = 0;
            xr_strcpy(n_tlist, delim + 1);
            Shaders[i] = Resources->Create(n_sh, n_tlist);
        }
        chunk->close();
    }

    // Components
    Wallmarks = xr_new<CWallmarksEngine>();
    Details = xr_new<CDetailManager>();

    // VB,IB,SWI
    g_pGamePersistent->LoadTitle("st_loading_geometry");
    {
        CStreamReader* geom = FS.rs_open("$level$", "level.geom");
        R_ASSERT2(geom, "level.geom");
        LoadBuffers(geom, FALSE);
        LoadSWIs(geom);
        FS.r_close(geom);
    }

    //...and alternate/fast geometry
    {
        CStreamReader* geom = FS.rs_open("$level$", "level.geomx");
        R_ASSERT2(geom, "level.geomX");
        LoadBuffers(geom, TRUE);
        FS.r_close(geom);
    }

    // Visuals
    g_pGamePersistent->LoadTitle("st_loading_spatial_db");
    chunk = fs->open_chunk(fsL_VISUALS);
    LoadVisuals(chunk);
    chunk->close();

    // Details
    g_pGamePersistent->LoadTitle("st_loading_details");
    Details->Load();

    // Sectors
    g_pGamePersistent->LoadTitle("st_loading_sectors_portals");
    LoadSectors(fs);

    // 3D Fluid
    Load3DFluid();

    // HOM
    HOM.Load();

    // Lights
    g_pGamePersistent->LoadTitle("st_loading_lights");
    LoadLights(fs);

    // End
    pApp->LoadEnd();

    // u32 m_base, c_base, m_lmaps, c_lmaps;
    RImplementation.ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources load completed!");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    // signal loaded
    b_loaded = TRUE;
}

void CRender::level_Unload()
{
    if (!g_pGameLevel)
        return;
    if (!b_loaded)
        return;

    u32 m_base, c_base, m_lmaps, c_lmaps;
    RImplementation.ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources unload...");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    // HOM
    HOM.Unload();

    //*** Details
    Details->Unload();

    //*** Sectors
    // 1.
    xr_delete(rmPortals);
    last_sector_id = INVALID_SECTOR_ID;

    // 2.
    cleanup_contexts();

    //*** Lights
    Lights.Unload();

    //*** Visuals
    for (dxRender_Visual* visual : Visuals)
    {
        visual->Release();
        xr_delete(visual);
    }
    Visuals.clear();

    //*** SWI
    for (auto& swi : SWIs)
        xr_free(swi.sw);
    SWIs.clear();

    //*** VB/IB
    for (auto* buffer : nVB)
        _RELEASE(buffer);
    for (auto* buffer : xVB)
        _RELEASE(buffer);

    nVB.clear();
    xVB.clear();

    for (auto* buffer : nIB)
        _RELEASE(buffer);
    for (auto* buffer : xIB)
        _RELEASE(buffer);

    nIB.clear();
    xIB.clear();
    nDC.clear();
    xDC.clear();

    //*** Components
    xr_delete(Details);
    xr_delete(Wallmarks);

    //*** Shaders
    Shaders.clear();

    // u32 m_base, c_base, m_lmaps, c_lmaps;
    RImplementation.ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources unload completed!");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    b_loaded = FALSE;
}

void CRender::LoadBuffers(CStreamReader* base_fs, BOOL _alternative)
{
    R_ASSERT2(base_fs, "Could not load geometry. File not found.");
    Resources->Evict();

    xr_vector<VertexDeclarator>& _DC = _alternative ? xDC : nDC;
    xr_vector<ID3DVertexBuffer*>& _VB = _alternative ? xVB : nVB;
    xr_vector<ID3DIndexBuffer*>& _IB = _alternative ? xIB : nIB;

    // Vertex buffers
    {
        // Use DX9-style declarators
        CStreamReader* fs = base_fs->open_chunk(fsL_VB);
        R_ASSERT2(fs, "Could not load geometry. File 'level.geom?' corrupted.");
        u32 count = fs->r_u32();
        _DC.resize(count);
        _VB.resize(count);

        // decl
        constexpr u32 buffer_size = (MAXD3DDECLLENGTH + 1) * sizeof(D3DVERTEXELEMENT9);
        auto* dcl = (D3DVERTEXELEMENT9*)_alloca(buffer_size);

        for (u32 i = 0; i < count; i++)
        {
            fs->r(dcl, buffer_size);
            fs->advance(-(int)buffer_size);

            u32 dcl_len = FVF::GetDeclLength(dcl) + 1;
            _DC[i].resize(dcl_len);
            fs->r(_DC[i].begin(), dcl_len * sizeof(D3DVERTEXELEMENT9));

            // count, size
            u32 vCount = fs->r_u32();
            u32 vSize = FVF::ComputeVertexSize(dcl, 0);
#ifndef MASTER_GOLD
            Msg("* [Loading VB] %d verts, %d Kb", vCount, (vCount * vSize) / 1024);
#endif

            // Create and fill
            //	TODO: DX10: Check fragmentation.
            //	Check if buffer is less then 2048 kb
            BYTE* pData = xr_alloc<BYTE>(vCount * vSize);
            fs->r(pData, vCount * vSize);
            dx10BufferUtils::CreateVertexBuffer(&_VB[i], pData, vCount * vSize);
            xr_free(pData);

            //			fs->advance			(vCount*vSize);
        }
        fs->close();
    }

    // Index buffers
    {
        CStreamReader* fs = base_fs->open_chunk(fsL_IB);
        u32 count = fs->r_u32();
        _IB.resize(count);
        for (u32 i = 0; i < count; i++)
        {
            u32 iCount = fs->r_u32();
#ifndef MASTER_GOLD
            Msg("* [Loading IB] %d indices, %d Kb", iCount, (iCount * 2) / 1024);
#endif

            // Create and fill
            //	TODO: DX10: Check fragmentation.
            //	Check if buffer is less then 2048 kb
            BYTE* pData = xr_alloc<BYTE>(iCount * 2);
            fs->r(pData, iCount * 2);
            dx10BufferUtils::CreateIndexBuffer(&_IB[i], pData, iCount * 2);
            xr_free(pData);
        }
        fs->close();
    }
}

void CRender::LoadVisuals(IReader* fs)
{
    u32 index = 0;
    IReader* chunk = nullptr;

    while ((chunk = fs->open_chunk(index)) != 0)
    {
        ogf_header H;
        chunk->r_chunk_safe(OGF_HEADER, &H, sizeof(H));

        dxRender_Visual* V = Models->Instance_Create(H.type);
        V->Load(nullptr, chunk, 0);
        Visuals.push_back(V);

        chunk->close();
        index++;
    }
}

void CRender::LoadLights(IReader* fs)
{
    // lights
    Lights.Load(fs);
    Lights.LoadHemi();
}

void CRender::LoadSectors(IReader* fs)
{
    // allocate memory for portals
    const u32 pt_size = fs->find_chunk(fsL_PORTALS);
    R_ASSERT(0 == pt_size % sizeof(CPortal::level_portal_data_t));

    const u32 portals_count = pt_size / sizeof(CPortal::level_portal_data_t);
    xr_vector<CPortal::level_portal_data_t> portals_data{portals_count};

    // load sectors
    xr_vector<CSector::level_sector_data_t> sectors_data;

    float largest_sector_vol = 0;
    IReader* S = fs->open_chunk(fsL_SECTORS);

    for (sector_id_t i = 0;; i++)
    {
        IReader* P = S->open_chunk(i);
        if (!P)
            break;

        auto& sector_data = sectors_data.emplace_back();

        u32 size = P->find_chunk(fsP_Portals);
        R_ASSERT(0 == (size & 1));
        u32 portals_in_sector = size / sizeof(u16);

        sector_data.portals_id.reserve(portals_in_sector);
        while (portals_in_sector)
        {
            const u16 ID = P->r_u16();
            sector_data.portals_id.emplace_back(ID);
            --portals_in_sector;
        }

        size = P->find_chunk(fsP_Root);
        R_ASSERT(size == 4);
        sector_data.root_id = P->r_u32();

        // Search for default sector - assume "default" or "outdoor" sector is the largest one
        // XXX: hack: need to know real outdoor sector
        auto* V = (dxRender_Visual*)getVisual(sector_data.root_id);
        float vol = V->vis.box.getvolume();
        if (vol > largest_sector_vol)
        {
            largest_sector_vol = vol;
            largest_sector_id = i;
        }

        P->close();
    }
    S->close();

    // load portals
    if (portals_count)
    {
        CDB::Collector CL;
        fs->find_chunk(fsL_PORTALS);
        for (u32 i = 0; i < portals_count; i++)
        {
            auto& P = portals_data[i];
            fs->r(&P, sizeof(P));

            for (u32 j = 2; j < P.vertices.size(); j++)
                CL.add_face_packed_D(P.vertices[0], P.vertices[j - 1], P.vertices[j], u32(i));
        }
        if (CL.getTS() < 2)
        {
            constexpr Fvector v1{-20000.f, -20000.f, -20000.f};
            constexpr Fvector v2{-20001.f, -20001.f, -20001.f};
            constexpr Fvector v3{-20002.f, -20002.f, -20002.f};
            CL.add_face_packed_D(v1, v2, v3, 0);
        }

        // build portal model
        rmPortals = xr_new<CDB::MODEL>();
        rmPortals->build(CL.getV(), int(CL.getVS()), CL.getT(), int(CL.getTS()));
    }
    else
    {
        rmPortals = nullptr;
    }

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
    {
        auto& dsgraph = contexts_pool[id];

        dsgraph.reset();
        dsgraph.load(sectors_data, portals_data);
    }

    contexts_used.zero();
}

void CRender::LoadSWIs(CStreamReader* base_fs)
{
    // allocate memory for portals
    if (base_fs->find_chunk(fsL_SWIS))
    {
        CStreamReader* fs = base_fs->open_chunk(fsL_SWIS);
        u32 item_count = fs->r_u32();

        for (auto& SWI : SWIs)
            xr_free(SWI.sw);

        SWIs.clear();

        SWIs.resize(item_count);
        for (u32 c = 0; c < item_count; c++)
        {
            FSlideWindowItem& swi = SWIs[c];
            swi.reserved[0] = fs->r_u32();
            swi.reserved[1] = fs->r_u32();
            swi.reserved[2] = fs->r_u32();
            swi.reserved[3] = fs->r_u32();
            swi.count = fs->r_u32();
            VERIFY(!swi.sw);
            swi.sw = xr_alloc<FSlideWindow>(swi.count);
            fs->r(swi.sw, sizeof(FSlideWindow) * swi.count);
        }
        fs->close();
    }
}

void CRender::Load3DFluid()
{
    if (!ps_r2_ls_flags.test(R3FLAG_VOLUMETRIC_SMOKE))
        return;

    string_path fn_game;
    if (FS.exist(fn_game, "$level$", "level.fog_vol"))
    {
        IReader* F = FS.r_open(fn_game);
        u16 version = F->r_u16();

        if (version == 3)
        {
            u32 cnt = F->r_u32();
            for (u32 i = 0; i < cnt; ++i)
            {
                dx103DFluidVolume* pVolume = xr_new<dx103DFluidVolume>();
                pVolume->Load("", F, 0);

                const auto& v = pVolume->getVisData().sphere.P;

                Msg("~ Loading fog volume with profile [%s]. Position x=[%f] y=[%f] z=[%f]", pVolume->getProfileName().c_str(), v.x, v.y, v.z);

                //	Attach to sector's static geometry
                for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
                {
                    auto& dsgraph = contexts_pool[id];
                    const auto sector_id = dsgraph.detect_sector(pVolume->getVisData().sphere.P);
                    auto* pSector = dsgraph.get_sector(sector_id);

                    if (!pSector)
                    {
                        Msg("!!Cannot find sector for fog volume. Position x=[%f] y=[%f] z=[%f]!", v.x, v.y, v.z);

                        xr_delete(pVolume);
                        continue;
                    }

                    //	3DFluid volume must be in render sector
                    R_ASSERT(pSector);

                    dxRender_Visual* pRoot = pSector->root();
                    //	Sector must have root
                    R_ASSERT(pRoot);
                    R_ASSERT(pRoot->getType() == MT_HIERRARHY);

                    ((FHierrarhyVisual*)pRoot)->children.push_back(pVolume);
                }
            }
        }

        FS.r_close(F);
    }
}
