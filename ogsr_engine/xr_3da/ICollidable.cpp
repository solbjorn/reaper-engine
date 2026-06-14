#include "stdafx.h"

#include "ICollidable.h"

#include "xr_collide_form.h"

ICollidable::~ICollidable() { xr_delete(collidable.model); }
