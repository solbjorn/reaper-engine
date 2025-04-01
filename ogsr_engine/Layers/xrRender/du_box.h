#ifndef du_boxH
#define du_boxH

//---------------------------------------------------------------------------
constexpr u32 DU_BOX_NUMVERTEX = 8;
constexpr u32 DU_BOX_NUMFACES = 12;
constexpr u32 DU_BOX_NUMLINES = 12;
constexpr u32 DU_BOX_NUMVERTEX2 = 36;

extern const Fvector du_box_vertices[];
extern const WORD du_box_faces[];
extern const WORD du_box_lines[];
extern const Fvector du_box_vertices2[];

#endif
