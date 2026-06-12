////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
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
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorALife : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorALife, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorALife(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorALife() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAlive : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorAlive, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorAlive(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorAlive() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItems
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItems : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorItems, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorItems(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorItems() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemies
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemies : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorEnemies, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

protected:
    u32 m_time_to_wait;
    const bool* m_dont_wait;

public:
    explicit CStalkerPropertyEvaluatorEnemies(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "", u32 time_to_wait = 0, const bool* dont_wait = nullptr);
    ~CStalkerPropertyEvaluatorEnemies() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSeeEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorSeeEnemy : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorSeeEnemy, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorSeeEnemy(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorSeeEnemy() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemySeeMe
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemySeeMe : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorEnemySeeMe, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorEnemySeeMe(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorEnemySeeMe() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItemToKill : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorItemToKill, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorItemToKill(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorItemToKill() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemCanKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItemCanKill : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorItemCanKill, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorItemCanKill(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorItemCanKill() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorFoundItemToKill : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorFoundItemToKill, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorFoundItemToKill(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorFoundItemToKill() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundAmmo
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorFoundAmmo : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorFoundAmmo, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorFoundAmmo(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorFoundAmmo() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorReadyToKill : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorReadyToKill, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorReadyToKill(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorReadyToKill() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToDetour
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorReadyToDetour : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorReadyToDetour, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorReadyToDetour(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorReadyToDetour() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAnomaly : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorAnomaly, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorAnomaly(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorAnomaly() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorInsideAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorInsideAnomaly : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorInsideAnomaly, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorInsideAnomaly(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorInsideAnomaly() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPanic
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorPanic : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorPanic, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorPanic(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorPanic() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSmartTerrainTask
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorSmartTerrainTask : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorSmartTerrainTask, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorSmartTerrainTask(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorSmartTerrainTask() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyReached
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemyReached : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorEnemyReached, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorEnemyReached(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorEnemyReached() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPlayerOnThePath
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorPlayerOnThePath : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorPlayerOnThePath, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorPlayerOnThePath(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorPlayerOnThePath() override = default;

    [[nodiscard]] _value_type evaluate() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyCriticallyWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemyCriticallyWounded : public CStalkerPropertyEvaluator
{
    RTTI_DECLARE_TYPEINFO(CStalkerPropertyEvaluatorEnemyCriticallyWounded, CStalkerPropertyEvaluator);

protected:
    typedef CStalkerPropertyEvaluator inherited;

public:
    explicit CStalkerPropertyEvaluatorEnemyCriticallyWounded(CAI_Stalker* object = nullptr, LPCSTR evaluator_name = "");
    ~CStalkerPropertyEvaluatorEnemyCriticallyWounded() override = default;

    [[nodiscard]] _value_type evaluate() override;
};
