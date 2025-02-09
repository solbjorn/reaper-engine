// DetailManager.h: interface for the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DetailManagerH
#define DetailManagerH
#pragma once

#include "../../xrCore/xrpool.h"
#include "detailformat.h"
#include "detailmodel.h"

constexpr int dm_max_decompress = 7;
// const int		dm_size				= 24;								//!
constexpr int dm_cache1_count = 4; //
// const int 		dm_cache1_line		= dm_size*2/dm_cache1_count;		//! dm_size*2 must be div dm_cache1_count
constexpr int dm_max_objects = 64;
constexpr int dm_obj_in_slot = 4;
// const int		dm_cache_line		= dm_size+1+dm_size;
// const int		dm_cache_size		= dm_cache_line*dm_cache_line;
// const float		dm_fade				= float(2*dm_size)-.5f;
constexpr float dm_slot_size = DETAIL_SLOT_SIZE;

constexpr u32 dm_max_cache_size = 62001 * 2; // assuming max dm_size = 124
extern u32 dm_size;
extern u32 dm_cache1_line;
extern u32 dm_cache_line;
extern u32 dm_cache_size;
extern float dm_fade;
extern u32 dm_current_size; //				= iFloor((float)ps_r__detail_radius/4)*2;				//!
extern u32 dm_current_cache1_line; //		= dm_current_size*2/dm_cache1_count;		//! dm_current_size*2 must be div dm_cache1_count
extern u32 dm_current_cache_line; //		= dm_current_size+1+dm_current_size;
extern u32 dm_current_cache_size; //		= dm_current_cache_line*dm_current_cache_line;
extern float dm_current_fade; //				= float(2*dm_current_size)-.5f;
extern float ps_current_detail_density;

class CDetailManager
{
public:
    float fade_distance = 99999;
    Fvector light_position;
    void details_clear();
    struct SlotItem
    { // один кустик
        float scale;
        float scale_calculated;
        Fmatrix mRotY;
        u32 vis_ID; // индекс в visibility списке он же тип [не качается, качается1, качается2]
        float c_hemi;
        float c_sun;
        float distance;
        Fvector position;
        Fvector normal;
        float alpha;
        float alpha_target;
    };
    DEFINE_VECTOR(SlotItem*, SlotItemVec, SlotItemVecIt);
    struct SlotPart
    { //
        u32 id; // ID модельки
        SlotItemVec items; // список кустиков
        SlotItemVec r_items[3]; // список кустиков for render
    };
    enum SlotType
    {
        stReady = 0, // Ready to use
        stPending, // Pending for decompression

        stFORCEDWORD = 0xffffffff
    };
    struct Slot
    { // распакованый слот размером DETAIL_SLOT_SIZE
        struct
        {
            u32 empty : 1;
            u32 type : 1;
            u32 frame : 30;
        };
        int sx, sz; // координаты слота X x Y
        vis_data vis; //
        SlotPart G[dm_obj_in_slot]; //
        bool hidden;

        Slot()
        {
            frame = 0;
            empty = 1;
            type = stReady;
            sx = sz = 0;
            vis.clear();
        }
    };
    struct CacheSlot1
    {
        u32 empty;
        vis_data vis;
        Slot** slots[dm_cache1_count * dm_cache1_count];
        CacheSlot1()
        {
            empty = 1;
            vis.clear();
        }
    };

    typedef xr_vector<xr_vector<SlotItemVec*>> vis_list;
    typedef svector<CDetail*, dm_max_objects> DetailVec;
    typedef DetailVec::iterator DetailIt;
    typedef poolSS<SlotItem, /*4096*/ 65536> PSS; // KD: try to avoid blinking
public:
    int dither[16][16];

public:
    // swing values
    struct SSwingValue
    {
        float rot1;
        float rot2;
        float amp1;
        float amp2;
        float speed;
        void lerp(const SSwingValue& v1, const SSwingValue& v2, float factor);
    };
    SSwingValue swing_desc[2];
    SSwingValue swing_current;
    float m_time_rot_1;
    float m_time_rot_2;
    float m_time_pos;
    float m_global_time_old;

public:
    IReader* dtFS;
    DetailHeader dtH;
    DetailSlot* dtSlots; // note: pointer into VFS
    DetailSlot DS_empty;

public:
    DetailVec objects;
    vis_list m_visibles[3]; // 0=still, 1=Wave1, 2=Wave2

    xrXRC xrc;
    CacheSlot1** cache_level1;
    Slot*** cache; // grid-cache itself
    svector<Slot*, dm_max_cache_size> cache_task; // non-unpacked slots
    Slot* cache_pool; // just memory for slots
    int cache_cx;
    int cache_cz;

    PSS poolSI; // pool из которого выделяются SlotItem

    void UpdateVisibleM();
    void UpdateVisibleS();

public:
    constexpr IC bool UseVS() const { return true; }

    // Software processor
    ref_geom soft_Geom;
    void soft_Load();
    void soft_Unload();
    void soft_Render();

    // Hardware processor
    ref_geom hw_Geom;
    size_t hw_BatchSize;
    ID3DVertexBuffer* hw_VB;
    ID3DIndexBuffer* hw_IB;

    void hw_Load();
    void hw_Load_Geom();
    void hw_Unload();
    void hw_Render();
    void hw_Render_dump(const Fvector4& consts, const Fvector4& wave, const Fvector4& wind, u32 var_id, u32 lod_id);

public:
    // get unpacked slot
    DetailSlot& QueryDB(int sx, int sz);

    void cache_Initialize();
    void cache_Update(int sx, int sz, Fvector& view);
    void cache_Task(int gx, int gz, Slot* D);
    Slot* cache_Query(int sx, int sz);
    void cache_Decompress(Slot* D);
    BOOL cache_Validate();
    // cache grid to world
    int cg2w_X(int x) { return cache_cx - dm_size + x; }
    int cg2w_Z(int z) { return cache_cz - dm_size + (dm_cache_line - 1 - z); }
    // world to cache grid
    int w2cg_X(int x) { return x - cache_cx + dm_size; }
    int w2cg_Z(int z) { return cache_cz - dm_size + (dm_cache_line - 1 - z); }

    void Load();
    void Unload();
    void Render();

    /// MT stuff
    xrCriticalSection MT;
    void StartAsync();
    void WaitAsync();

    std::future<void> awaiter;
    bool async_started{};

    void MT_CALC();

    bool need_init{};

    CDetailManager();
    virtual ~CDetailManager();
};

#endif // DetailManagerH
