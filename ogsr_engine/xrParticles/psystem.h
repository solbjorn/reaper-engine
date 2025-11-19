#pragma once

// Actually this must be < sqrt(MAXFLOAT) since we store this value squared.
constexpr inline f32 P_MAXFLOAT{1.0e16f};
constexpr inline s32 P_MAXINT{std::numeric_limits<s32>::max()};

#define drand48() ::Random.randF()

namespace PAPI
{
class pVector : public Fvector
{
public:
    constexpr pVector() = default;
    constexpr explicit pVector(f32 ax, f32 ay, f32 az) { set(ax, ay, az); }

    constexpr pVector(const pVector&) = default;
    constexpr pVector(pVector&&) = default;

    constexpr pVector& operator=(const pVector&) = default;
    constexpr pVector& operator=(pVector&&) = default;

    [[nodiscard]] constexpr f32 length() const { return _sqrt(x * x + y * y + z * z); }
    [[nodiscard]] constexpr f32 length2() const { return (x * x + y * y + z * z); }
    [[nodiscard]] constexpr f32 operator*(const pVector& a) const { return x * a.x + y * a.y + z * a.z; }
    constexpr pVector operator*(f32 s) const { return pVector(x * s, y * s, z * s); }

    constexpr pVector operator/(f32 s) const
    {
        f32 invs = 1.0f / s;
        return pVector(x * invs, y * invs, z * invs);
    }

    constexpr pVector operator+(const pVector& a) const { return pVector(x + a.x, y + a.y, z + a.z); }
    constexpr pVector operator-(const pVector& a) const { return pVector(x - a.x, y - a.y, z - a.z); }

    constexpr pVector operator-()
    {
        x = -x;
        y = -y;
        z = -z;

        return *this;
    }

    constexpr pVector& operator+=(const pVector& a)
    {
        x += a.x;
        y += a.y;
        z += a.z;

        return *this;
    }

    constexpr pVector& operator-=(const pVector& a)
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;

        return *this;
    }

    constexpr pVector& operator*=(f32 a)
    {
        x *= a;
        y *= a;
        z *= a;

        return *this;
    }

    constexpr pVector& operator/=(f32 a)
    {
        f32 b = 1.0f / a;
        x *= b;
        y *= b;
        z *= b;

        return *this;
    }

    constexpr pVector operator^(const pVector& b) const { return pVector(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};

// A single particle
struct Rotation
{
    f32 x;
};

struct Particle
{
    enum
    {
        ANIMATE_CCW = (1 << 0),
    };

    Rotation rot; // 4
    pVector pos; // 12
    pVector posB; // 12
    pVector vel; // 12
    pVector size; // 12
    f32 colorR; // 4
    f32 colorG; // 4
    f32 colorB; // 4
    f32 colorA; // 4
    f32 age; // 4
    u16 frame; // 2
    Flags16 flags; // 2
}; // = 76

typedef void (*OnBirthParticleCB)(void* owner, u32 param, PAPI::Particle& P, u32 idx);
typedef void (*OnDeadParticleCB)(void* owner, u32 param, PAPI::Particle& P, u32 idx);

//////////////////////////////////////////////////////////////////////
// Type codes for domains
enum PDomainEnum : u32
{
    PDPoint = 0, // Single point
    PDLine = 1, // Line segment
    PDTriangle = 2, // Triangle
    PDPlane = 3, // Arbitrarily-oriented plane
    PDBox = 4, // Axis-aligned box
    PDSphere = 5, // Sphere
    PDCylinder = 6, // Cylinder
    PDCone = 7, // Cone
    PDBlob = 8, // Gaussian blob
    PDDisc = 9, // Arbitrarily-oriented disc
    PDRectangle = 10, // Rhombus-shaped planar region
};

//////////////////////////////////////////////////////////////////////
// Type codes for all actions
enum PActionEnum : u32
{
    PAAvoidID, // Avoid entering the domain of space.
    PABounceID, // Bounce particles off a domain of space.
    PACallActionListID_obsolette, //
    PACopyVertexBID, // Set the secondary position from current position.
    PADampingID, // Dampen particle velocities.
    PAExplosionID, // An Explosion.
    PAFollowID, // Accelerate toward the previous particle in the effect.
    PAGravitateID, // Accelerate each particle toward each other particle.
    PAGravityID, // Acceleration in the given direction.
    PAJetID, //
    PAKillOldID, //
    PAMatchVelocityID, //
    PAMoveID, //
    PAOrbitLineID, //
    PAOrbitPointID, //
    PARandomAccelID, //
    PARandomDisplaceID, //
    PARandomVelocityID, //
    PARestoreID, //
    PASinkID, //
    PASinkVelocityID, //
    PASourceID, //
    PASpeedLimitID, //
    PATargetColorID, //
    PATargetSizeID, //
    PATargetRotateID, //
    PATargetRotateDID, //
    PATargetVelocityID, //
    PATargetVelocityDID, //
    PAVortexID, //
    PATurbulenceID, //
    PAScatterID, //
};

struct ParticleAction;

class XR_NOVTABLE IParticleManager : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IParticleManager);

public:
    IParticleManager() = default;
    ~IParticleManager() override = 0;

    // create&destroy
    virtual int CreateEffect(u32 max_particles) = 0;
    virtual void DestroyEffect(int effect_id) = 0;
    virtual int CreateActionList() = 0;
    virtual void DestroyActionList(int alist_id) = 0;

    // control
    virtual void PlayEffect(int alist_id) = 0;
    virtual void StopEffect(int effect_id, int alist_id, BOOL deffered = TRUE) = 0;

    // update&render
    virtual void Update(int effect_id, int alist_id, float dt) = 0;
    virtual void Render(int effect_id) = 0;
    virtual void Transform(int alist_id, const Fmatrix& m, const Fvector& velocity) = 0;

    // effect
    virtual void RemoveParticle(int effect_id, u32 p_id) = 0;
    virtual void SetMaxParticles(int effect_id, u32 max_particles) = 0;
    virtual void SetCallback(int effect_id, OnBirthParticleCB b, OnDeadParticleCB d, void* owner, u32 param) = 0;
    virtual void GetParticles(int effect_id, Particle*& particles, u32& cnt) = 0;
    virtual u32 GetParticlesCount(int effect_id) = 0;

    // action
    virtual ParticleAction* CreateAction(PActionEnum type) = 0;
    virtual u32 LoadActions(int alist_id, IReader& R, bool copFormat) = 0;
};

inline IParticleManager::~IParticleManager() = default;

IParticleManager* ParticleManager();
} // namespace PAPI
