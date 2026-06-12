#pragma once

#include "..\xr_3da\feel_touch.h"
#include "..\xr_3da\feel_sound.h"
#include "..\xr_3da\iinputreceiver.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "actor_flags.h"
#include "actor_defs.h"
#include "entity_alive.h"
#include "PHMovementControl.h"
#include "PhysicsShell.h"
#include "InventoryOwner.h"
#include "..\xr_3da\StatGraph.h"
#include "PhraseDialogManager.h"

#include "step_manager.h"
#include "xr_level_controller.h"

class CInfoPortion;
struct GAME_NEWS_DATA;
class CActorCondition;
class CCustomOutfit;
class CKnownContactsRegistryWrapper;
class CEncyclopediaRegistryWrapper;
class CGameTaskRegistryWrapper;
class CGameNewsRegistryWrapper;
class CCharacterPhysicsSupport;
// refs
class CCameraBase;
class CBoneInstance;
class CBlend;
class CWeaponList;
class CEffectorBobbing;
class CHolderCustom;
class CUsableScriptObject;

struct SShootingEffector;
struct SSleepEffector;
class CSleepEffectorPP;
class IInventoryBox;
// class  CActorEffector;

class CHudItem;
class CArtefact;

struct SActorMotions;
struct SActorVehicleAnims;
class CActorCondition;
class SndShockEffector;
class CActorFollowerMngr;
class CGameTaskManager;

class CCameraShotEffector;
class CActorInputHandler;

class CActorMemory;
class CActorStatisticMgr;

class CLocationManager;

class CActorCameraManager;

struct ActorRestoreParams
{
    float HealthRestoreSpeed;
    float PowerRestoreSpeed;
    float BleedingRestoreSpeed;
    float SatietyRestoreSpeed;
    float RadiationRestoreSpeed;
    float PsyHealthRestoreSpeed;
    float ThirstRestoreSpeed;
};

class CActor : public CEntityAlive,
               public IInputReceiver,
               public Feel::Touch,
               public CInventoryOwner,
               public CPhraseDialogManager,
               public CStepManager,
               public Feel::Sound
#ifdef DEBUG
    ,
               public pureRender
#endif
{
    RTTI_DECLARE_TYPEINFO(CActor, CEntityAlive, IInputReceiver, Feel::Touch, CInventoryOwner, CPhraseDialogManager, CStepManager, Feel::Sound
#ifdef DEBUG
                          ,
                          pureRender
#endif
    );

public:
    friend class CActorCondition;
    friend class CScriptActor;

private:
    typedef CEntityAlive inherited;

    //////////////////////////////////////////////////////////////////////////
    // General fucntions
    //////////////////////////////////////////////////////////////////////////
public:
    CActor();
    ~CActor() override;

    [[nodiscard]] BOOL AlwaysTheCrow() override { return TRUE; }

    [[nodiscard]] CAttachmentOwner* cast_attachment_owner() override { return this; }
    [[nodiscard]] CInventoryOwner* cast_inventory_owner() override { return this; }
    [[nodiscard]] CActor* cast_actor() override { return this; }
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }
    [[nodiscard]] IInputReceiver* cast_input_receiver() override { return this; }
    [[nodiscard]] CCharacterPhysicsSupport* character_physics_support() override { return m_pPhysics_support; }
    [[nodiscard]] CCharacterPhysicsSupport* character_physics_support() const override { return m_pPhysics_support; }
    [[nodiscard]] CPHDestroyable* ph_destroyable() override;
    CHolderCustom* Holder() { return m_holder; }
    u16 HolderID() { return m_holder_id; }

public:
    void Load(gsl::czstring section) override;

    tmc::task<void> shedule_Update(u32 T) override;
    tmc::task<void> UpdateCL() override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

    // Render
    void renderable_Render(u32 context_id, IRenderable* root) override;
    [[nodiscard]] BOOL renderable_ShadowGenerate() override;
    void feel_sound_new(CObject* who, int, CSound_UserDataPtr, const Fvector&, float power, float) override;
    [[nodiscard]] Feel::Sound* dcast_FeelSound() override { return this; }
    float m_snd_noise;

