///////////////////////////////////////////////////////////////
// AI_PhraseDialogManager.cpp
// Класс, от которого наследуются NPC персонажи, ведущие диалог
// с актером
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "AI_PhraseDialogManager.h"

#include "PhraseDialog.h"
#include "InventoryOwner.h"
#include "character_info.h"
#include "GameObject.h"
#include "relation_registry.h"

#include "UIGameSP.h"
#include "HUDManager.h"
#include "Level.h"
#include "ui/UITalkWnd.h"

CAI_PhraseDialogManager::CAI_PhraseDialogManager() = default;
CAI_PhraseDialogManager::~CAI_PhraseDialogManager() = default;

// PhraseDialogManager
void CAI_PhraseDialogManager::ReceivePhrase(DIALOG_SHARED_PTR& phrase_dialog)
{
    AnswerPhrase(phrase_dialog);
    CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}

void CAI_PhraseDialogManager::AnswerPhrase(DIALOG_SHARED_PTR& phrase_dialog)
{
    auto pInvOwner = XR_ASSERT_VAL(smart_cast<CInventoryOwner*>(this) != nullptr);
    auto pOthersGO = XR_ASSERT_VAL(smart_cast<CGameObject*>(phrase_dialog->OurPartner(this)) != nullptr);
    auto pOthersIO = XR_ASSERT_VAL(smart_cast<CInventoryOwner*>(pOthersGO) != nullptr);

    if (!phrase_dialog->IsFinished())
    {
        CHARACTER_GOODWILL attitude = RELATION_REGISTRY().GetAttitude(pOthersIO, pInvOwner);

        xr_vector<int> phrases;
        CHARACTER_GOODWILL phrase_goodwill = NO_GOODWILL;
        // если не найдем более подходяещей выводим фразу
        // последнюю из списка (самую грубую)
        int phrase_num = phrase_dialog->PhraseList().size() - 1;
        for (u32 i = 0; i < phrase_dialog->PhraseList().size(); ++i)
        {
            phrase_goodwill = phrase_dialog->PhraseList()[i]->GoodwillLevel();
            if (attitude >= phrase_goodwill)
            {
                phrase_num = i;
                break;
            }
        }

        for (u32 i = 0; i < phrase_dialog->PhraseList().size(); i++)
        {
            if (phrase_goodwill == phrase_dialog->PhraseList()[i]->GoodwillLevel())
                phrases.push_back(i);
        }

        phrase_num = phrases[Random.randI(0, phrases.size())];
        shared_str phrase_id = phrase_dialog->PhraseList()[phrase_num]->GetID();

        CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
        pGameSP->TalkMenu->AddAnswer(shared_str{phrase_dialog->GetPhraseText(phrase_id)}, pInvOwner->Name());

        CPhraseDialogManager::SayPhrase(phrase_dialog, phrase_id);
    }
}

void CAI_PhraseDialogManager::SetStartDialog(shared_str phrase_dialog) { m_sStartDialog = phrase_dialog; }
void CAI_PhraseDialogManager::SetDefaultStartDialog(shared_str phrase_dialog) { m_sDefaultStartDialog = phrase_dialog; }
void CAI_PhraseDialogManager::RestoreDefaultStartDialog() { m_sStartDialog = m_sDefaultStartDialog; }

void CAI_PhraseDialogManager::UpdateAvailableDialogs(CPhraseDialogManager* partner)
{
    m_AvailableDialogs.clear();
    m_CheckedDialogs.clear();

    if (m_sStartDialog.c_str() != nullptr)
        std::ignore = inherited::AddAvailableDialog(m_sStartDialog, partner);

    std::ignore = inherited::AddAvailableDialog(shared_str{"hello_dialog"}, partner);
    inherited::UpdateAvailableDialogs(partner);
}
