// Blender_Vertex_aref.h: interface for the CBlender_Vertex_aref class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_2
#define AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_2

class CBlender_Detail_Still : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_Detail_Still, IBlenderXr);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "LEVEL: detail objects"; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    CBlender_Detail_Still();
    ~CBlender_Detail_Still() override;
};

#endif // AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_2