#ifdef DEBUG
    tmc::task<void> OnRender() override;
#endif

    // Имеется ли воздействие пси-ауры на ГГ (тряска рук)
    bool PsyAuraAffect{};

    /////////////////////////////////////////////////////////////////
    // Inventory Owner

public:
    // information receive & dialogs
    [[nodiscard]] bool OnReceiveInfo(shared_str info_id) const override;
    void OnDisableInfo(shared_str info_id) const override;

    void NewPdaContact(CInventoryOwner*) override;
    void LostPdaContact(CInventoryOwner*) override;

protected:
    virtual void AddEncyclopediaArticle(const CInfoPortion*, bool = false) const;
    virtual void AddGameTask(const CInfoPortion* info_portion) const;

protected:
    struct SDefNewsMsg
    {
        GAME_NEWS_DATA* news_data;
        u32 time;
        bool operator<(const SDefNewsMsg& other) const { return time > other.time; }
    };
    xr_vector<SDefNewsMsg> m_defferedMessages;
    void UpdateDefferedMessages();
    u32 m_news_to_show;

public:
    void AddGameNews_deffered(GAME_NEWS_DATA& news_data, u32 delay);
    virtual void AddGameNews(GAME_NEWS_DATA& news_data);
    void PushNewsData(GAME_NEWS_DATA& news_data);
    u32 NewsToShow() { return m_news_to_show; }

protected:
    CGameTaskManager* m_game_task_manager{};
    CActorStatisticMgr* m_statistic_manager{};

public:
    void StartTalk(CInventoryOwner* talk_partner, bool = true) override;
    virtual void RunTalkDialog(CInventoryOwner* talk_partner);
    CGameTaskManager& GameTaskManager() const { return *m_game_task_manager; }
    CActorStatisticMgr& StatisticMgr() { return *m_statistic_manager; }
    CEncyclopediaRegistryWrapper* encyclopedia_registry;
    CGameNewsRegistryWrapper* game_news_registry;
    CCharacterPhysicsSupport* m_pPhysics_support;

    [[nodiscard]] gsl::czstring Name() const override { return CInventoryOwner::Name(); }

public:
    // PhraseDialogManager
    void ReceivePhrase(DIALOG_SHARED_PTR& phrase_dialog) override;
    void UpdateAvailableDialogs(CPhraseDialogManager* partner) override;
    virtual void TryToTalk();
    bool OnDialogSoundHandlerStart(CInventoryOwner* inv_owner, LPCSTR phrase);
    bool OnDialogSoundHandlerStop(CInventoryOwner* inv_owner);

    void reinit() override;
    void reload(gsl::czstring section) override;
    [[nodiscard]] bool use_bolts() const override;

    void OnItemTake(CInventoryItem* inventory_item) override;

    void OnItemRuck(CInventoryItem* inventory_item, EItemPlace previous_place) override;
    void OnItemBelt(CInventoryItem* inventory_item, EItemPlace previous_place) override;
    void OnItemSlot(CInventoryItem* inventory_item, EItemPlace previous_place) override;

    void OnItemDrop(CInventoryItem* inventory_item) override;
    void OnItemDropUpdate() override;

    tmc::task<void> Die(CObject* who) override;
    void Hit(SHit* pHDS) override;
    void PHHit(SHit& H) override;
    void HitSignal(f32 P, Fvector& vLocalDir, CObject* who, s16 element) override;
    void HitSector(CObject* who, CObject* weapon);

private:
    void HitMark(float P, Fvector dir, ALife::EHitType hit_type);

