//---------------------------------------------------------------------------
#ifndef particle_actionsH
#define particle_actionsH

namespace PAPI
{
// refs
struct ParticleEffect;

struct XR_NOVTABLE ParticleAction : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ParticleAction);

public:
    enum
    {
        ALLOW_ROTATE = (1 << 1)
    };
    bool m_copFormat{};
    Flags32 m_Flags;
    PActionEnum type{}; // Type field

    ParticleAction() { m_Flags.zero(); }
    virtual ~ParticleAction() = 0;

    virtual void Execute(ParticleEffect* pe, const float dt) = 0;
    virtual void Transform(const Fmatrix& m) = 0;

    virtual void Load(IReader& F) = 0;
    virtual void Save(IWriter& F) = 0;
};

inline ParticleAction::~ParticleAction() = default;

DEFINE_VECTOR(ParticleAction*, PAVec, PAVecIt);

class ParticleActions
{
    PAVec m_actions;

public:
    std::recursive_mutex m_bLocked;

    ParticleActions() { m_actions.reserve(4); }
    ~ParticleActions() { clear(); }

    IC void clear()
    {
        std::scoped_lock slock(m_bLocked);

        for (auto& it : m_actions)
            xr_delete(it);

        m_actions.clear();
    }

    IC void append(ParticleAction* pa)
    {
        std::scoped_lock slock(m_bLocked);
        m_actions.push_back(pa);
    }

    IC bool empty() const { return m_actions.empty(); }
    IC PAVecIt begin() { return m_actions.begin(); }
    IC PAVecIt end() { return m_actions.end(); }
    IC size_t size() const { return m_actions.size(); }
    void lock() { m_bLocked.lock(); }
    void unlock() { m_bLocked.unlock(); }
};
} // namespace PAPI
//---------------------------------------------------------------------------
#endif
