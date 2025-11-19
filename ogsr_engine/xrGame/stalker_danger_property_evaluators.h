////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_property_evaluators.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger property evaluators classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wrapper_abstract.h"
#include "property_evaluator_const.h"
#include "property_evaluator_member.h"
#include "danger_object.h"

class CAI_Stalker;

typedef CWrapperAbstract2<CAI_Stalker, CPropertyEvaluator> CStalkerPropertyEvaluator;
typedef CWrapperAbstract2<CAI_Stalker, CPropertyEvaluatorConst> CStalkerPropertyEvaluatorConst;
typedef CWrapperAbstract2<CAI_Stalker, CPropertyEvaluatorMember> CStalkerPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangers
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangers : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangers, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangers(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangers() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerUnknown
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerUnknown : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerUnknown, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerUnknown(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerUnknown() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerInDirection
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerInDirection : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerInDirection, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerInDirection(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerInDirection() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerWithGrenade
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerWithGrenade : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerWithGrenade, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerWithGrenade(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerWithGrenade() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerBySound
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerBySound : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerBySound, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerBySound(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerBySound() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerUnknownCoverActual
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerUnknownCoverActual : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerUnknownCoverActual, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

private:
    Fvector m_cover_selection_position;

public:
    explicit CStalkerPropertyEvaluatorDangerUnknownCoverActual(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerUnknownCoverActual() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerGrenadeExploded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerGrenadeExploded : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerGrenadeExploded, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerGrenadeExploded(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerGrenadeExploded() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorGrenadeToExplode
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorGrenadeToExplode : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorGrenadeToExplode, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorGrenadeToExplode(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorGrenadeToExplode() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemyWounded : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorEnemyWounded, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorEnemyWounded(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorEnemyWounded() override = default;

    virtual _value_type evaluate();
};
