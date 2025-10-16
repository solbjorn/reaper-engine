#pragma once

class CPHScriptCondition;
class CPHScriptAction;
class CPHScriptObjectCondition;
class CPHScriptObjectAction;
class CPHScriptObjectConditionN;
class CPHScriptObjectActionN;
class CPHScriptGameObjectCondition;
class CPHScriptGameObjectAction;
class CPHConstForceAction;

class CPHParticlesPlayCall;
class CPHParticlesCondition;
class CPHFindParticlesComparer;

class CPHReqComparerV : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPHReqComparerV);

public:
    virtual bool compare(const CPHScriptCondition*) const { return false; }
    virtual bool compare(const CPHScriptAction*) const { return false; }
    virtual bool compare(const CPHScriptObjectCondition*) const { return false; }
    virtual bool compare(const CPHScriptObjectAction*) const { return false; }
    virtual bool compare(const CPHScriptObjectConditionN*) const { return false; }
    virtual bool compare(const CPHScriptObjectActionN*) const { return false; }
    virtual bool compare(const CPHScriptGameObjectAction*) const { return false; }
    virtual bool compare(const CPHScriptGameObjectCondition*) const { return false; }
    virtual bool compare(const CPHConstForceAction*) const { return false; }

    virtual bool compare(const CPHParticlesPlayCall*) const { return false; }
    virtual bool compare(const CPHParticlesCondition*) const { return false; }
    virtual bool compare(const CPHFindParticlesComparer*) const { return false; }
};
