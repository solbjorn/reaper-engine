#ifndef _CPS_Instance_H_
#define _CPS_Instance_H_

#include "../xrcdb/ispatial.h"
#include "isheduled.h"
#include "irenderable.h"

class XR_NOVTABLE CPS_Instance : public ISpatial, public ISheduled, public IRenderable
{
    RTTI_DECLARE_TYPEINFO(CPS_Instance, ISpatial, ISheduled, IRenderable);

private:
    friend class IGame_Persistent;

protected:
    int m_iLifeTime{std::numeric_limits<int>::max()};
    bool m_bAutoRemove{true};
    bool m_bDead{false};

private:
    bool m_destroy_on_game_load;

protected:
    virtual void PSI_internal_delete();

public:
    explicit CPS_Instance(bool destroy_on_game_load);
    ~CPS_Instance() override;

    [[nodiscard]] bool destroy_on_game_load() const { return m_destroy_on_game_load; }
    virtual void PSI_destroy();
    IC BOOL PSI_alive() { return m_iLifeTime > 0; }
    IC BOOL PSI_IsAutomatic() { return m_bAutoRemove; }
    IC void PSI_SetLifeTime(float life_time) { m_iLifeTime = iFloor(life_time * 1000); }

    virtual void Play(BOOL hudMode = FALSE) = 0;
    virtual BOOL Locked() { return FALSE; }

    virtual shared_str shedule_Name() const { return shared_str("particle_instance"); }

    virtual void shedule_Update(u32 dt);
    virtual IRenderable* dcast_Renderable() { return this; }
};

#endif
