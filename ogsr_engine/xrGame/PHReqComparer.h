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
    virtual bool compare(const CPHScriptCondition* v) const { return false; }
    virtual bool compare(const CPHScriptAction* v) const { return false; }
    virtual bool compare(const CPHScriptObjectCondition* v) const { return false; }
    virtual bool compare(const CPHScriptObjectAction* v) const { return false; }
    virtual bool compare(const CPHScriptObjectConditionN* v) const { return false; }
    virtual bool compare(const CPHScriptObjectActionN* v) const { return false; }
    virtual bool compare(const CPHScriptGameObjectAction* v) const { return false; }
    virtual bool compare(const CPHScriptGameObjectCondition* v) const { return false; }
    virtual bool compare(const CPHConstForceAction* v) const { return false; }

    virtual bool compare(const CPHParticlesPlayCall* v) const { return false; }
    virtual bool compare(const CPHParticlesCondition* v) const { return false; }
    virtual bool compare(const CPHFindParticlesComparer* v) const { return false; }
};
