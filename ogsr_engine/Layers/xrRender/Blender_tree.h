// Blender_Tree.h: interface for the CBlender_Tree class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#define AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3

class CBlender_Tree : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_Tree, IBlenderXr);

private:
    xrP_BOOL oBlend;
    xrP_BOOL oNotAnTree;

public:
    [[nodiscard]] gsl::czstring getComment() override { return "LEVEL: trees/bushes"; }
    [[nodiscard]] BOOL canBeDetailed() override { return TRUE; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    CBlender_Tree();
    ~CBlender_Tree() override;
};

#endif // AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
