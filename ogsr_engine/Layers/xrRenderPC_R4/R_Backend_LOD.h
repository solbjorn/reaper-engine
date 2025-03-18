#ifndef R_BACKEND_LOD_H_INCLUDED
#define R_BACKEND_LOD_H_INCLUDED

class CBackend;

class R_LOD
{
public:
    R_constant* c_LOD;

    R_LOD();

    void unmap() { c_LOD = 0; }
    void set_LOD(R_constant* C) { c_LOD = C; }
    void set_LOD(float LOD);

private:
    ICF CBackend& cmd_list();
};

#endif // #ifndef R_BACKEND_LOD_H_INCLUDED