public:
    [[nodiscard]] f32 GetMass() override;
    [[nodiscard]] f32 GetCarryWeight() const override;
    [[nodiscard]] f32 Radius() const override;
    virtual void g_PerformDrop();

    [[nodiscard]] bool unlimited_ammo() override;

    // свойства артефактов
    virtual void UpdateArtefactsOnBelt();

    virtual ActorRestoreParams ActiveArtefactsOnBelt();
    virtual float HitArtefactsOnBelt(float, ALife::EHitType, bool = false);

    virtual void UpdateArtefactPanel();

protected:
    void ApplyArtefactEffects(ActorRestoreParams&, CArtefact*);
    // звук тяжелого дыхания
    ref_sound m_HeavyBreathSnd;
    ref_sound m_BloodSnd;

    /////////////////////////////////////////////////////////////////
    // misc properties
protected:
    // Death
    float hit_slowmo{};
    float hit_probability;
    bool m_hit_slowmo_jump;

    // media
    SndShockEffector* m_sndShockEffector;
    xr_vector<ref_sound> sndHit[ALife::eHitTypeMax];
    ref_sound sndDie[ACTOR_DEFS::SND_DIE_COUNT];

    float m_fLandingTime;
    float m_fJumpTime;
    float m_fFallTime;
    float m_fCamHeightFactor;

    // Dropping
    BOOL b_DropActivated;
    float f_DropPower;

    bool m_bOutBorder{};
    // сохраняет счетчик объектов в feel_touch, для которых необходимо обновлять размер колижена с актером
    u32 m_feel_touch_characters{};
    // разрешения на удаление трупа актера
    // после того как контролирующий его игрок зареспавнился заново.
    // устанавливается в game
    u8 m_loaded_ph_box_id{};

private:
    void SwitchOutBorder(bool new_border_state);

public:
    void detach_Vehicle();
    void steer_Vehicle(float angle);
    void attach_Vehicle(CHolderCustom* vehicle);

    [[nodiscard]] bool can_attach(const CInventoryItem* inventory_item) const override;

protected:
    CHolderCustom* m_holder{};
    u16 m_holderID{std::numeric_limits<u16>::max()};

    bool use_Holder(CHolderCustom* holder);

    bool use_Vehicle(CHolderCustom* object);
    bool use_MountedWeapon(CHolderCustom* object);
    void ActorUse();

    /////////////////////////////////////////////////////////
    // actor model & animations
    /////////////////////////////////////////////////////////
protected:
    BOOL m_bAnimTorsoPlayed;
    static void AnimTorsoPlayCallBack(CBlend* B);

    // Rotation
    SRotation r_torso;
    float r_torso_tgt_roll;
    // положение торса без воздействия эффекта отдачи оружия
    SRotation unaffected_r_torso;

    // ориентация модели
    float r_model_yaw_dest;
    float r_model_yaw; // orientation of model
    float r_model_yaw_delta; // effect on multiple "strafe"+"something"

public:
    SActorMotions* m_anims;
    SActorVehicleAnims* m_vehicle_anims;

    CBlend* m_current_legs_blend;
    CBlend* m_current_torso_blend;
    CBlend* m_current_jump_blend;
    MotionID m_current_legs;
    MotionID m_current_torso;
    MotionID m_current_head;

    // callback на анимации модели актера
    void SetCallbacks();
    void ResetCallbacks();
    static void Spin0Callback(CBoneInstance*);
    static void Spin1Callback(CBoneInstance*);
    static void ShoulderCallback(CBoneInstance*);
    static void HeadCallback(CBoneInstance*);
    static void VehicleHeadCallback(CBoneInstance*);

    [[nodiscard]] const SRotation Orientation() const override { return r_torso; }
    SRotation& Orientation() { return r_torso; }

private:
    void g_SetAnimation(u32 mstate_rl);
    void g_SetSprintAnimation(u32 mstate_rl, MotionID& legs);

    //////////////////////////////////////////////////////////////////////////
    // HUD
    //////////////////////////////////////////////////////////////////////////
