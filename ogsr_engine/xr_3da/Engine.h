// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "EngineAPI.h"
#include "EventAPI.h"

class light;

#include "xrSheduler.h"

class CEngine
{
public:
    // DLL api stuff
    CEngineAPI External;
    CEventAPI Event;
    CSheduler Sheduler;

    void Initialize();
    void Destroy();

    CEngine() = default;
    ~CEngine() = default;
};

extern CEngine Engine;
