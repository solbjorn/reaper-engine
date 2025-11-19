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
    virtual LPCSTR getComment() { return "LEVEL: trees/bushes"; }
    virtual BOOL canBeDetailed() { return TRUE; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_Tree();
    ~CBlender_Tree() override;
};

#endif // AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
