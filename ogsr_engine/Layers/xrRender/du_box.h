#ifndef du_boxH
#define du_boxH

//---------------------------------------------------------------------------
constexpr inline u32 DU_BOX_NUMVERTEX{8};
constexpr inline u32 DU_BOX_NUMFACES{12};
constexpr inline u32 DU_BOX_NUMLINES{12};
constexpr inline u32 DU_BOX_NUMVERTEX2{36};

extern const std::array<Fvector, DU_BOX_NUMVERTEX> XR_ALIGNED_DEFAULT du_box_vertices;
extern const std::array<u16, DU_BOX_NUMFACES * 3> XR_ALIGNED_DEFAULT du_box_faces;
extern const std::array<u16, DU_BOX_NUMLINES * 2> XR_ALIGNED_DEFAULT du_box_lines;
extern const std::array<Fvector, DU_BOX_NUMVERTEX2> XR_ALIGNED_DEFAULT du_box_vertices2;

#endif
