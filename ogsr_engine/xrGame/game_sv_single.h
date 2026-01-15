#pragma once

#include "game_sv_base.h"

class xrServer;
class CALifeSimulator;

class game_sv_Single : public game_sv_GameState
{
    RTTI_DECLARE_TYPEINFO(game_sv_Single, game_sv_GameState);

private:
    typedef game_sv_GameState inherited;

protected:
    CALifeSimulator* m_alife_simulator{};

public:
    game_sv_Single();
    ~game_sv_Single() override;

    virtual LPCSTR type_name() const { return "single"; }
    tmc::task<void> Create(shared_str& options) override;

    virtual void OnCreate(u16 id_who);
    [[nodiscard]] BOOL OnTouch(u16 eid_who, u16 eid_what, BOOL = FALSE) override;
    virtual void OnDetach(u16 eid_who, u16 eid_what);

    // Main
    virtual void Update();
    virtual ALife::_TIME_ID GetGameTime();
    virtual float GetGameTimeFactor();
    virtual void SetGameTimeFactor(const float fTimeFactor);

    virtual ALife::_TIME_ID GetEnvironmentGameTime();
    virtual float GetEnvironmentGameTimeFactor();
    virtual void SetEnvironmentGameTimeFactor(const float fTimeFactor);

    [[nodiscard]] bool change_level(NET_Packet& net_packet, ClientID sender) override;
    void save_game(NET_Packet& net_packet, ClientID) override;
    [[nodiscard]] bool load_game(NET_Packet& net_packet, ClientID sender) override;
    void switch_distance(NET_Packet& net_packet, ClientID) override;
    void teleport_object(NET_Packet& packet, u16 id) override;

    virtual void add_restriction(NET_Packet& packet, u16 id);
    virtual void remove_restriction(NET_Packet& packet, u16 id);
    virtual void remove_all_restrictions(NET_Packet& packet, u16 id);

    virtual bool custom_sls_default() { return !!m_alife_simulator; }
    virtual void sls_default();

    virtual shared_str level_name(const shared_str& server_options) const;
    virtual void on_death(CSE_Abstract* e_dest, CSE_Abstract* e_src);
    tmc::task<void> restart_simulator(gsl::czstring saved_game_name);

    IC xrServer& server() const
    {
        VERIFY(m_server);
        return (*m_server);
    }

    IC CALifeSimulator& alife() const
    {
        VERIFY(m_alife_simulator);
        return (*m_alife_simulator);
    }
};
