////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_primary.h
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Primary function classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ef_base.h"

class CDistanceFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CDistanceFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CDistanceFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 3.0;
        m_fMaxResultValue = 20.0;
        strcat(m_caName, "Distance");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CDistanceFunction() {}

    virtual float ffGetValue();

    virtual u32 dwfGetDiscreteValue(u32 dwDiscretizationValue)
    {
        float fTemp = ffGetValue();
        if (fTemp <= m_fMinResultValue)
            return (0);
        else if (fTemp >= m_fMaxResultValue)
            return (dwDiscretizationValue - 1);
        else
            return (1);
    }
};

class CPersonalHealthFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalHealthFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalHealthFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalHealth");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual float ffGetValue();
};

class CPersonalMoraleFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalMoraleFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalMoraleFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalMorale");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalMoraleFunction() {}

    virtual float ffGetValue();
};

class CPersonalCreatureTypeFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalCreatureTypeFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalCreatureTypeFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 21.0;
        strcat(m_caName, "PersonalCreatureType");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalCreatureTypeFunction() {}

    virtual float ffGetValue();
};

class CPersonalWeaponTypeFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalWeaponTypeFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalWeaponTypeFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 12.0;
        strcat(m_caName, "PersonalWeaponType");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalWeaponTypeFunction() {}

    u32 dwfGetWeaponType();
    float ffGetTheBestWeapon();
    virtual float ffGetValue();
};

class CPersonalAccuracyFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalAccuracyFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalAccuracyFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalAccuracy");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalAccuracyFunction() {}

    virtual float ffGetValue();
};

class CPersonalIntelligenceFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalIntelligenceFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalIntelligenceFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalIntelligence");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalIntelligenceFunction() {}

    virtual float ffGetValue();
};

class CPersonalRelationFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalRelationFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalRelationFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalRelation");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalRelationFunction() {}

    virtual float ffGetValue();
};

class CPersonalGreedFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalGreedFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalGreedFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalGreed");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalGreedFunction() {}

    virtual float ffGetValue();
};

class CPersonalAggressivenessFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalAggressivenessFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalAggressivenessFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalAggressiveness");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalAggressivenessFunction() {}

    virtual float ffGetValue();
};

class CEnemyMoraleFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEnemyMoraleFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEnemyMoraleFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "EnemyMorale");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEnemyMoraleFunction() {}

    virtual float ffGetValue();
};

class CEnemyEquipmentCostFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEnemyEquipmentCostFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEnemyEquipmentCostFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 12.0;
        strcat(m_caName, "EnemyEquipmentCost");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEnemyEquipmentCostFunction() {}

    virtual float ffGetValue();
};

class CEnemyRukzakWeightFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEnemyRukzakWeightFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEnemyRukzakWeightFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 12.0;
        strcat(m_caName, "EnemyRukzakWeight");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEnemyRukzakWeightFunction() {}

    virtual float ffGetValue();
};

class CEnemyAnomalityFunction : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEnemyAnomalityFunction, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEnemyAnomalityFunction(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 12.0;
        strcat(m_caName, "EnemyAnomality");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEnemyAnomalityFunction() {}

    virtual float ffGetValue();
};

class CGraphPointType0 : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CGraphPointType0, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CGraphPointType0(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "GraphPointType0");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CGraphPointType0() {}

    virtual float ffGetValue();
};

class CPersonalEyeRange : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalEyeRange, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalEyeRange(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "PersonalEyeRange");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalEyeRange() {}

    virtual float ffGetValue();
};

class CPersonalMaxHealth : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CPersonalMaxHealth, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CPersonalMaxHealth(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 1000.0;
        strcat(m_caName, "PersonalMaxHealth");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CPersonalMaxHealth() {}

    virtual float ffGetValue();
    virtual u32 dwfGetDiscreteValue(u32 dwDiscretizationValue = 1);
};

class CEquipmentType : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEquipmentType, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEquipmentType(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 5.0;
        strcat(m_caName, "EquipmentType");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEquipmentType() {}

    virtual float ffGetValue();
};

class CItemDeterioration : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CItemDeterioration, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CItemDeterioration(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 100.0;
        strcat(m_caName, "ItemDeterioration");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CItemDeterioration() {}

    virtual float ffGetValue();
};

class CEquipmentPreference : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEquipmentPreference, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEquipmentPreference(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 3.0;
        strcat(m_caName, "EquipmentPreference");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEquipmentPreference() {}

    virtual float ffGetValue();
};

class CMainWeaponType : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CMainWeaponType, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CMainWeaponType(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 4.0;
        strcat(m_caName, "MainWeaponType");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CMainWeaponType() {}

    virtual float ffGetValue();
};

class CMainWeaponPreference : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CMainWeaponPreference, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CMainWeaponPreference(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 1.0;
        m_fMaxResultValue = 3.0;
        strcat(m_caName, "MainWeaponPreference");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CMainWeaponPreference() {}

    virtual float ffGetValue();
};

class CItemValue : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CItemValue, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CItemValue(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 100.0;
        m_fMaxResultValue = 2000.0;
        strcat(m_caName, "ItemValue");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CItemValue() {}

    virtual float ffGetValue();
};

class CWeaponAmmoCount : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CWeaponAmmoCount, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CWeaponAmmoCount(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 10.0;
        strcat(m_caName, "WeaponAmmoCount");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CWeaponAmmoCount() {}

    virtual float ffGetValue();
    virtual u32 dwfGetDiscreteValue(u32 dwDiscretizationValue = 1);
};

class CEnemyAnomalyType : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEnemyAnomalyType, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEnemyAnomalyType(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 7.0;
        strcat(m_caName, "EnemyAnomalyType");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEnemyAnomalyType() {}

    virtual float ffGetValue();
};

class CDetectorType : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CDetectorType, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CDetectorType(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 2.0;
        strcat(m_caName, "DetectorType");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CDetectorType() {}

    virtual float ffGetValue();
};

class CEnemyDistanceToGraphPoint : public CBaseFunction
{
    RTTI_DECLARE_TYPEINFO(CEnemyDistanceToGraphPoint, CBaseFunction);

public:
    typedef CBaseFunction inherited;

    CEnemyDistanceToGraphPoint(CEF_Storage* storage) : CBaseFunction(storage)
    {
        m_fMinResultValue = 0.0;
        m_fMaxResultValue = 4.0;
        strcat(m_caName, "EnemyDistanceToGraphPoint");
        OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized", m_caName);
    };

    virtual ~CEnemyDistanceToGraphPoint() {}

    virtual float ffGetValue();
};
