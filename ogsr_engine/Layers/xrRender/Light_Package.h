#ifndef __XRRENDER_LIGHT_PACKAGE_H
#define __XRRENDER_LIGHT_PACKAGE_H

#include "light.h"

class light_Package
{
public:
    xr_vector<light*> v_point;
    xr_vector<light*> v_spot;
    xr_vector<light*> v_shadowed;

public:
    void clear();
    void sort();
    void vis_prepare();
    tmc::task<void> vis_update();
};

#endif /* __XRRENDER_LIGHT_PACKAGE_H */
