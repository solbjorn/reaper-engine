////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_properties.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager properties
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CAgentManager;

typedef CPropertyEvaluator<CAgentManager> CAgentManagerPropertyEvaluator;
typedef CPropertyEvaluatorConst<CAgentManager> CAgentManagerPropertyEvaluatorConst;
typedef CPropertyEvaluatorMember<CAgentManager> CAgentManagerPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorItem
//////////////////////////////////////////////////////////////////////////

class CAgentManagerPropertyEvaluatorItem : public CAgentManagerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CAgentManagerPropertyEvaluatorItem, CAgentManagerPropertyEvaluator);

protected:
    typedef CAgentManagerPropertyEvaluator inherited;

public:
    inline explicit CAgentManagerPropertyEvaluatorItem(CAgentManager* object = nullptr, LPCSTR evaluator_name = "");
    ~CAgentManagerPropertyEvaluatorItem() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorEnemy
//////////////////////////////////////////////////////////////////////////

class CAgentManagerPropertyEvaluatorEnemy : public CAgentManagerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CAgentManagerPropertyEvaluatorEnemy, CAgentManagerPropertyEvaluator);

protected:
    typedef CAgentManagerPropertyEvaluator inherited;

public:
    inline explicit CAgentManagerPropertyEvaluatorEnemy(CAgentManager* object = nullptr, LPCSTR evaluator_name = "");
    ~CAgentManagerPropertyEvaluatorEnemy() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorDanger
//////////////////////////////////////////////////////////////////////////

class CAgentManagerPropertyEvaluatorDanger : public CAgentManagerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CAgentManagerPropertyEvaluatorDanger, CAgentManagerPropertyEvaluator);

protected:
    typedef CAgentManagerPropertyEvaluator inherited;

public:
    inline explicit CAgentManagerPropertyEvaluatorDanger(CAgentManager* object = nullptr, LPCSTR evaluator_name = "");
    ~CAgentManagerPropertyEvaluatorDanger() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

#include "agent_manager_properties_inline.h"
