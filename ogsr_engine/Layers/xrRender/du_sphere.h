#ifndef du_sphereH
#define du_sphereH

//---------------------------------------------------------------------------
constexpr inline u32 DU_SPHERE_NUMVERTEX{92};
constexpr inline u32 DU_SPHERE_NUMFACES{180};
constexpr inline u32 DU_SPHERE_NUMVERTEXL{60};
constexpr inline u32 DU_SPHERE_NUMLINES{60};

extern const std::array<Fvector, DU_SPHERE_NUMVERTEX> XR_ALIGNED_DEFAULT du_sphere_vertices;
extern const std::array<u16, DU_SPHERE_NUMFACES * 3> XR_ALIGNED_DEFAULT du_sphere_faces;
extern const std::array<Fvector, DU_SPHERE_NUMVERTEXL> XR_ALIGNED_DEFAULT du_sphere_verticesl;
extern const std::array<u16, DU_SPHERE_NUMLINES * 2> XR_ALIGNED_DEFAULT du_sphere_lines;

#endif
