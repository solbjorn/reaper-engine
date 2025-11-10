#ifndef du_sphere_partH
#define du_sphere_partH

//---------------------------------------------------------------------------
constexpr inline u32 DU_SPHERE_PART_NUMVERTEX{82};
constexpr inline u32 DU_SPHERE_PART_NUMFACES{160};
constexpr inline u32 DU_SPHERE_PART_NUMLINES{176};

extern const std::array<Fvector, DU_SPHERE_PART_NUMVERTEX> XR_ALIGNED_DEFAULT du_sphere_part_vertices;
extern const std::array<u16, DU_SPHERE_PART_NUMFACES * 3> XR_ALIGNED_DEFAULT du_sphere_part_faces;
extern const std::array<u16, DU_SPHERE_PART_NUMLINES * 2> XR_ALIGNED_DEFAULT du_sphere_part_lines;

#endif
