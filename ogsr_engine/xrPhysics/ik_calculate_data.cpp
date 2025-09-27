#include "StdAfx.h"

#include "ik/IKLimb.h"
#include "ik_calculate_data.h"

SCalculateData::SCalculateData(CIKLimb& l, const Fmatrix& o) : m_limb{&l}, m_obj{&o} {}
