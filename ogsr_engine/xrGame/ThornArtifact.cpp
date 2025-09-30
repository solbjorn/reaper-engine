///////////////////////////////////////////////////////////////
// ThornArtifact.cpp
// ThornArtefact - артефакт колючка
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ThornArtifact.h"
#include "PhysicsShell.h"

CThornArtefact::CThornArtefact() = default;
CThornArtefact::~CThornArtefact() = default;

void CThornArtefact::Load(LPCSTR section) { inherited::Load(section); }
