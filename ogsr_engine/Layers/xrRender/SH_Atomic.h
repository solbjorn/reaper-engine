#ifndef sh_atomicH
#define sh_atomicH

#include "../../xrCore/xr_resource.h"
#include "tss_def.h"

#include "../xrRenderDX10/StateManager/dx10State.h"

//////////////////////////////////////////////////////////////////////////
// Atomic resources
//////////////////////////////////////////////////////////////////////////

struct SInputSignature : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(SInputSignature, xr_resource_flagged);

public:
    ID3DBlob* signature;

    explicit SInputSignature(ID3DBlob* pBlob);
    ~SInputSignature() override;
};
typedef resptr_core<SInputSignature, resptr_base<SInputSignature>> ref_input_sign;

//////////////////////////////////////////////////////////////////////////

struct SVS : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(SVS, xr_resource_named);

public:
    ID3DVertexShader* vs{};
    R_constant_table constants;
    ref_input_sign signature;

    ~SVS() override;
};
typedef resptr_core<SVS, resptr_base<SVS>> ref_vs;

//////////////////////////////////////////////////////////////////////////

struct SPS : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(SPS, xr_resource_named);

public:
    ID3DPixelShader* ps;
    R_constant_table constants;

    ~SPS() override;
};
typedef resptr_core<SPS, resptr_base<SPS>> ref_ps;

//////////////////////////////////////////////////////////////////////////

struct SGS : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(SGS, xr_resource_named);

public:
    ID3DGeometryShader* gs;
    R_constant_table constants;

    ~SGS() override;
};
typedef resptr_core<SGS, resptr_base<SGS>> ref_gs;

struct SHS : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(SHS, xr_resource_named);

public:
    ID3D11HullShader* sh;
    R_constant_table constants;

    ~SHS() override;
};
typedef resptr_core<SHS, resptr_base<SHS>> ref_hs;

struct SDS : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(SDS, xr_resource_named);

public:
    ID3D11DomainShader* sh;
    R_constant_table constants;

    ~SDS() override;
};
typedef resptr_core<SDS, resptr_base<SDS>> ref_ds;

struct SCS : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(SCS, xr_resource_named);

public:
    ID3D11ComputeShader* sh;
    R_constant_table constants;

    ~SCS() override;
};
typedef resptr_core<SCS, resptr_base<SCS>> ref_cs;

//////////////////////////////////////////////////////////////////////////

struct SState : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(SState, xr_resource_flagged);

public:
    ID3DState* state;
    SimulatorStates state_code;

    SState() = default;
    ~SState() override;
};
typedef resptr_core<SState, resptr_base<SState>> ref_state;

//////////////////////////////////////////////////////////////////////////

struct SDeclaration : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(SDeclaration, xr_resource_flagged);

public:
    //	Maps input signature to input layout
    xr_map<ID3DBlob*, ID3DInputLayout*> vs_to_layout;
    xr_vector<D3D_INPUT_ELEMENT_DESC> dx10_dcl_code;

    //	Use this for DirectX10 to cache DX9 declaration for comparison purpose only
    xr_vector<D3DVERTEXELEMENT9> dcl_code;

    SDeclaration() = default;
    ~SDeclaration() override;
};
typedef resptr_core<SDeclaration, resptr_base<SDeclaration>> ref_declaration;

#endif // sh_atomicH