public:
    void OnHUDDraw(ctx_id_t context_id, CCustomHUD*, IRenderable* root) override;
    BOOL HUDview() const;

    // visiblity
    [[nodiscard]] f32 ffGetFov() const override { return 90.0f; }
    [[nodiscard]] f32 ffGetRange() const override { return 500.0f; }

    //////////////////////////////////////////////////////////////////////////
    // Cameras and effectors
    //////////////////////////////////////////////////////////////////////////
public:
    CActorCameraManager& Cameras()
    {
        VERIFY(m_pActorEffector);
        return *m_pActorEffector;
    }

    IC CCameraBase* cam_Active() { return cameras[cam_active]; }
    [[nodiscard]] CCameraBase* cam_ByIndex(u16 index) { return (index < ACTOR_DEFS::eacMaxCam ? cameras[index] : nullptr); }
    [[nodiscard]] CCameraBase* cam_FirstEye() { return cameras[ACTOR_DEFS::eacFirstEye]; }
    [[nodiscard]] ACTOR_DEFS::EActorCameras active_cam() const { return cam_active; } // KD: need to know which cam active outside actor methods
    CEffectorBobbing* GetEffectorBobbing() { return pCamBobbing; }

protected:
    void cam_Set(ACTOR_DEFS::EActorCameras style);
    tmc::task<void> cam_Update(f32 dt, f32 fFOV);
    void camUpdateLadder(float dt);
    void cam_SetLadder();
    void cam_UnsetLadder();
    float currentFOV();

    // Cameras
    CCameraBase* cameras[ACTOR_DEFS::eacMaxCam];
    ACTOR_DEFS::EActorCameras cam_active;
    float fPrevCamPos;
    float current_ik_cam_shift;
    Fvector vPrevCamDir;
    float fCurAVelocity;
    CEffectorBobbing* pCamBobbing{};

    void LoadSleepEffector(LPCSTR section);
    SSleepEffector* m_pSleepEffector{};
    CSleepEffectorPP* m_pSleepEffectorPP{};

    // менеджер эффекторов, есть у каждого актрера
    CActorCameraManager* m_pActorEffector{};
    static float f_Ladder_cam_limit;
    ////////////////////////////////////////////
    // для взаимодействия с другими персонажами
    // или предметами
    ///////////////////////////////////////////
