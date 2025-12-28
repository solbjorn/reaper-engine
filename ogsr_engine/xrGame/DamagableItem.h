#ifndef __DAMAGABLE_ITEM_H
#define __DAMAGABLE_ITEM_H

class CDamagableItem : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CDamagableItem);

protected:
    float m_max_health{};
    u16 m_levels_num{std::numeric_limits<u16>::max()};
    u16 m_level_applied{std::numeric_limits<u16>::max()};

public:
    CDamagableItem() = default;
    ~CDamagableItem() override = default;

    virtual void Init(float max_health, u16 level_num);
    void HitEffect();
    void RestoreEffect();
    float DamageLevelToHealth(u16 dl);

protected:
    u16 DamageLevel();
    virtual float Health() = 0;
    virtual void ApplyDamage(u16 level);
};

class CDamagableHealthItem : public CDamagableItem
{
    RTTI_DECLARE_TYPEINFO(CDamagableHealthItem, CDamagableItem);

private:
    using inherited = CDamagableItem;

    f32 m_health{};

public:
    ~CDamagableHealthItem() override = default;

    virtual void Init(float max_health, u16 level_num);
    void Hit(float P);
    void SetHealth(float health) { m_health = health; }

protected:
    virtual float Health() { return m_health; }
};

#endif // __DAMAGABLE_ITEM_H
