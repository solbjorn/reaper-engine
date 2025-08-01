////////////////////////////////////////////////////////////////////////////
//  Created     : 19.06.2018
//  Authors     : Xottab_DUTY (OpenXRay project)
//                FozeSt
//                Unfainthful
//
//  Copyright (C) GSC Game World - 2018
////////////////////////////////////////////////////////////////////////////
#pragma once

class ILoadingScreen : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ILoadingScreen);

public:
    virtual ~ILoadingScreen() = default;

    virtual void Initialize() = 0;

    virtual void Show(bool status) = 0;
    virtual bool IsShown() = 0;

    virtual void Update(int stagesCompleted, int stagesTotal) = 0;
    virtual void ForceDrop() = 0;
    virtual void ForceFinish() = 0;

    virtual void SetLevelLogo(const char* name) = 0;
    virtual void SetLevelText(const char* name) = 0;
    virtual void SetStageTitle(const char* title) = 0;
    virtual void SetStageTip() = 0;
};
