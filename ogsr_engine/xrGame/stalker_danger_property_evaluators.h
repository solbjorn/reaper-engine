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

class CStalkerPropertyEvaluatorDangers final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangers, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangers(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangers() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerUnknown
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerUnknown final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerUnknown, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerUnknown(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerUnknown() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerInDirection
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerInDirection final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerInDirection, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerInDirection(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerInDirection() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerWithGrenade
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerWithGrenade final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerWithGrenade, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerWithGrenade(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerWithGrenade() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerBySound
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerBySound final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerBySound, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerBySound(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerBySound() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerUnknownCoverActual
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerUnknownCoverActual final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerUnknownCoverActual, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

private:
    Fvector m_cover_selection_position;

public:
    explicit CStalkerPropertyEvaluatorDangerUnknownCoverActual(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerUnknownCoverActual() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerGrenadeExploded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerGrenadeExploded final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorDangerGrenadeExploded, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorDangerGrenadeExploded(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorDangerGrenadeExploded() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorGrenadeToExplode
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorGrenadeToExplode final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorGrenadeToExplode, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorGrenadeToExplode(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorGrenadeToExplode() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemyWounded final : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorEnemyWounded, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorEnemyWounded(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorEnemyWounded() override = default;

    [[nodiscard]] _value_type evaluate() override;
};
