////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_header.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator header
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "alife_space.h"

class CALifeSimulatorHeader : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CALifeSimulatorHeader);

protected:
    u32 m_version{ALIFE_VERSION};

public:
    CALifeSimulatorHeader() = default;
    virtual ~CALifeSimulatorHeader();

    virtual void save(IWriter& tMemoryStream);
    virtual void load(IReader& tFileStream);

    [[nodiscard]] inline u32 version() const { return m_version; }
    bool valid(IReader& file_stream) const;
};
