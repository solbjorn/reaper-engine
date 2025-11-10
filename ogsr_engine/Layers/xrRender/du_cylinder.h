#ifndef du_cylinderH
#define du_cylinderH

//---------------------------------------------------------------------------
constexpr inline u32 DU_CYLINDER_NUMVERTEX{26};
constexpr inline u32 DU_CYLINDER_NUMFACES{48};
constexpr inline u32 DU_CYLINDER_NUMLINES{30};

extern const std::array<Fvector, DU_CYLINDER_NUMVERTEX> XR_ALIGNED_DEFAULT du_cylinder_vertices;
extern const std::array<u16, DU_CYLINDER_NUMFACES * 3> XR_ALIGNED_DEFAULT du_cylinder_faces;
extern const std::array<u16, DU_CYLINDER_NUMLINES * 2> XR_ALIGNED_DEFAULT du_cylinder_lines;

#endif
