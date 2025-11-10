#ifndef du_coneH
#define du_coneH

//---------------------------------------------------------------------------
constexpr inline u32 DU_CONE_NUMVERTEX{18};
constexpr inline u32 DU_CONE_NUMFACES{32};
constexpr inline u32 DU_CONE_NUMLINES{24};

extern const std::array<Fvector, DU_CONE_NUMVERTEX> XR_ALIGNED_DEFAULT du_cone_vertices;
extern const std::array<u16, DU_CONE_NUMFACES * 3> XR_ALIGNED_DEFAULT du_cone_faces;
extern const std::array<u16, DU_CONE_NUMLINES * 2> XR_ALIGNED_DEFAULT du_cone_lines;

#endif
