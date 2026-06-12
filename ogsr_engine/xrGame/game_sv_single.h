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

    [[nodiscard]] gsl::czstring type_name() const override { return "single"; }
    tmc::task<void> Create(shared_str& options) override;

    void OnCreate(u16 id_who) override;
    [[nodiscard]] BOOL OnTouch(u16 eid_who, u16 eid_what, BOOL = FALSE) override;
    void OnDetach(u16 eid_who, u16 eid_what) override;

    // Main
    void Update() override;
    [[nodiscard]] ALife::_TIME_ID GetGameTime() override;
    [[nodiscard]] f32 GetGameTimeFactor() override;
    void SetGameTimeFactor(const f32 fTimeFactor) override;

    [[nodiscard]] ALife::_TIME_ID GetEnvironmentGameTime() override;
    [[nodiscard]] f32 GetEnvironmentGameTimeFactor() override;
    void SetEnvironmentGameTimeFactor(const f32 fTimeFactor) override;

    [[nodiscard]] bool change_level(NET_Packet& net_packet, ClientID sender) override;
    void save_game(NET_Packet& net_packet, ClientID) override;
    [[nodiscard]] bool load_game(NET_Packet& net_packet, ClientID sender) override;
    void switch_distance(NET_Packet& net_packet, ClientID) override;
    void teleport_object(NET_Packet& packet, u16 id) override;

    void add_restriction(NET_Packet& packet, u16 id) override;
    void remove_restriction(NET_Packet& packet, u16 id) override;
    void remove_all_restrictions(NET_Packet& packet, u16 id) override;

    [[nodiscard]] bool custom_sls_default() override { return !!m_alife_simulator; }
    void sls_default() override;

    [[nodiscard]] shared_str level_name(const shared_str& server_options) const override;
    void on_death(CSE_Abstract* e_dest, CSE_Abstract* e_src) override;
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
