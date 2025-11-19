#pragma once

#include "../control_path_builder.h"

class CPoltergeist;
class CCustomMonster;

class CPoltergeisMovementManager : public CControlPathBuilder
{
    RTTI_DECLARE_TYPEINFO(CPoltergeisMovementManager, CControlPathBuilder);

private:
    typedef CControlPathBuilder inherited;

    CPoltergeist* m_monster;

public:
    explicit CPoltergeisMovementManager(CPoltergeist* monster) : inherited((CCustomMonster*)monster), m_monster(monster) {}
    ~CPoltergeisMovementManager() override = default;

    virtual void move_along_path(CPHMovementControl* movement_control, Fvector& dest_position, float time_delta);

    Fvector CalculateRealPosition();
};
