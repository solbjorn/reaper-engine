#ifndef r_backend_hemiH
#define r_backend_hemiH

class CBackend;

class R_hemi final
{
public:
    R_constant* c_pos_faces;
    R_constant* c_neg_faces;
    R_constant* c_material;

    R_hemi();
    void unmap();

    void set_c_pos_faces(R_constant* C) { c_pos_faces = C; }
    void set_c_neg_faces(R_constant* C) { c_neg_faces = C; }
    void set_c_material(R_constant* C) { c_material = C; }

    void set_pos_faces(CBackend& cmd_list, f32 posx, f32 posy, f32 posz);
    void set_neg_faces(CBackend& cmd_list, f32 negx, f32 negy, f32 negz);
    void set_material(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w);
};

#endif
