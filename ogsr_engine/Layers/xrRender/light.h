#ifndef LAYERS_XRRENDER_LIGHT_H_INCLUDED
#define LAYERS_XRRENDER_LIGHT_H_INCLUDED

#include "../../xrcdb/ispatial.h"

#include "light_package.h"
#include "light_smapvis.h"

extern Fvector4 ps_ssfx_volumetric;

#define MIN_VIRTUAL_SIZE 0.01f

class light : public IRender_Light, public ISpatial
{
public:
    struct
    {
        u32 type : 4;
        u32 bStatic : 1;
        u32 bActive : 1;
        u32 bShadow : 1;
        u32 bVolumetric : 1;
        u32 bVolumetricManual : 1;
        u32 bHudMode : 1;
        u32 bMoveable : 1;
    } flags{};
    Fvector position;
    Fvector direction;
    Fvector right;
    float range;
    float cone;
    Fcolor color;

    vis_data hom;
    u32 frame_render;

    int omnipart_num;
    int sss_id;
    int sss_refresh;
    s8 sss_priority;
    light* omipart_parent;
    float distance;
    float distance_lpos;

    float m_volumetric_quality;
    float m_volumetric_intensity;
    float m_volumetric_distance;

    float virtual_size;

    float falloff; // precalc to make light equal to zero at light range
    float attenuation0; // Constant attenuation
    float attenuation1; // Linear attenuation
    float attenuation2; // Quadratic attenuation

    light* omnipart[6];

    smapvis svis[R__NUM_CONTEXTS]; // used for 6-cubemap faces

    ref_shader s_spot;
    ref_shader s_point;

    ref_shader s_spot_msaa;
    ref_shader s_point_msaa;

    u32 m_xform_frame;
    Fmatrix m_xform;

    struct _vis
    {
        u32 frame2test; // frame the test is sheduled to
        u32 query_id; // ID of occlusion query
        u32 query_order; // order of occlusion query
        bool visible; // visible/invisible
        bool pending; // test is still pending
        u16 smap_ID;
        float distance;
    } vis;

    union _xform
    {
        struct _D
        {
            Fmatrix combine[R__NUM_SUN_CASCADES];
            s32 minX, maxX;
            s32 minY, maxY;
        } D;
        struct _P
        {
            Fmatrix world;
            Fmatrix view;
            Fmatrix project;
            Fmatrix combine;
        } P;
        struct _S
        {
            Fmatrix view;
            Fmatrix project;
            Fmatrix combine;
            u32 size;
            u32 posX;
            u32 posY;
        } S;
    } X;

    virtual void set_type(LT type) { flags.type = type; }
    virtual u32 get_type() const override { return flags.type; }
    virtual void set_active(bool b);
    virtual bool get_active() { return flags.bActive; }
    virtual void set_shadow(bool b) { flags.bShadow = b; }
    virtual void set_volumetric(bool b, bool manual = false)
    {
        flags.bVolumetricManual = manual;

        if (!manual && ps_ssfx_volumetric.x > 0)
            b = true;

        flags.bVolumetric = b;
    }

    virtual void set_volumetric_quality(float fValue) { m_volumetric_quality = fValue; }
    virtual void set_volumetric_intensity(float fValue) { m_volumetric_intensity = ps_ssfx_volumetric.y; }
    virtual void set_volumetric_distance(float fValue) { m_volumetric_distance = 1.0f; }

    virtual void set_position(const Fvector& P);
    virtual void set_rotation(const Fvector& D, const Fvector& R);
    virtual void set_cone(float angle);
    virtual void set_range(float R);
    float get_range() const override { return range; };
    virtual void set_virtual_size(float S) { virtual_size = (S > MIN_VIRTUAL_SIZE) ? S : MIN_VIRTUAL_SIZE; }

    virtual void set_color(const Fcolor& C) { color.set(C); }
    virtual void set_color(float r, float g, float b) { color.set(r, g, b, 1); }
    Fcolor get_color() const override { return color; };

    virtual void set_texture(LPCSTR name);
    virtual void set_hud_mode(bool b) { flags.bHudMode = b; }
    virtual bool get_hud_mode() { return flags.bHudMode; };

    virtual void set_moveable(bool b) override { flags.bMoveable = b; }

    virtual void spatial_move();
    virtual Fvector spatial_sector_point();

    virtual IRender_Light* dcast_Light() { return this; }

    vis_data& get_homdata();
    void xform_calc();
    void vis_prepare();
    void vis_update();
    void export_to(light_Package& dest);
    void set_attenuation_params(float a0, float a1, float a2, float fo);

    float get_LOD();

    light();
    virtual ~light();
};

#endif // #define LAYERS_XRRENDER_LIGHT_H_INCLUDED
