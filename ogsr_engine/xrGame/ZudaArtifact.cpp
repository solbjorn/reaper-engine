///////////////////////////////////////////////////////////////
// ZudaArtifact.cpp
// ZudaArtefact - артефакт "зуда"
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ZudaArtifact.h"
#include "PhysicsShell.h"

CZudaArtefact::CZudaArtefact() = default;
CZudaArtefact::~CZudaArtefact() = default;

void CZudaArtefact::Load(LPCSTR section) { inherited::Load(section); }
