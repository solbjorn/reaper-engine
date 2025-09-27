#include "stdafx.h"

#include "sh_atomic.h"
#include "ResourceManager.h"

///////////////////////////////////////////////////////////////////////
//	SVS
SVS::~SVS()
{
    RImplementation.Resources->_DeleteVS(this);
    _RELEASE(vs);
}

///////////////////////////////////////////////////////////////////////
//	SPS
SPS::~SPS()
{
    _RELEASE(ps);
    RImplementation.Resources->_DeletePS(this);
}

///////////////////////////////////////////////////////////////////////
//	SGS
SGS::~SGS()
{
    _RELEASE(gs);
    RImplementation.Resources->_DeleteGS(this);
}

SHS::~SHS()
{
    _RELEASE(sh);
    RImplementation.Resources->_DeleteHS(this);
}

SDS::~SDS()
{
    _RELEASE(sh);
    RImplementation.Resources->_DeleteDS(this);
}

SCS::~SCS()
{
    _RELEASE(sh);
    RImplementation.Resources->_DeleteCS(this);
}

///////////////////////////////////////////////////////////////////////
//	SInputSignature
SInputSignature::SInputSignature(ID3DBlob* pBlob)
{
    VERIFY(pBlob);
    signature = pBlob;
    signature->AddRef();
}

SInputSignature::~SInputSignature()
{
    _RELEASE(signature);
    RImplementation.Resources->_DeleteInputSignature(this);
}

///////////////////////////////////////////////////////////////////////
//	SState
SState::~SState()
{
    _RELEASE(state);
    RImplementation.Resources->_DeleteState(this);
}

///////////////////////////////////////////////////////////////////////
//	SDeclaration
SDeclaration::~SDeclaration()
{
    RImplementation.Resources->_DeleteDecl(this);
    xr_map<ID3DBlob*, ID3DInputLayout*>::iterator iLayout;
    iLayout = vs_to_layout.begin();
    for (; iLayout != vs_to_layout.end(); ++iLayout)
    {
        //	Release vertex layout
        _RELEASE(iLayout->second);
    }
}