public:
    void feel_touch_new(CObject*) override;
    void feel_touch_delete(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_on_contact(CObject* O) override;

    CGameObject* ObjectWeLookingAt() { return m_pObjectWeLookingAt; }
    CInventoryOwner* PersonWeLookingAt() { return m_pPersonWeLookingAt; }
    LPCSTR GetDefaultActionForObject() { return m_sDefaultObjAction; }

protected:
    CUsableScriptObject* m_pUsableObject{};
    // Person we're looking at
    CInventoryOwner* m_pPersonWeLookingAt{};
    CHolderCustom* m_pVehicleWeLookingAt{};
    CGameObject* m_pObjectWeLookingAt{};
    IInventoryBox* m_pInvBoxWeLookingAt;

    // Tip for action for object we're looking at
    const char* m_sDefaultObjAction{};

    // режим подбирания предметов
    bool m_bPickupMode{};
    // расстояние подсветки предметов
    float m_fPickupInfoRadius;

    void PickupInfoDraw(CObject* object);

    void PickupModeUpdate();
    void PickupModeUpdate_COD();

public:
    void PickupModeOn();
    void PickupModeOff();

    //////////////////////////////////////////////////////////////////////////
    // Motions (передвижения актрера)
    //////////////////////////////////////////////////////////////////////////
public:
    void g_cl_CheckControls(u32 mstate_wf, Fvector& vControlAccel, float& Jump, float dt);
    void g_cl_ValidateMState(float dt, u32 mstate_wf);
    void g_cl_Orientate(u32 mstate_rl, float dt);
    void g_Orientate(u32 mstate_rl, float dt);
    bool g_LadderOrient();
    void UpdateMotionIcon(u32 mstate_rl);

    bool CanAccelerate();
    bool CanJump(float weight);
    bool CanMove();
    float CameraHeight();
    float CurrentHeight{};
    bool CanSprint();
    bool CanRun();
    void StopAnyMove();

    [[nodiscard]] bool AnyAction() const { return (mstate_real & ACTOR_DEFS::mcAnyAction) != 0; }
    [[nodiscard]] bool AnyMove() const { return (mstate_real & ACTOR_DEFS::mcAnyMove) != 0; }

    bool is_jump();
    bool is_crouch();
    u32 MovingState() const { return mstate_real; }

    IC float GetJumpSpeed() const { return m_fJumpSpeed; }
    IC float GetWalkAccel() const { return m_fWalkAccel; }
    IC float GetExoFactor() const { return m_fExoFactor; }
    IC void SetJumpSpeed(float _factor) { m_fJumpSpeed = _factor; }
    IC void SetWalkAccel(float _factor) { m_fWalkAccel = _factor; }
    IC void SetExoFactor(float _factor) { m_fExoFactor = _factor; }

protected:
    u32 mstate_wishful;
    u32 mstate_old;
    u32 mstate_real;

    BOOL m_bJumpKeyPressed;

    float m_fWalkAccel;
    float m_fJumpSpeed;
    float m_fRunFactor;
    float m_fRunBackFactor;
    float m_fWalkBackFactor;
    float m_fCrouchFactor;
    float m_fClimbFactor;
    float m_fSprintFactor;

    float m_fWalk_StrafeFactor;
    float m_fRun_StrafeFactor;

    float m_fExoFactor;
    float m_fLookoutAngle;

public:
    //////////////////////////////////////////////////////////////////////////
    // User input/output
    //////////////////////////////////////////////////////////////////////////
    void IR_OnMouseMove(s32 x, s32 y) override;
    tmc::task<void> IR_OnKeyboardPress(xr::key_id dik) override;
    void IR_OnKeyboardRelease(xr::key_id dik) override;
    tmc::task<void> IR_OnKeyboardHold(xr::key_id dik) override;
    tmc::task<void> IR_OnMouseWheel(gsl::index direction) override;
    virtual float GetLookFactor();

    IC void ResetStates()
    {
        mstate_old = 0;
        mstate_real = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    // Weapon fire control (оружие актрера)
    //////////////////////////////////////////////////////////////////////////
public:
    void g_WeaponBones(s32& L, s32& R1, s32& R2) override;
    void g_fireParams(CHudItem* pHudItem, Fvector& P, Fvector& D, const bool for_cursor = false) override;
    [[nodiscard]] BOOL g_State(SEntityState& state) const override;
    [[nodiscard]] f32 GetWeaponAccuracy() const override;
    bool IsZoomAimingMode() const { return m_bZoomAimingMode; }

protected:
    // если актер целится в прицел
    bool m_bZoomAimingMode{};

    // настройки аккуратности стрельбы
    // базовая дисперсия (когда игрок стоит на месте)
    float m_fDispBase;
    float m_fDispAim;
    // коэффициенты на сколько процентов увеличится базовая дисперсия
    // учитывает скорость актера
    float m_fDispVelFactor;
    // если актер бежит
    float m_fDispAccelFactor;
    // если актер сидит
    float m_fDispCrouchFactor;
    // crouch+no acceleration
    float m_fDispCrouchNoAccelFactor;
    // смещение firepoint относительно default firepoint для бросания болтов и гранат
    Fvector m_vMissileOffset;

public:
    // Получение, и запись смещения для гранат
    Fvector GetMissileOffset() const;
    void SetMissileOffset(const Fvector& vNewOffset);

protected:
    // косточки используемые при стрельбе
    int m_r_hand;
    int m_l_finger1;
    int m_r_finger2;
    int m_head;

    int m_l_clavicle;
    int m_r_clavicle;
    int m_spine2;
    int m_spine1;
    int m_spine;
    int m_neck;

    //////////////////////////////////////////////////////////////////////////
    // Network
    //////////////////////////////////////////////////////////////////////////
public:
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void net_Export(CSE_Abstract* E) override;
    tmc::task<void> net_Destroy() override;
    [[nodiscard]] BOOL net_Relevant() override; //	{ return getSVU() | getLocal(); };		// relevant for export to server
    void net_Relcase(CObject* O) override; //
    virtual void on_requested_spawn(CObject* object);
    // object serialization
    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;
    void net_Save(NET_Packet& P) override;
    [[nodiscard]] BOOL net_SaveRelevant() override;

protected:
    Fvector NET_SavedAccel;

    ////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] bool can_validate_position_on_spawn() override { return false; }

    //---------------------------------------------
    //	bool					m_bHasUpdate;
    /// spline coeff /////////////////////
    float SCoeff[3][4]; // коэффициэнты для сплайна Бизье
    float HCoeff[3][4]; // коэффициэнты для сплайна Эрмита
    Fvector IPosS, IPosH, IPosL; // положение актера после интерполяции Бизье, Эрмита, линейной

#ifdef DEBUG
    DEF_DEQUE(VIS_POSITION, Fvector);

    VIS_POSITION LastPosS;
    VIS_POSITION LastPosH;
    VIS_POSITION LastPosL;
#endif

    //////////////////////////////////////////////////////////////////////////
    // Actor physics
    //////////////////////////////////////////////////////////////////////////
public:
    void g_Physics(Fvector& accel, float jump, float dt);
    void SetPhPosition(const Fmatrix& pos);

    void ForceTransform(const Fmatrix& m) override;
    void ForceTransformAndDirection(const Fmatrix& m) override;

    //	virtual void			UpdatePosStack	( u32 Time0, u32 Time1 );
    virtual void MoveActor(Fvector NewPos, Fvector NewDir);

    void spawn_supplies() override;
    [[nodiscard]] bool human_being() const override { return true; }

    virtual shared_str GetDefaultVisualOutfit() const { return m_DefaultVisualOutfit; }
    virtual void SetDefaultVisualOutfit(shared_str DefaultOutfit) { m_DefaultVisualOutfit = DefaultOutfit; }
    virtual void UpdateAnimation() { g_SetAnimation(mstate_real); }

    virtual void ChangeVisual(shared_str NewVisual);
    void OnChangeVisual() override;

    virtual void RenderText(LPCSTR Text, Fvector dpos, float* pdup, u32 color);

    //////////////////////////////////////////////////////////////////////////
    // Controlled Routines
    //////////////////////////////////////////////////////////////////////////

    void set_input_external_handler(CActorInputHandler* handler);

    bool input_external_handler_installed() const { return !!m_input_external_handler; }
    IC void lock_accel_for(u32 time) { m_time_lock_accel = Device.dwTimeGlobal + time; }

private:
    CActorInputHandler* m_input_external_handler;
    u32 m_time_lock_accel;

    /////////////////////////////////////////
    // DEBUG INFO
protected:
    CStatGraph* pStatGraph{};

    shared_str m_DefaultVisualOutfit;

    u32 last_hit_frame;
#ifdef DEBUG
    friend class CLevelGraph;
#endif

public:
    void SetWeaponHideState(u32 State, bool bSet, bool now = false);
    [[nodiscard]] CCustomOutfit* GetOutfit() const override;

private:
    CActorCondition* m_entity_condition{};

protected:
    [[nodiscard]] CEntityConditionSimple* create_entity_condition(CEntityConditionSimple* ec) override;

public:
    IC CActorCondition& conditions() const;
    [[nodiscard]] DLL_Pure* _construct() override;
    [[nodiscard]] bool natural_weapon() const override { return false; }
    [[nodiscard]] bool natural_detector() const override { return false; }
    [[nodiscard]] bool use_center_to_aim() const override;

protected:
    u16 m_iLastHitterID{std::numeric_limits<u16>::max()};
    u16 m_iLastHittingWeaponID{std::numeric_limits<u16>::max()};
    s16 m_s16LastHittedElement;
    Fvector m_vLastHitDir;
    Fvector m_vLastHitPos;
    float m_fLastHealth;
    bool m_bWasHitted;
    bool m_bWasBackStabbed;

public:
    void SetHitInfo(CObject* who, CObject* weapon, s16 element, Fvector Pos, Fvector Dir) override;

    virtual bool InventoryAllowSprint();
    tmc::task<void> OnNextWeaponSlot();
    tmc::task<void> OnPrevWeaponSlot();

public:
    void on_weapon_shot_start(CWeapon* weapon) override;
    void on_weapon_shot_stop(CWeapon*) override;
    void on_weapon_hide(CWeapon*) override;
    [[nodiscard]] Fvector weapon_recoil_delta_angle();
    [[nodiscard]] Fvector weapon_recoil_last_delta();

protected:
    virtual void update_camera(CCameraShotEffector* effector);
    // step manager
    [[nodiscard]] bool is_on_ground() override;

private:
    CActorMemory* m_memory;

public:
    void SetActorVisibility(u16 who, float value);
    IC CActorMemory& memory() const
    {
        VERIFY(m_memory);
        return *m_memory;
    }

    void OnDifficultyChanged();

    IC float HitProbability() { return hit_probability; }
    [[nodiscard]] CVisualMemoryManager* visual_memory() const override;

    void On_B_NotCurrentEntity() override;

private:
    collide::rq_results RQR;
    BOOL CanPickItem(const CFrustum& frustum, const Fvector& from, CObject* item);
    xr_vector<ISpatial*> ISpatialResult;

private:
    CLocationManager* m_location_manager;

public:
    IC const CLocationManager& locations() const
    {
        VERIFY(m_location_manager);
        return (*m_location_manager);
    }

private:
    ALife::_OBJECT_ID m_holder_id;

public:
    [[nodiscard]] bool register_schedule() const override { return false; }
    IC u32 get_state() const { return this->mstate_real; }

    IC void set_state(u32 state) { mstate_real = state; }

    IC u32 get_state_wishful() const { return this->mstate_wishful; }

    IC void set_state_wishful(u32 state) { mstate_wishful = state; }

    bool is_actor_normal();
    bool is_actor_crouch();
    bool is_actor_creep();
    bool is_actor_climb();
    bool is_actor_walking();
    bool is_actor_running();
    bool is_actor_sprinting();
    bool is_actor_crouching();
    bool is_actor_creeping();
    bool is_actor_climbing();
    bool is_actor_moving();

    void RepackAmmo();

    bool IsDetectorActive() const;

private:
    // иммунитеты от препаратов, применяемые для ослабления хита
    float m_fDrugPsyProtectionCoeff;
    float m_fDrugRadProtectionCoeff;

public:
    IC void SetDrugRadProtection(float _prot) { m_fDrugRadProtectionCoeff = _prot; }
    IC void SetDrugPsyProtection(float _prot) { m_fDrugPsyProtectionCoeff = _prot; }
};
XR_SOL_BASE_CLASSES(CActor);

extern bool isActorAccelerated(u32 mstate, bool ZoomMode);

IC CActorCondition& CActor::conditions() const
{
    VERIFY(m_entity_condition);
    return (*m_entity_condition);
}

extern CActor* g_actor;
CActor* Actor();

constexpr inline float s_fFallTime{0.2f};

// Actor_Movement.cpp
extern float cam_LookoutSpeed;

// ActorCameras.cpp
extern float cam_HeightInterpolationSpeed;

// ActorInput.cpp
extern bool g_bAutoClearCrouch;
