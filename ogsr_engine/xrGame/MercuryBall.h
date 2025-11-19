///////////////////////////////////////////////////////////////
// MercuryBall.h
// MercuryBall - переливающийся и колыхающийся шар
// перекатывается с места на место
///////////////////////////////////////////////////////////////

#pragma once

#include "artifact.h"

class CMercuryBall : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CMercuryBall, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CMercuryBall();
    ~CMercuryBall() override;

    virtual void Load(LPCSTR section);

protected:
    virtual void UpdateCLChild();

    // время последнего обновления поведения шара
    ALife::_TIME_ID m_timeLastUpdate;
    // время между апдейтами
    ALife::_TIME_ID m_timeToUpdate;

    // диапазон импульсов катания шара
    float m_fImpulseMin;
    float m_fImpulseMax;
};
XR_SOL_BASE_CLASSES(CMercuryBall);

/*
///////////////////////////////////////////////////////////////////////////////////////////////////
// Ртутный шар
// Появляется после выброса, держится недолго, после чего испаряется.
// Цены:  от 50 до 200 рублей, в зависимости от размера
// Специфика: опасное аномальное образование, хранить только в защищенном контейнере,
// например в капсуле R1.
*/
