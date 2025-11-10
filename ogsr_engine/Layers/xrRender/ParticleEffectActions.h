//---------------------------------------------------------------------------
#ifndef ParticleEffectActionsH
#define ParticleEffectActionsH

#include "../../xrCore/_stl_extensions.h"

struct PBool
{
    BOOL val;

    PBool() : val(FALSE) {}

    PBool(BOOL _val) : val(_val) {}

    void set(BOOL v) { val = v; }
};

struct PFloat
{
    float val;
    float mn;
    float mx;

    PFloat()
    {
        val = 0.f;
        mn = 0.f;
        mx = 0.f;
    }

    PFloat(float _val, float _mn, float _mx) : val(_val), mn(_mn), mx(_mx) {}

    void set(float v) { val = v; }
};

struct PInt
{
    int val;
    int mn;
    int mx;

    PInt()
    {
        val = 0;
        mn = 0;
        mx = 0;
    }

    PInt(int _val, int _mn, int _mx) : val(_val), mn(_mn), mx(_mx) {}

    void set(int v) { val = v; }
};

struct PVector
{
    Fvector val{};
    float mn{};
    float mx{};

    enum EType : u32
    {
        vNum,
        vAngle,
        vColor,
    };

    EType type{vNum};

    PVector() = default;
    PVector(EType t, Fvector _val, float _mn, float _mx) : val{_val}, mn{_mn}, mx{_mx}, type{t} {}

    void set(const Fvector& v) { val.set(v); }
    void set(float x, float y, float z) { val.set(x, y, z); }
};

struct PDomain
{
    PAPI::PDomainEnum type;

    union
    {
        float f[9];
        Fvector v[3];
    };

    enum EType : u32
    {
        vNum,
        vAngle,
        vColor,
    };

    enum : u32
    {
        flRenderable = (1 << 0)
    };

    EType e_type;
    Flags32 flags;
    u32 clr;

    PDomain() = default;
    PDomain(EType et, BOOL renderable, u32 color = 0x00000000, PAPI::PDomainEnum type = PAPI::PDPoint, float inA0 = 0.0f, float inA1 = 0.0f, float inA2 = 0.0f, float inA3 = 0.0f,
            float inA4 = 0.0f, float inA5 = 0.0f, float inA6 = 0.0f, float inA7 = 0.0f, float inA8 = 0.0f);
    PDomain(const PDomain& in);
    PDomain(PDomain&&);

    PDomain& operator=(const PDomain&);
    PDomain& operator=(PDomain&&);

    void Load(IReader& F);
    void Save(IWriter& F);

    void Load2(CInifile& ini, gsl::czstring sect);
    void Save2(CInifile& ini, gsl::czstring sect);
};

struct XR_NOVTABLE EParticleAction : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(EParticleAction);

public:
    DEFINE_MAP(std::string, PDomain, PDomainMap, PDomainMapIt);
    DEFINE_MAP(std::string, PBool, PBoolMap, PBoolMapIt);
    DEFINE_MAP(std::string, PFloat, PFloatMap, PFloatMapIt);
    DEFINE_MAP(std::string, PInt, PIntMap, PIntMapIt);
    DEFINE_MAP(std::string, PVector, PVectorMap, PVectorMapIt);

    shared_str actionName;
    shared_str actionType;
    shared_str hint;

    enum
    {
        flEnabled = (1 << 0),
        flDraw = (1 << 1),
    };

    Flags32 flags{};
    PAPI::PActionEnum type{};

    PDomainMap domains;
    PBoolMap bools;
    PFloatMap floats;
    PIntMap ints;
    PVectorMap vectors;

    enum EValType
    {
        tpDomain,
        tpVector,
        tpFloat,
        tpBool,
        tpInt,
    };

    struct SOrder
    {
        EValType type{};
        std::string name;

        SOrder(EValType _type, std::string _name) : type(_type), name(_name) {}
    };

    DEFINE_VECTOR(SOrder, OrderVec, OrderVecIt);
    OrderVec orders;

    EParticleAction(PAPI::PActionEnum _type)
    {
        flags.assign(flEnabled);
        type = _type;
    }

    virtual ~EParticleAction() = 0;

    void appendFloat(LPCSTR name, float v, float mn, float mx);
    void appendInt(LPCSTR name, int v, int mn = -P_MAXINT, int mx = P_MAXINT);
    void appendVector(LPCSTR name, PVector::EType type, float vx, float vy, float vz, float mn = -P_MAXFLOAT, float mx = P_MAXFLOAT);
    void appendDomain(LPCSTR name, PDomain v);
    void appendBool(LPCSTR name, BOOL v);

    PFloat& _float(LPCSTR name)
    {
        PFloatMapIt it = floats.find(name);
        R_ASSERT(it != floats.end(), name);
        return it->second;
    }

    PInt& _int(LPCSTR name)
    {
        PIntMapIt it = ints.find(name);
        R_ASSERT(it != ints.end(), name);
        return it->second;
    }

    PVector& _vector(LPCSTR name)
    {
        PVectorMapIt it = vectors.find(name);
        R_ASSERT(it != vectors.end(), name);
        return it->second;
    }

    PDomain& _domain(LPCSTR name)
    {
        PDomainMapIt it = domains.find(name);
        R_ASSERT(it != domains.end(), name);
        return it->second;
    }

    PBool& _bool(LPCSTR name)
    {
        PBoolMapIt it = bools.find(name);
        R_ASSERT(it != bools.end(), name);
        return it->second;
    }

    PBool* _bool_safe(LPCSTR name)
    {
        PBoolMapIt it = bools.find(name);
        return (it != bools.end()) ? &it->second : nullptr;
    }

    virtual void Compile(IWriter& F) = 0;

    virtual bool Load(IReader& F);
    virtual void Save(IWriter& F);

    virtual void Load2(CInifile& ini, gsl::czstring sect);
    virtual void Save2(CInifile& ini, gsl::czstring sect);
};

