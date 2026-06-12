#pragma once

#include "../action_planner.h"
#include "../property_evaluator_const.h"

class CUIMapWnd;
class CUICustomMap;
class CUILevelMap;

class CMapActionPlanner : public CActionPlanner<CUIMapWnd, true>
{
    RTTI_DECLARE_TYPEINFO(CMapActionPlanner, CActionPlanner<CUIMapWnd, true>);

private:
    typedef CActionPlanner<CUIMapWnd, true> inherited;

public:
    CMapActionPlanner();
    ~CMapActionPlanner() override;

    void setup(CUIMapWnd* object) override;
    [[nodiscard]] virtual gsl::czstring object_name() const;
};
