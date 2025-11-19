// ModelPool.h: interface for the CModelPool class.
//////////////////////////////////////////////////////////////////////
#ifndef ModelPoolH
#define ModelPoolH

// refs
class dxRender_Visual;

namespace PS
{
struct SEmitter;
}

// defs
class CModelPool : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CModelPool);

private:
    friend class CRender;

    struct str_pred
    {
        [[nodiscard]] constexpr bool operator()(const shared_str& x, const shared_str& y) const { return std::is_lt(xr_strcmp(x, y)); }
    };

    struct ModelDef
    {
        shared_str name;
        dxRender_Visual* model{};
        std::atomic<gsl::index> refs{};

        constexpr ModelDef() = default;
        constexpr explicit ModelDef(gsl::czstring nm, dxRender_Visual* md) : model{md} { name._set(nm); }

        // Non-atomic, for vector purposes only
        constexpr ModelDef(const ModelDef& that) : name{that.name}, model{that.model}, refs{that.refs.load()} {}

        constexpr ModelDef(ModelDef&& that)
        {
            name = std::move(that.name);
            model = std::move(that.model);
            refs = that.refs.load();
        }

        constexpr ModelDef& operator=(const ModelDef& that)
        {
            name = that.name;
            model = that.model;
            refs = that.refs.load();

            return *this;
        }

        constexpr ModelDef& operator=(ModelDef&& that)
        {
            name = std::move(that.name);
            model = std::move(that.model);
            refs = that.refs.load();

            return *this;
        }
    };

    typedef xr_multimap<shared_str, dxRender_Visual*, str_pred> POOL;
    typedef POOL::iterator POOL_IT;
    typedef xr_map<dxRender_Visual*, shared_str> REGISTRY;
    typedef REGISTRY::iterator REGISTRY_IT;

private:
    xr_vector<ModelDef> Models; // Reference / Base
    xr_vector<dxRender_Visual*> ModelsToDelete; //
    REGISTRY Registry; // Just pairing of pointer / Name
    POOL Pool; // Unused / Inactive
    BOOL bLogging;
    BOOL bForceDiscard;
    BOOL bAllowChildrenDuplicate;

    string_unordered_map<std::string, bool> m_prefetched;

    void Destroy();
    void refresh_prefetch(const char* low_name, const bool is_hud_visual);
    void process_vis_prefetch();

    CInifile* vis_prefetch_ini = nullptr;

    bool now_prefetch1 = false;
    bool now_prefetch2 = false;

public:
    CModelPool();
    ~CModelPool() override;

    dxRender_Visual* Instance_Create(u32 Type);
    dxRender_Visual* Instance_Duplicate(dxRender_Visual* V);
    dxRender_Visual* Instance_Load(LPCSTR N, BOOL allow_register);
    dxRender_Visual* Instance_Load(LPCSTR N, IReader* data, BOOL allow_register);
    void Instance_Register(LPCSTR N, dxRender_Visual* V);
    dxRender_Visual* Instance_Find(LPCSTR N);

    dxRender_Visual* CreatePE(PS::CPEDef* source);
    dxRender_Visual* CreatePG(PS::CPGDef* source);
    dxRender_Visual* Create(LPCSTR name, IReader* data = nullptr);
    dxRender_Visual* CreateChild(LPCSTR name, IReader* data);
    void Delete(dxRender_Visual*& V, BOOL bDiscard = FALSE);
    void Discard(dxRender_Visual*& V, BOOL b_complete);
    void DeleteInternal(dxRender_Visual*& V, BOOL bDiscard = FALSE);
    void DeleteQueue();

    void Logging(BOOL bEnable) { bLogging = bEnable; }

    void Prefetch();
    void ClearPool(BOOL b_complete);

    void dump();

    void memory_stats(u32& vb_mem_video, u32& vb_mem_system, u32& ib_mem_video, u32& ib_mem_system);

    void save_vis_prefetch();
    void begin_prefetch1(bool val);
};

#endif // ModelPoolH
