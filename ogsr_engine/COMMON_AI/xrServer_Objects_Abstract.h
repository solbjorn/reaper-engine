////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.h
//	Created 	: 19.09.2002
//  Modified 	: 18.06.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Space.h"
#ifdef XRGAME_EXPORTS
#include "../../xrCDB/xrCDB.h"
#else // XRGAME_EXPORTS
#include "../xrCDB/xrCDB.h"
#endif // XRGAME_EXPORTS
#include "ShapeData.h"

class NET_Packet;
class CDUInterface;

#ifndef XRGAME_EXPORTS
#include "Sound.h"
#endif

#include "..\..\Include\xrRender\DrawUtils.h"

class XR_NOVTABLE ISE_Shape : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ISE_Shape);

public:
    ~ISE_Shape() override = 0;

    virtual void assign_shapes(CShapeData::shape_def* shapes, u32 cnt) = 0;
};

inline ISE_Shape::~ISE_Shape() = default;

class XR_NOVTABLE CSE_Visual : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CSE_Visual);

public:
    DECLARE_SCRIPT_REGISTER_FUNCTION();

    shared_str visual_name;
    shared_str startup_animation;
    enum
    {
        flObstacle = (1 << 0)
    };
    Flags8 flags;

public:
    explicit CSE_Visual(LPCSTR name = nullptr);
    ~CSE_Visual() override;

    void visual_read(NET_Packet& P, u16 version);
    void visual_write(NET_Packet& P);

    void set_visual(LPCSTR name);
    [[nodiscard]] gsl::czstring get_visual() const { return visual_name.c_str(); }

    virtual CSE_Visual* visual() = 0;
};

add_to_type_list(CSE_Visual);
#undef script_type_list
#define script_type_list save_type_list(CSE_Visual)

class XR_NOVTABLE CSE_Motion : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CSE_Motion);

public:
    DECLARE_SCRIPT_REGISTER_FUNCTION();

    shared_str motion_name;

    explicit CSE_Motion(LPCSTR name = nullptr);
    ~CSE_Motion() override;

    void motion_read(NET_Packet& P);
    void motion_write(NET_Packet& P);

    void set_motion(LPCSTR name);
    [[nodiscard]] gsl::czstring get_motion() const { return motion_name.c_str(); }

    virtual CSE_Motion* motion() = 0;
};

add_to_type_list(CSE_Motion);
#undef script_type_list
#define script_type_list save_type_list(CSE_Motion)

struct XR_NOVTABLE ISE_AbstractLEOwner : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ISE_AbstractLEOwner);

public:
    ~ISE_AbstractLEOwner() override = 0;

    virtual void get_bone_xform(LPCSTR name, Fmatrix& xform) = 0;
};

inline ISE_AbstractLEOwner::~ISE_AbstractLEOwner() = default;

struct XR_NOVTABLE ISE_Abstract : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ISE_Abstract);

public:
    enum
    {
        flUpdateProperties = u32(1 << 0),
        flVisualChange = u32(1 << 1),
        flVisualAnimationChange = u32(1 << 2),
        flMotionChange = u32(1 << 3),
    };
    Flags32 m_editor_flags;
    IC void set_editor_flag(u32 mask) { m_editor_flags.set(mask, TRUE); }

    ~ISE_Abstract() override = 0;

    virtual void Spawn_Write(NET_Packet& tNetPacket, BOOL bLocal) = 0;
    [[nodiscard]] virtual BOOL Spawn_Read(NET_Packet& tNetPacket) = 0;
    [[nodiscard]] virtual gsl::czstring name() const = 0;
    virtual void set_name(gsl::czstring) = 0;
    [[nodiscard]] virtual gsl::czstring name_replace() const = 0;
    virtual void set_name_replace(gsl::czstring) = 0;
    [[nodiscard]] virtual Fvector3& position() = 0;
    [[nodiscard]] virtual Fvector3& angle() = 0;
    [[nodiscard]] virtual Flags16& flags() = 0;
    [[nodiscard]] virtual ISE_Shape* shape() = 0;
    [[nodiscard]] virtual CSE_Visual* visual() = 0;
    [[nodiscard]] virtual CSE_Motion* motion() = 0;
    [[nodiscard]] virtual bool validate() = 0;
    virtual void on_render(CDUInterface* du, ISE_AbstractLEOwner* owner, bool bSelected, const Fmatrix& parent, s32 priority, bool strictB2F) = 0;
};

inline ISE_Abstract::~ISE_Abstract() = default;
