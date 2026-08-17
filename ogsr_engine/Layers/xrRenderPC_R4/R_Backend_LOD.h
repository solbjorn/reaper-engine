#ifndef R_BACKEND_LOD_H_INCLUDED
#define R_BACKEND_LOD_H_INCLUDED

class CBackend;

class R_LOD final
{
public:
    R_constant* c_LOD;

    R_LOD();

    void unmap() { c_LOD = nullptr; }
    void set_LOD(R_constant* C) { c_LOD = C; }

    void set_LOD(CBackend& cmd_list, f32 LOD);
};

#endif // R_BACKEND_LOD_H_INCLUDED
