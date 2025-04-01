#ifndef du_sphereH
#define du_sphereH

//---------------------------------------------------------------------------
constexpr u32 DU_SPHERE_NUMVERTEX = 92;
constexpr u32 DU_SPHERE_NUMFACES = 180;
constexpr u32 DU_SPHERE_NUMVERTEXL = 60;
constexpr u32 DU_SPHERE_NUMLINES = 60;

extern const Fvector du_sphere_vertices[];
extern const WORD du_sphere_faces[];
extern const Fvector du_sphere_verticesl[];
extern const WORD du_sphere_lines[];

#endif
