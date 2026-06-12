#ifndef __DAMAGABLE_ITEM_H
#define __DAMAGABLE_ITEM_H

class XR_NOVTABLE CDamagableItem : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CDamagableItem);

protected:
    float m_max_health{};
    u16 m_levels_num{std::numeric_limits<u16>::max()};
    u16 m_level_applied{std::numeric_limits<u16>::max()};

public:
    CDamagableItem() = default;
    ~CDamagableItem() override = 0;

    virtual void Init(f32 max_health, u16 level_num);
    void HitEffect();
    void RestoreEffect();
    float DamageLevelToHealth(u16 dl);

protected:
    u16 DamageLevel();
    [[nodiscard]] virtual f32 Health() = 0;
    virtual void ApplyDamage(u16 level);
};

inline CDamagableItem::~CDamagableItem() = default;

class CDamagableHealthItem : public CDamagableItem
{
    RTTI_DECLARE_TYPEINFO(CDamagableHealthItem, CDamagableItem);

private:
    using inherited = CDamagableItem;

    f32 m_health{};

public:
    ~CDamagableHealthItem() override = default;

    void Init(f32 max_health, u16 level_num) override;
    void Hit(float P);
    void SetHealth(float health) { m_health = health; }

protected:
    [[nodiscard]] f32 Health() override { return m_health; }
};

#endif // __DAMAGABLE_ITEM_H
