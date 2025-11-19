// UICharacterInfo.h:  окошко, с информацией о персонаже
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "uiwindow.h"
#include "../alife_space.h"
#include "../character_info_defs.h"

class CSE_ALifeTraderAbstract;
class CUIStatic;
class CCharacterInfo;
class CUIXml;
class CUIScrollView;

class CUICharacterInfo : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUICharacterInfo, CUIWindow);

private:
    typedef CUIWindow inherited;

protected:
    void SetRelation(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill);
    void ResetAllStrings();
    void UpdateRelation();
    bool hasOwner() { return (m_ownerID != u16(-1)); }

    // Biography
    CUIScrollView* pUIBio{};
    bool m_bForceUpdate{};
    u16 m_ownerID{std::numeric_limits<u16>::max()};

    enum
    {
        eUIIcon = 0,
        eUIName,
        eUIRank,
        eUIRankCaption,
        eUICommunity,
        eUICommunityCaption,
        eUIReputation,
        eUIReputationCaption,
        eUIRelation,
        eUIRelationCaption,
        eMaxCaption,
    };
    CUIStatic* m_icons[eMaxCaption]{};
    shared_str m_texture_name;

public:
    CUICharacterInfo();
    ~CUICharacterInfo() override;

    void Init(float x, float y, float width, float height, CUIXml* xml_doc);
    void Init(float x, float y, float width, float height, const char* xml_name);
    void InitCharacter(u16 id);
    void ClearInfo();

    virtual void Update();

    u16 OwnerID() const { return m_ownerID; }
    CUIStatic& UIIcon()
    {
        VERIFY(m_icons[eUIIcon]);
        return *m_icons[eUIIcon];
    }
    CUIStatic& UIName()
    {
        VERIFY(m_icons[eUIName]);
        return *m_icons[eUIName];
    }
    const shared_str& IconName() { return m_texture_name; }
};

CSE_ALifeTraderAbstract* ch_info_get_from_id(u16 id);
