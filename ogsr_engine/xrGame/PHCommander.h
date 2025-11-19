#ifndef PH_COMMANDER_H
#define PH_COMMANDER_H

class CPHReqBase;
class CPHReqComparerV;

class XR_NOVTABLE CPHReqBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPHReqBase);

public:
    ~CPHReqBase() override = 0;

    [[nodiscard]] virtual bool obsolete() const = 0;
    [[nodiscard]] virtual bool compare(const CPHReqComparerV*) const { return false; }
};

inline CPHReqBase::~CPHReqBase() = default;

class XR_NOVTABLE CPHCondition : public CPHReqBase
{
    RTTI_DECLARE_TYPEINFO(CPHCondition, CPHReqBase);

public:
    ~CPHCondition() override = 0;

    [[nodiscard]] virtual bool is_true() = 0;
};

inline CPHCondition::~CPHCondition() = default;

class XR_NOVTABLE CPHAction : public CPHReqBase
{
    RTTI_DECLARE_TYPEINFO(CPHAction, CPHReqBase);

public:
    ~CPHAction() override = 0;

    virtual void run() = 0;
};

inline CPHAction::~CPHAction() = default;

class CPHOnesCondition : public CPHCondition
{
    RTTI_DECLARE_TYPEINFO(CPHOnesCondition, CPHCondition);

public:
    bool b_called{};

    CPHOnesCondition() = default;
    ~CPHOnesCondition() override = default;

    [[nodiscard]] bool is_true() override
    {
        b_called = true;
        return true;
    }

    [[nodiscard]] bool obsolete() const override { return b_called; }
};

class CPHDummiAction : public CPHAction
{
    RTTI_DECLARE_TYPEINFO(CPHDummiAction, CPHAction);

public:
    ~CPHDummiAction() override = default;

    void run() override {}
    [[nodiscard]] bool obsolete() const override { return false; }
};

class CPHCall
{
    CPHAction* m_action;
    CPHCondition* m_condition;

    bool removed;
    u32 paused;

public:
    explicit CPHCall(CPHCondition* condition, CPHAction* action);
    ~CPHCall();

    void check();
    [[nodiscard]] bool obsolete();
    [[nodiscard]] bool equal(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    [[nodiscard]] bool is_any(CPHReqComparerV* v);

    [[nodiscard]] bool isPaused();
    [[nodiscard]] bool isNeedRemove();
    void removeLater();
    void setPause(u32 ms);
};

DEFINE_VECTOR(std::unique_ptr<CPHCall>, PHCALL_STORAGE, PHCALL_I);

class CPHCommander
{
    PHCALL_STORAGE m_calls;

public:
    ~CPHCommander();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] CPHCall* add_call_unique(CPHCondition* condition, CPHReqComparerV* cmp_condition, CPHAction* action, CPHReqComparerV* cmp_action);
    [[nodiscard]] CPHCall* add_call(CPHCondition* condition, CPHAction* action);

    [[nodiscard]] bool has_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    [[nodiscard]] PHCALL_I find_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    void remove_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    void remove_calls(CPHReqComparerV* cmp_object);

    void update();
    void clear();
};

#endif