inline EParticleAction::~EParticleAction() = default;

struct EPAAvoid : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAAvoid, EParticleAction);

public:
    EPAAvoid();
    virtual void Compile(IWriter& F);
};

struct EPABounce : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPABounce, EParticleAction);

public:
    EPABounce();
    virtual void Compile(IWriter& F);
};

struct EPACopyVertexB : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPACopyVertexB, EParticleAction);

public:
    EPACopyVertexB();
    virtual void Compile(IWriter& F);
};

struct EPADamping : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPADamping, EParticleAction);

public:
    EPADamping();
    virtual void Compile(IWriter& F);
};

struct EPAExplosion : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAExplosion, EParticleAction);

public:
    EPAExplosion();
    virtual void Compile(IWriter& F);
};

struct EPAFollow : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAFollow, EParticleAction);

public:
    EPAFollow();
    virtual void Compile(IWriter& F);
};

struct EPAGravitate : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAGravitate, EParticleAction);

public:
    EPAGravitate();
    virtual void Compile(IWriter& F);
};

struct EPAGravity : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAGravity, EParticleAction);

public:
    EPAGravity();
    virtual void Compile(IWriter& F);
};

struct EPAJet : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAJet, EParticleAction);

public:
    EPAJet();
    virtual void Compile(IWriter& F);
};

struct EPAKillOld : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAKillOld, EParticleAction);

public:
    EPAKillOld();
    virtual void Compile(IWriter& F);
};

struct EPAMatchVelocity : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAMatchVelocity, EParticleAction);

public:
    EPAMatchVelocity();
    virtual void Compile(IWriter& F);
};

struct EPAMove : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAMove, EParticleAction);

public:
    EPAMove();
    virtual void Compile(IWriter& F);
};

struct EPAOrbitLine : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAOrbitLine, EParticleAction);

public:
    EPAOrbitLine();
    virtual void Compile(IWriter& F);
};

struct EPAOrbitPoint : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAOrbitPoint, EParticleAction);

public:
    EPAOrbitPoint();
    virtual void Compile(IWriter& F);
};

struct EPARandomAccel : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPARandomAccel, EParticleAction);

public:
    EPARandomAccel();
    virtual void Compile(IWriter& F);
};

struct EPARandomDisplace : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPARandomDisplace, EParticleAction);

public:
    EPARandomDisplace();
    virtual void Compile(IWriter& F);
};

struct EPARandomVelocity : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPARandomVelocity, EParticleAction);

public:
    EPARandomVelocity();
    virtual void Compile(IWriter& F);
};

struct EPARestore : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPARestore, EParticleAction);

public:
    EPARestore();
    virtual void Compile(IWriter& F);
};

struct EPAScatter : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAScatter, EParticleAction);

public:
    EPAScatter();
    virtual void Compile(IWriter& F);
};

struct EPASink : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPASink, EParticleAction);

public:
    EPASink();
    virtual void Compile(IWriter& F);
};

struct EPASinkVelocity : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPASinkVelocity, EParticleAction);

public:
    EPASinkVelocity();
    virtual void Compile(IWriter& F);
};

struct EPASpeedLimit : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPASpeedLimit, EParticleAction);

public:
    EPASpeedLimit();
    virtual void Compile(IWriter& F);
};

struct EPASource : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPASource, EParticleAction);

public:
    EPASource();
    virtual void Compile(IWriter& F);
};

struct EPATargetColor : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPATargetColor, EParticleAction);

public:
    EPATargetColor();
    virtual void Compile(IWriter& F);
};

struct EPATargetSize : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPATargetSize, EParticleAction);

public:
    EPATargetSize();
    virtual void Compile(IWriter& F);
};

struct EPATargetRotate : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPATargetRotate, EParticleAction);

public:
    EPATargetRotate();
    virtual void Compile(IWriter& F);
};

struct EPATargetVelocity : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPATargetVelocity, EParticleAction);

public:
    EPATargetVelocity();
    virtual void Compile(IWriter& F);
};

struct EPAVortex : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPAVortex, EParticleAction);

public:
    EPAVortex();
    virtual void Compile(IWriter& F);
};

struct EPATurbulence : public EParticleAction
{
    RTTI_DECLARE_TYPEINFO(EPATurbulence, EParticleAction);

public:
    float*** nval;
    float age;

    EPATurbulence();

    virtual void Compile(IWriter& F);
};

typedef EParticleAction* (*_CreateEAction)(PAPI::PActionEnum type);
extern _CreateEAction pCreateEAction;

//---------------------------------------------------------------------------

#endif
