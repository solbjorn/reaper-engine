#ifndef r_backend_treeH
#define r_backend_treeH

class CBackend;

class R_tree final
{
public:
    R_constant* c_m_xform_v;
    R_constant* c_m_xform;
    R_constant* c_consts;
    R_constant* c_wave;
    R_constant* c_wind;
    R_constant* c_c_scale;
    R_constant* c_c_bias;
    R_constant* c_c_sun;

    R_tree();
    void unmap();

    void set_c_m_xform_v(R_constant* C) { c_m_xform_v = C; }
    void set_c_m_xform(R_constant* C) { c_m_xform = C; }
    void set_c_consts(R_constant* C) { c_consts = C; }
    void set_c_wave(R_constant* C) { c_wave = C; }
    void set_c_wind(R_constant* C) { c_wind = C; }
    void set_c_c_scale(R_constant* C) { c_c_scale = C; }
    void set_c_c_bias(R_constant* C) { c_c_bias = C; }
    void set_c_c_sun(R_constant* C) { c_c_sun = C; }

    void set_m_xform_v(CBackend& cmd_list, const Fmatrix& mat);
    void set_m_xform(CBackend& cmd_list, const Fmatrix& mat);
    void set_consts(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w);
    void set_wave(CBackend& cmd_list, const Fvector4& vec);
    void set_wind(CBackend& cmd_list, const Fvector4& vec);
    void set_c_scale(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w);
    void set_c_bias(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w);
    void set_c_sun(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w);
};

#endif
