#ifndef PHSHELL_CREATOR_H
#define PHSHELL_CREATOR_H

#include "ph_shell_interface.h"

class CPHShellSimpleCreator : public IPhysicShellCreator
{
    RTTI_DECLARE_TYPEINFO(CPHShellSimpleCreator, IPhysicShellCreator);

public:
    virtual void CreatePhysicsShell();
};
#endif
