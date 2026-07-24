#include "stdafx.h"

#include "SH_Atomic.h"

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
    signature = XR_ASSERT_VAL(pBlob != nullptr);
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

    for (auto& layout : vs_to_layout)
        //	Release vertex layout
        _RELEASE(layout.second);
}
