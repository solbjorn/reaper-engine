#pragma once

class CPhysicsShell;
class CPhysicsShellHolder;

class physics_shell_animated : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(physics_shell_animated);

protected:
    CPhysicsShell* physics_shell;
    bool update_velocity;

public:
    explicit physics_shell_animated(CPhysicsShellHolder* ca, bool _update_velocity);
    ~physics_shell_animated() override;

    [[nodiscard]] CPhysicsShell* shell() const { return physics_shell; }
    [[nodiscard]] CPhysicsShell* shell() { return physics_shell; }

public:
    bool update(const Fmatrix& xrorm);

protected:
    virtual void create_shell(CPhysicsShellHolder* O);
};
