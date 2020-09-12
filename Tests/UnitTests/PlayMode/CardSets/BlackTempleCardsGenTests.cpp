﻿// Copyright (c) 2019 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Utils/CardSetUtils.hpp>
#include <Utils/TestUtils.hpp>

#include <Rosetta/PlayMode/Actions/Draw.hpp>
#include <Rosetta/PlayMode/Cards/Cards.hpp>
#include <Rosetta/PlayMode/Zones/DeckZone.hpp>
#include <Rosetta/PlayMode/Zones/FieldZone.hpp>
#include <Rosetta/PlayMode/Zones/HandZone.hpp>
#include <Rosetta/PlayMode/Zones/SecretZone.hpp>

using namespace RosettaStone;
using namespace PlayMode;
using namespace PlayerTasks;
using namespace SimpleTasks;

// ------------------------------------------ SPELL - DRUID
// [BT_130] Overgrowth - COST:4
// - Faction: Neutral, Set: Core, Rarity: Common
// --------------------------------------------------------
// Text: Gain two empty Mana Crystals.
// --------------------------------------------------------
TEST_CASE("[Druid : Spell] - BT_130 : Overgrowth")
{
    GameConfig config;
    config.player1Class = CardClass::DRUID;
    config.player2Class = CardClass::WARLOCK;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(8);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Overgrowth"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Overgrowth"));

    game.Process(curPlayer, PlayCardTask::Spell(card1));
    CHECK_EQ(curPlayer->GetRemainingMana(), 4);
    CHECK_EQ(curPlayer->GetTotalMana(), 10);

    game.Process(curPlayer, PlayCardTask::Spell(card2));
    CHECK_EQ(curPlayer->GetRemainingMana(), 0);
    CHECK_EQ(curPlayer->GetTotalMana(), 10);
}

// ----------------------------------------- MINION - DRUID
// [BT_131] Ysiel Windsinger - COST:9 [ATK:5/HP:5]
// - Set: BLACK_TEMPLE, Rarity: Legendary
// --------------------------------------------------------
// Text: Your spells cost (1).
// --------------------------------------------------------
// GameTag:
//  - ELITE = 1
//  - AURA = 1
// --------------------------------------------------------
TEST_CASE("[Druid : Minion] - BT_131 : Ysiel Windsinger")
{
    GameConfig config;
    config.player1Class = CardClass::DRUID;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();

    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Ysiel Windsinger"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Moonfire"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Silence"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(card2->GetCost(), 1);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card3, card1));
    CHECK_EQ(card2->GetCost(), 0);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer,
                 PlayCardTask::SpellTarget(card2, opPlayer->GetHero()));
    CHECK_EQ(curPlayer->GetRemainingMana(), 10);
}

// ----------------------------------------- MINION - DRUID
// [BT_136] Archspore Msshi'fn - COST:3 [ATK:3/HP:4]
//  - Set: BLACK_TEMPLE, Rarity: Legendary
// --------------------------------------------------------
// Text: <b>Taunt</b> <b>Deathrattle:</b> Shuffle
//       'Msshi'fn Prime' into your deck.
// --------------------------------------------------------
// GameTag:
//  - ELITE = 1
//  - DEATHRATTLE = 1
//  - TAUNT = 1
// --------------------------------------------------------
TEST_CASE("[Druid : Minion] - BT_136 : Archspore Msshi'fn")
{
    GameConfig config;
    config.player1Class = CardClass::DRUID;
    config.player2Class = CardClass::WARLOCK;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = false;
    config.autoRun = false;
    config.doShuffle = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(40);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(20);
    opPlayer->SetUsedMana(0);

    auto& curDeck = *(curPlayer->GetDeckZone());
    auto& curField = *(curPlayer->GetFieldZone());
    auto& curHand = *(curPlayer->GetHandZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Archspore Msshi'fn"));
    const auto card2 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Archspore Msshi'fn"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));
    const auto card4 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    game.Process(curPlayer, PlayCardTask::Minion(card2));
    CHECK_EQ(curField[0]->HasTaunt(), true);
    CHECK_EQ(curField[1]->HasTaunt(), true);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card3, card1));
    game.Process(opPlayer, PlayCardTask::SpellTarget(card4, card2));
    CHECK_EQ(curField.GetCount(), 0);
    CHECK_EQ(curDeck.GetCount(), 2);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    const auto card5 = curHand[0];

    game.Process(curPlayer, PlayCardTask::Minion(card5, 1));
    CHECK_EQ(curField[0]->card->name, "Msshi'fn Prime");
    CHECK_EQ(curField[0]->GetAttack(), 9);
    CHECK_EQ(curField[0]->GetHealth(), 9);
    CHECK_EQ(curField[1]->card->name, "Fungal Guardian");
    CHECK_EQ(curField[1]->HasTaunt(), true);
    CHECK_EQ(curField[1]->GetAttack(), 9);
    CHECK_EQ(curField[1]->GetHealth(), 9);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    const auto card6 = curHand[0];

    game.Process(curPlayer, PlayCardTask::Minion(card6, 2));
    CHECK_EQ(curField[2]->card->name, "Msshi'fn Prime");
    CHECK_EQ(curField[2]->GetAttack(), 9);
    CHECK_EQ(curField[2]->GetHealth(), 9);
    CHECK_EQ(curField[3]->card->name, "Fungal Bruiser");
    CHECK_EQ(curField[3]->HasRush(), true);
    CHECK_EQ(curField[3]->GetAttack(), 9);
    CHECK_EQ(curField[3]->GetHealth(), 9);
}

// ------------------------------------------- SPELL - MAGE
// [BT_002] Incanter's Flow - COST:2
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Reduce the Cost of spells in your deck by (1).
// --------------------------------------------------------
TEST_CASE("[Mage : Spell] - BT_002 : Incanter's Flow")
{
    GameConfig config;
    config.player1Class = CardClass::MAGE;
    config.player2Class = CardClass::ROGUE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = false;
    config.autoRun = false;

    for (int i = 0; i < 30; ++i)
    {
        config.player1Deck[i] = Cards::FindCardByName("Blizzard");
    }

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curHand = *(curPlayer->GetHandZone());
    auto& curDeck = *(curPlayer->GetDeckZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Incanter's Flow"));

    CHECK_EQ(curHand[0]->GetCost(), 6);
    CHECK_EQ(curHand[1]->GetCost(), 6);
    CHECK_EQ(curHand[2]->GetCost(), 6);
    CHECK_EQ(curHand[3]->GetCost(), 6);
    for (auto& card : curDeck.GetAll())
    {
        CHECK_EQ(card->GetCost(), 6);
    }

    game.Process(curPlayer, PlayCardTask::Spell(card1));
    CHECK_EQ(curHand[0]->GetCost(), 6);
    CHECK_EQ(curHand[1]->GetCost(), 6);
    CHECK_EQ(curHand[2]->GetCost(), 6);
    CHECK_EQ(curHand[3]->GetCost(), 6);
    for (auto& card : curDeck.GetAll())
    {
        CHECK_EQ(card->GetCost(), 5);
    }
}

// ------------------------------------------ MINION - MAGE
// [BT_014] Starscryer - COST:2 [ATK:3/HP:1]
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Deathrattle:</b> Draw a spell.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
// --------------------------------------------------------
TEST_CASE("[Mage : Minion] - BT_014 : Starscryer")
{
    GameConfig config;
    config.player1Class = CardClass::MAGE;
    config.player2Class = CardClass::PALADIN;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = false;
    config.autoRun = false;

    for (int i = 0; i < 10; ++i)
    {
        config.player1Deck[i] = Cards::FindCardByName("Wolfrider");
        config.player1Deck[i + 10] = Cards::FindCardByName("Starscryer");
        config.player1Deck[i + 20] = Cards::FindCardByName("Pyroblast");
    }

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curHand = *(curPlayer->GetHandZone());
    auto& curField = *(curPlayer->GetFieldZone());
    auto& opField = *(opPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Starscryer"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Wolfrider"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curHand.GetCount(), 4);
    CHECK_EQ(curField.GetCount(), 1);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::Minion(card2));
    CHECK_EQ(opField.GetCount(), 1);

    game.Process(opPlayer, AttackTask(card2, card1));
    CHECK_EQ(curField.GetCount(), 0);
    CHECK_EQ(opField.GetCount(), 0);
    CHECK_EQ(curHand.GetCount(), 5);
    CHECK_EQ(curHand[4]->card->name, "Pyroblast");
}

// ------------------------------------------- SPELL - MAGE
// [BT_072] Deep Freeze - COST:8
//  - Set: BLACK_TEMPLE, Rarity: Rare
// --------------------------------------------------------
// Text: <b>Freeze</b> an enemy. Summon two 3/6 Water Elementals.
// --------------------------------------------------------
// GameTag:
//  - FREEZE = 1
// --------------------------------------------------------
TEST_CASE("[Mage : Speel] - BT_072 : Deep Freeze")
{
    GameConfig config;
    config.player1Class = CardClass::WARLOCK;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());
    auto& opField = *(opPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Flame Imp"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Deep Freeze"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField[0]->IsFrozen(), false);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    CHECK_EQ(curField[0]->IsFrozen(), true);
    CHECK_EQ(opField.GetCount(), 2);
    CHECK_EQ(opField[0]->card->name, "Water Elemental");
    CHECK_EQ(opField[1]->card->name, "Water Elemental");
}

// ---------------------------------------- SPELL - PALADIN
// [BT_011] Libram of Justice - COST:5
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Equip a 1/4 weapon.
//       Change the Health of all enemy minions to 1.
// --------------------------------------------------------
TEST_CASE("[Paladin : Spell] - BT_011 : Libram of Justice")
{
    GameConfig config;
    config.player1Class = CardClass::PALADIN;
    config.player2Class = CardClass::PALADIN;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Tirion Fordring"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Tirion Fordring"));
    const auto card3 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Libram of Justice"));

    auto& curField = *(curPlayer->GetFieldZone());
    auto& opField = *(opPlayer->GetFieldZone());

    game.Process(curPlayer, PlayCardTask::Minion(card1));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::Minion(card2));

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer, PlayCardTask::Spell(card3));
    CHECK_EQ(curPlayer->GetHero()->weapon->GetAttack(), 1);
    CHECK_EQ(curPlayer->GetHero()->weapon->GetDurability(), 4);
    CHECK_EQ(curField[0]->GetHealth(), 6);
    CHECK_EQ(opField[0]->GetHealth(), 1);
}

// ---------------------------------------- SPELL - PALADIN
// [BT_024] Libram of Hope - COST:9
//  - Set: BLACK_TEMPLE, Rarity: Epic
// --------------------------------------------------------
// Text: Restore 8 Health. Summon an 8/8 Guardian
//       with <b>Taunt</b> and <b>Divine Shield</b>.
// --------------------------------------------------------
// RefTag:
//  - DIVINE_SHIELD = 1
//  - TAUNT = 1
// --------------------------------------------------------
TEST_CASE("[Paladin : Spell] - BT_024 : Libram of Hope")
{
    GameConfig config;
    config.player1Class = CardClass::PALADIN;
    config.player2Class = CardClass::PRIEST;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);
    curPlayer->GetHero()->SetDamage(10);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Libram of Hope"));

    game.Process(curPlayer,
                 PlayCardTask::SpellTarget(card1, curPlayer->GetHero()));
    CHECK_EQ(curPlayer->GetHero()->GetHealth(), 28);
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Ancient Guardian");
    CHECK_EQ(curField[0]->GetAttack(), 8);
    CHECK_EQ(curField[0]->GetHealth(), 8);
    CHECK_EQ(curField[0]->HasTaunt(), true);
    CHECK_EQ(curField[0]->HasDivineShield(), true);
}

// ----------------------------------------- SPELL - PRIEST
// [BT_252] Renew - COST:1
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Restore 3 Health. <b>Discover</b> a spell.
// --------------------------------------------------------
// GameTag:
//  - DISCOVER = 1
// --------------------------------------------------------
TEST_CASE("[Preist : Spell] - BT_252 : Renew")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::HUNTER;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);
    curPlayer->GetHero()->SetDamage(5);

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Renew"));

    game.Process(curPlayer,
                 PlayCardTask::SpellTarget(card1, curPlayer->GetHero()));
    CHECK_EQ(curPlayer->GetHero()->GetHealth(), 28);
    CHECK(curPlayer->choice != nullptr);
    CHECK_EQ(curPlayer->choice->choices.size(), 3);

    auto cards = TestUtils::GetChoiceCards(game);
    for (auto& card : cards)
    {
        CHECK_EQ(card->GetCardType(), CardType::SPELL);
        CHECK_EQ(card->IsCardClass(CardClass::PRIEST), true);
    }
}

// ----------------------------------------- SPELL - PRIEST
// [BT_253] Psyche Split - COST:5
//  - Set: BLACK_TEMPLE, Rarity: Rare
// --------------------------------------------------------
// Text: Give a minion +1/+2. Summon a copy of it.
// --------------------------------------------------------
TEST_CASE("[Preist : Spell] - BT_253 : Psyche Split")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Psyche Split"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Loot Hoarder"));

    game.Process(curPlayer, PlayCardTask::Minion(card2));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->GetAttack(), 2);
    CHECK_EQ(curField[0]->GetHealth(), 1);

    game.Process(curPlayer, PlayCardTask::SpellTarget(card1, card2));
    CHECK_EQ(curField.GetCount(), 2);
    CHECK_EQ(curField[0]->GetAttack(), 3);
    CHECK_EQ(curField[0]->GetHealth(), 3);
    CHECK_EQ(curField[1]->GetAttack(), 3);
    CHECK_EQ(curField[1]->GetHealth(), 3);
}

// ----------------------------------------- SPELL - PRIEST
// [BT_257] Apotheosis - COST:3
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Give a minion +2/+3 and <b>Lifesteal</b>.
// --------------------------------------------------------
// RefTag:
//  - LIFESTEAL = 1
// --------------------------------------------------------
TEST_CASE("[Preist : Spell] - BT_257 : Apotheosis")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Apotheosis"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Loot Hoarder"));

    auto& curField = *(curPlayer->GetFieldZone());

    game.Process(curPlayer, PlayCardTask::Minion(card2));
    CHECK_EQ(curField[0]->GetGameTag(GameTag::LIFESTEAL), 0);
    CHECK_EQ(curField[0]->GetAttack(), 2);
    CHECK_EQ(curField[0]->GetHealth(), 1);

    game.Process(curPlayer, PlayCardTask::SpellTarget(card1, card2));
    CHECK_EQ(curField[0]->GetGameTag(GameTag::LIFESTEAL), 1);
    CHECK_EQ(curField[0]->GetAttack(), 4);
    CHECK_EQ(curField[0]->GetHealth(), 4);
}

// ---------------------------------------- MINION - PRIEST
// [BT_258] Imprisoned Homunculus - COST:1 [ATK:2/HP:5]
// - Race: Demon, Set: Black Temple, Rarity: Common
// --------------------------------------------------------
// Text: <b>Dormant</b> for 2 turns. <b>Taunt</b>
// --------------------------------------------------------
// GameTag:
// - TAUNT = 1
// --------------------------------------------------------
TEST_CASE("[Priest : Minion] - BT_258 : Imprisoned Homunculus")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::PALADIN;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Imprisoned Homunculus"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_1), 2);
    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_2), 0);
    CHECK_EQ(curField[0]->IsUntouchable(), true);
    CHECK_EQ(curField[0]->CanAttack(), false);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_2), 1);
    CHECK_EQ(curField[0]->IsUntouchable(), true);
    CHECK_EQ(curField[0]->CanAttack(), false);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_2), 2);
    CHECK_EQ(curField[0]->IsUntouchable(), false);
    CHECK_EQ(curField[0]->CanAttack(), false);
    CHECK_EQ(curField[0]->HasTaunt(), true);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    CHECK_EQ(curField[0]->CanAttack(), true);

    game.Process(curPlayer, AttackTask(card1, opPlayer->GetHero()));
    CHECK_EQ(opPlayer->GetHero()->GetHealth(), 28);
}

// ---------------------------------------- SPELL - PALADIN
// [BT_292] Hand of A'dal - COST: 2
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Give a minion +2/+2. Draw a card.
// --------------------------------------------------------
TEST_CASE("[Paladin : SPELL] - BT_292 : Hand of A'dal")
{
    GameConfig config;
    config.player1Class = CardClass::PALADIN;
    config.player2Class = CardClass::WARRIOR;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());
    auto& curHand = *(curPlayer->GetHandZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Hand of A'dal"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Wolfrider"));

    game.Process(curPlayer, PlayCardTask::Minion(card2));
    CHECK_EQ(curField[0]->GetAttack(), 3);
    CHECK_EQ(curField[0]->GetHealth(), 1);

    game.Process(curPlayer, PlayCardTask::SpellTarget(card1, card2));
    CHECK_EQ(curHand.GetCount(), 5);
    CHECK_EQ(curField[0]->GetAttack(), 5);
    CHECK_EQ(curField[0]->GetHealth(), 3);
}

// ----------------------------------------- MINION - ROGUE
// [BT_703] Cursed Vagrant - COST:7 [ATK:7/HP:5]
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Deathrattle:</b> Summon a 7/5 Shadow with <b>Stealth</b>.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
// --------------------------------------------------------
// RefTag:
//  - STEALTH = 1
// --------------------------------------------------------
TEST_CASE("[Rogue : Minion] - BT_703 : Cursed Vagrant")
{
    GameConfig config;
    config.player1Class = CardClass::ROGUE;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Cursed Vagrant"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Cursed Vagrant");

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Cursed Shadow");
    CHECK_EQ(curField[0]->GetAttack(), 7);
    CHECK_EQ(curField[0]->GetHealth(), 5);
    CHECK_EQ(curField[0]->HasStealth(), true);
}

// ----------------------------------------- SPELL - SHAMAN
// [BT_100] Serpentshrine Portal - COST:3
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Deal 3 damage. Summon a random 3-Cost minion.
//       <b>Overload:</b> (1)
// --------------------------------------------------------
// GameTag:
//  - OVERLOAD = 1
// --------------------------------------------------------
TEST_CASE("[Shaman : Spell] - BT_100 : Serpentshrine Portal")
{
    GameConfig config;
    config.player1Class = CardClass::SHAMAN;
    config.player2Class = CardClass::WARRIOR;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Serpentshrine Portal"));

    game.Process(curPlayer,
                 PlayCardTask::SpellTarget(card1, opPlayer->GetHero()));
    CHECK_EQ(opPlayer->GetHero()->GetHealth(), 27);
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->GetCost(), 3);
    CHECK_EQ(curPlayer->GetRemainingMana(), 7);
    CHECK_EQ(curPlayer->GetOverloadOwed(), 1);
    CHECK_EQ(curPlayer->GetOverloadLocked(), 0);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    CHECK_EQ(curPlayer->GetRemainingMana(), 9);
    CHECK_EQ(curPlayer->GetOverloadOwed(), 0);
    CHECK_EQ(curPlayer->GetOverloadLocked(), 1);
}

// ----------------------------------------- SPELL - SHAMAN
// [BT_113] Totemic Reflection - COST:3
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Give a minion +2/+2.
//       If it's a Totem, summon a copy of it.
// --------------------------------------------------------
// PlayReq:
// - REQ_TARGET_TO_PLAY = 0
// - REQ_MINION_TARGET = 0
// --------------------------------------------------------
TEST_CASE("[Shaman : Spell] - BT_113 : Totemic Reflection")
{
    GameConfig config;
    config.player1Class = CardClass::SHAMAN;
    config.player2Class = CardClass::SHAMAN;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());
    auto& opField = *(opPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Totemic Reflection"));
    const auto card2 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Totemic Reflection"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Mana Tide Totem"));
    const auto card4 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Dust Devil"));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::Minion(card3));
    game.Process(opPlayer, PlayCardTask::Minion(card4));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer, PlayCardTask::SpellTarget(card1, card3));
    game.Process(curPlayer, PlayCardTask::SpellTarget(card2, card4));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Mana Tide Totem");
    CHECK_EQ(curField[0]->GetAttack(), 2);
    CHECK_EQ(curField[0]->GetHealth(), 5);
    CHECK_EQ(opField[0]->GetAttack(), 2);
    CHECK_EQ(opField[0]->GetHealth(), 5);
    CHECK_EQ(opField[1]->GetAttack(), 5);
    CHECK_EQ(opField[1]->GetHealth(), 3);
}

// ---------------------------------------- SPELL - WARLOCK
// [BT_199] Unstable Felbolt - COST:1
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: Deal 3 damage to an enemy minion
//       and a random friendly one.
// --------------------------------------------------------
// PlayReq:
// - REQ_TARGET_TO_PLAY = 0
// - REQ_MINION_TARGET = 0
// - REQ_ENEMY_TARGET = 0
// --------------------------------------------------------
TEST_CASE("[Warlock : Spell] - BT_199 : Unstable Felbolt")
{
    GameConfig config;
    config.player1Class = CardClass::WARLOCK;
    config.player2Class = CardClass::DEMONHUNTER;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());
    auto& opField = *(opPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Unstable Felbolt"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Wolfrider"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Doomsayer"));

    game.Process(curPlayer, PlayCardTask::Minion(card2));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::Minion(card3));

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer, PlayCardTask::SpellTarget(card1, card3));
    CHECK_EQ(curField.GetCount(), 0);
    CHECK_EQ(opField.GetCount(), 1);
    CHECK_EQ(opField[0]->GetHealth(), 4);
}

// --------------------------------------- MINION - WARLOCK
// [BT_304] Enhanced Dreadlord - COST:8 [ATK:5/HP:7]
//  - Race: Demon, Set: BLACK_TEMPLE, Rarity: Rare
// --------------------------------------------------------
// Text: <b>Taunt</b> <b>Deathrattle:</b> Summon a 5/5
//       Dreadlord with <b>Lifesteal</b>.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
//  - TAUNT = 1
// --------------------------------------------------------
// RefTag:
//  - LIFESTEAL = 1
// --------------------------------------------------------
TEST_CASE("[Warlock : Minion] - BT_304 : Enhanced Dreadlord")
{
    GameConfig config;
    config.player1Class = CardClass::WARLOCK;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Enhanced Dreadlord"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Enhanced Dreadlord");
    CHECK_EQ(curField[0]->HasTaunt(), true);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    game.Process(opPlayer, PlayCardTask::SpellTarget(card3, card1));
    game.Process(opPlayer, PlayCardTask::SpellTarget(card3, card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Desperate Dreadlord");
    CHECK_EQ(curField[0]->GetAttack(), 5);
    CHECK_EQ(curField[0]->GetHealth(), 5);
    CHECK_EQ(curField[0]->HasLifesteal(), true);
}

// ---------------------------------------- SPELL - WARRIOR
// [BT_117] Bladestorm - COST:3
//  - Set: BLACK_TEMPLE, Rarity: Epic
// --------------------------------------------------------
// Text: Deal 1 damage to all minions. Repeat until one dies.
// --------------------------------------------------------
TEST_CASE("[Warrior : Spell] - BT_117 : Bladestorm")
{
    GameConfig config;
    config.player1Class = CardClass::WARRIOR;
    config.player2Class = CardClass::WARLOCK;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Bladestorm"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Target Dummy"));
    const auto card3 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Dire Mole"));

    game.Process(curPlayer, PlayCardTask::Minion(card2));
    game.Process(curPlayer, PlayCardTask::Minion(card3));

    game.Process(curPlayer, PlayCardTask::Spell(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Dire Mole");
    CHECK_EQ(curField[0]->GetHealth(), 1);
}

// --------------------------------------- MINION - WARRIOR
// [BT_120] Warmaul Challenger - COST:3 [ATK:1/HP:10]
//  - Set: BLACK_TEMPLE, Rarity: Epic
// --------------------------------------------------------
// Text: <b>Battlecry:</b> Choose an enemy minion.
//       Battle it to the death!
// --------------------------------------------------------
// PlayReq:
// - REQ_TARGET_IF_AVAILABLE = 0
// - REQ_MINION_TARGET = 0
// - REQ_ENEMY_TARGET = 0
// --------------------------------------------------------
// GameTag:
//  - BATTLECRY = 1
// --------------------------------------------------------
TEST_CASE("[Warrior : Minion] - BT_120 : Warmaul Challenger")
{
    GameConfig config;
    config.player1Class = CardClass::WARRIOR;
    config.player2Class = CardClass::WARLOCK;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());
    auto& opField = *(opPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Dire Mole"));
    const auto card2 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Warmaul Challenger"));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::Minion(card1));

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer, PlayCardTask::MinionTarget(card2, card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->GetHealth(), 7);
    CHECK_EQ(opField.GetCount(), 0);
}

// --------------------------------------- MINION - WARRIOR
// [BT_123] Kargath Bladefist - COST:4 [ATK:4/HP:4]
// - Set: BLACK_TEMPLE, Rarity: LEGENDARY
// --------------------------------------------------------
// Text: <b>Rush</b> <b>Deathrattle:</b> Shuffle
//       'Kargath Prime' into your deck.
// --------------------------------------------------------
// GameTag:
// - ELITE = 1
// - RUSH = 1
// - DEATHRATTLE = 1
// --------------------------------------------------------
TEST_CASE("[Warrior : Minion] - BT_123 : Kargath Bladefist")
{
    GameConfig config;
    config.player1Class = CardClass::WARRIOR;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;
    config.doShuffle = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curDeck = *(curPlayer->GetDeckZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Kargath Bladefist"));
    const auto card2 = Generic::DrawCard(
        opPlayer, Cards::FindCardByName("Reckless Rocketeer"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Wisp"));
    const auto card4 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Wisp"));

    // Player1 summon 'Kargath Bladefist'
    game.Process(curPlayer, PlayCardTask::Minion(card1));
    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    // Player2 summon minions and attack player1's 'Kargath Bladefist'
    game.Process(opPlayer, PlayCardTask::Minion(card2));
    game.Process(opPlayer, PlayCardTask::Minion(card3));
    game.Process(opPlayer, PlayCardTask::Minion(card4));
    game.Process(opPlayer, AttackTask(card2, card1));

    // Player1's 'Kargath Prime' move to top of the deck
    auto curCards = curDeck.GetAll();
    auto iter = std::find_if(
        curCards.begin(), curCards.end(),
        [](const Playable* dc) { return (dc->card->name) == "Kargath Prime"; });
    CHECK(iter != curCards.end());
    const auto card5 = *iter;
    curDeck.Swap(card5, curDeck.GetTopCard());

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    // Player1 summon 'Kargath Prime' and 'Kargath Prime' kill a minion
    game.Process(curPlayer, PlayCardTask::Minion(card5));
    game.Process(curPlayer, AttackTask(card5, card3));

    CHECK_EQ(curPlayer->GetHero()->GetArmor(), 10);
    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    // 'Kargath Prime' kill a minion
    game.Process(curPlayer, AttackTask(card5, card4));

    CHECK_EQ(curPlayer->GetHero()->GetArmor(), 20);
    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    // 'Kargath Prime' kill a hero
    opPlayer->GetHero()->SetHealth(10);
    game.Process(curPlayer, AttackTask(card5, opPlayer->GetHero()));
    CHECK_EQ(curPlayer->GetHero()->GetArmor(), 20);
}

// ---------------------------------------- SPELL - WARRIOR
// [BT_124] Corsair Cache - COST:2
// - Set: BLACK_TEMPLE, Rarity: Rare
// --------------------------------------------------------
// Text: Draw a weapon. Give it +1 Durability.
// --------------------------------------------------------
TEST_CASE("[Warrior : Spell] - BT_124 : Corsair Cache")
{
    GameConfig config;
    config.player1Class = CardClass::WARRIOR;
    config.player2Class = CardClass::WARLOCK;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = false;
    config.autoRun = false;
    config.doShuffle = false;

    for (int i = 0; i < 30; ++i)
    {
        config.player1Deck[i] = Cards::FindCardByName("Fiery War Axe");
    }

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    HandZone& curHand = *(curPlayer->GetHandZone());

    const auto card =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Corsair Cache"));

    game.Process(curPlayer, PlayCardTask::Spell(card));
    CHECK_EQ(curPlayer->GetRemainingMana(), 8);
    CHECK_EQ(curHand.GetCount(), 5);
    CHECK_EQ(dynamic_cast<Weapon*>(curHand[3])->GetDurability(), 2);
    CHECK_EQ(dynamic_cast<Weapon*>(curHand[4])->GetDurability(), 3);
}

// ----------------------------------- MINION - DEMONHUNTER
// [BT_509] Fel Summoner - COST:6 [ATK:8/HP:3]
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Deathrattle:</b> Summon a random Demon from your hand.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
// --------------------------------------------------------
TEST_CASE("[Demon Hunter : Minion] - BT_509 : Fel Summoner")
{
    GameConfig config;
    config.player1Class = CardClass::DEMONHUNTER;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(20);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Fel Summoner"));
    [[maybe_unused]] const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Battlefiend"));
    [[maybe_unused]] const auto card3 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Ur'zul Horror"));
    [[maybe_unused]] const auto card4 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Wolfrider"));
    const auto card5 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Fel Summoner");

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card5, card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->IsRace(Race::DEMON), true);
}

// ----------------------------------- MINION - DEMONHUNTER
// [BT_761] Coilfang Warlord - COST:8 [ATK:9/HP:5]
//  - Set: BLACK_TEMPLE, Rarity: Rare
// --------------------------------------------------------
// Text: <b>Rush</b> <b>Deathrattle:</b> Summon a
//       5/9 Warlord with <b>Taunt</b>.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
//  - RUSH = 1
// --------------------------------------------------------
// RefTag:
//  - TAUNT = 1
// --------------------------------------------------------
TEST_CASE("[Demon Hunter : Minion] - BT_761 : Coilfang Warlord")
{
    GameConfig config;
    config.player1Class = CardClass::DEMONHUNTER;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Coilfang Warlord"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Coilfang Warlord");
    CHECK_EQ(curField[0]->HasRush(), true);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Conchguard Warlord");
    CHECK_EQ(curField[0]->GetAttack(), 5);
    CHECK_EQ(curField[0]->GetHealth(), 9);
    CHECK_EQ(curField[0]->HasTaunt(), true);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_008] Rustsworn Initiate - COST:2 [ATK:2/HP:2]
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Deathrattle:</b> Summon a 1/1 Impcaster with
//       <b>Spell Damage +1</b>.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
// --------------------------------------------------------
// RefTag:
//  - SPELLPOWER = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_008 : Rustsworn Initiate")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Rustsworn Initiate"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Rustsworn Initiate");

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    CHECK_EQ(curField[0]->card->name, "Impcaster");
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->GetAttack(), 1);
    CHECK_EQ(curField[0]->GetHealth(), 1);
    CHECK_EQ(curField[0]->GetSpellPower(), 1);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_010] Felfin Navigator - COST:4 [ATK:4/HP:4]
//  - Race: Murloc, Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Battlecry:</b> Give your other Murlocs +1/+1.
// --------------------------------------------------------
// GameTag:
//  - BATTLECRY = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_010 : Felfin Navigator")
{
    GameConfig config;
    config.player1Class = CardClass::WARRIOR;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());
    auto& opField = *(opPlayer->GetFieldZone());

    const auto card1 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Felfin Navigator"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Murloc Raider"));
    const auto card3 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Murloc Raider"));
    const auto card4 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Wolfrider"));
    const auto card5 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Murloc Raider"));

    game.Process(curPlayer, PlayCardTask::Minion(card2));
    game.Process(curPlayer, PlayCardTask::Minion(card3));
    game.Process(curPlayer, PlayCardTask::Minion(card4));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::Minion(card5));

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField[0]->GetAttack(), 3);
    CHECK_EQ(curField[0]->GetHealth(), 2);
    CHECK_EQ(curField[1]->GetAttack(), 3);
    CHECK_EQ(curField[1]->GetHealth(), 2);
    CHECK_EQ(curField[2]->GetAttack(), 3);
    CHECK_EQ(curField[2]->GetHealth(), 1);
    CHECK_EQ(curField[3]->GetAttack(), 4);
    CHECK_EQ(curField[3]->GetHealth(), 4);
    CHECK_EQ(opField[0]->GetAttack(), 2);
    CHECK_EQ(opField[0]->GetHealth(), 1);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_155] Scrapyard Colossus - COST:10 [ATK:7/HP:7]
//  - Race: Elemental, Set: BLACK_TEMPLE, Rarity: Rare
// --------------------------------------------------------
// Text: <b>Taunt</b> <b>Deathrattle:</b> Summon a
//       7/7 Felcracked Colossus with <b>Taunt</b>.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
//  - TAUNT = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_155 : Scrapyard Colossus")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Scrapyard Colossus"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Scrapyard Colossus");

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    game.Process(opPlayer, PlayCardTask::SpellTarget(card3, card1));
    CHECK_EQ(curField[0]->card->name, "Felcracked Colossus");
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->GetAttack(), 7);
    CHECK_EQ(curField[0]->GetHealth(), 7);
    CHECK_EQ(curField[0]->HasTaunt(), true);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_156] Imprisoned Vilefiend - COST:2 [ATK:3/HP:5]
//  - Race: Demon, Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Dormant</b> for 2 turns. <b>Rush</b>
// --------------------------------------------------------
// GameTag:
//  - RUSH = 1
// --------------------------------------------------------
TEST_CASE("[Priest : Minion] - BT_156 : Imprisoned Vilefiend")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::PALADIN;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Imprisoned Vilefiend"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_1), 2);
    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_2), 0);
    CHECK_EQ(curField[0]->IsUntouchable(), true);
    CHECK_EQ(curField[0]->CanAttack(), false);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_2), 1);
    CHECK_EQ(curField[0]->IsUntouchable(), true);
    CHECK_EQ(curField[0]->CanAttack(), false);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    CHECK_EQ(curField[0]->GetGameTag(GameTag::TAG_SCRIPT_DATA_NUM_2), 2);
    CHECK_EQ(curField[0]->IsUntouchable(), false);
    CHECK_EQ(curField[0]->CanAttack(), false);
    CHECK_EQ(curField[0]->HasRush(), true);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    CHECK_EQ(curField[0]->CanAttack(), true);

    game.Process(curPlayer, AttackTask(card1, opPlayer->GetHero()));
    CHECK_EQ(opPlayer->GetHero()->GetHealth(), 27);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_159] Terrorguard Escapee - COST:3 [ATK:3/HP:7]
//  - Race: Demon, Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Battlecry:</b> Summon three 1/1 Huntresses
//       for your opponent.
// --------------------------------------------------------
// GameTag:
//  - BATTLECRY = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_159 : Terrorguard Escapee")
{
    GameConfig config;
    config.player1Class = CardClass::PALADIN;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& opField = *(opPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Terrorguard Escapee"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Wolfrider"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::Minion(card2));
    CHECK_EQ(opField.GetCount(), 4);
    CHECK_EQ(opField[0]->GetHealth(), 1);
    CHECK_EQ(opField[0]->GetAttack(), 1);
    CHECK_EQ(opField[0]->card->name, "Huntress");
    CHECK_EQ(opField[1]->GetHealth(), 1);
    CHECK_EQ(opField[1]->GetAttack(), 1);
    CHECK_EQ(opField[1]->card->name, "Huntress");
    CHECK_EQ(opField[2]->GetHealth(), 1);
    CHECK_EQ(opField[2]->GetAttack(), 1);
    CHECK_EQ(opField[2]->card->name, "Huntress");
}

// --------------------------------------- MINION - NEUTRAL
// [BT_160] Rustsworn Cultist - COST:4 [ATK:3/HP:3]
//  - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Battlecry:</b> Give your other minions
//       "<b>Deathrattle:</b> Summon a 1/1 Demon."
// --------------------------------------------------------
// GameTag:
//  - BATTLECRY = 1
// --------------------------------------------------------
// RefTag:
//  - DEATHRATTLE = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_160 : Rustsworn Cultist")
{
    GameConfig config;
    config.player1Class = CardClass::PALADIN;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Rustsworn Cultist"));
    const auto card2 =
        Generic::DrawCard(curPlayer, Cards::FindCardByName("Wolfrider"));
    const auto card3 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card2));
    game.Process(curPlayer, PlayCardTask::Minion(card1));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card3, card2));
    CHECK_EQ(curField.GetCount(), 2);
    CHECK_EQ(curField[0]->card->name, "Rusted Devil");
    CHECK_EQ(curField[0]->GetAttack(), 1);
    CHECK_EQ(curField[0]->GetHealth(), 1);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_715] Bonechewer Brawler - COST:2 [ATK:2/HP:3]
// - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Taunt</b> Whenever this minion takes damage,
//       gain +2 Attack.
// --------------------------------------------------------
// GameTag:
//  - TAUNT = 1
//  - TRIGGER_VISUAL = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_715 : Bonechewer Brawler")
{
    GameConfig config;
    config.player1Class = CardClass::WARLOCK;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Bonechewer Brawler"));

    game.Process(curPlayer, PlayCardTask::Minion(card));
    CHECK_EQ(curField[0]->HasTaunt(), true);
    CHECK_EQ(curField[0]->GetAttack(), 2);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, HeroPowerTask(card));
    CHECK_EQ(curField[0]->GetAttack(), 4);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, HeroPowerTask(card));
    CHECK_EQ(curField[0]->GetAttack(), 6);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_716] Bonechewer Vanguard - COST:7 [ATK:4/HP:10]
// - Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Taunt</b> Whenever this minion takes damage,
//       gain +2 Attack.
// --------------------------------------------------------
// GameTag:
//  - TAUNT = 1
//  - TRIGGER_VISUAL = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_716 : Bonechewer Vanguard")
{
    GameConfig config;
    config.player1Class = CardClass::WARLOCK;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Bonechewer Vanguard"));

    game.Process(curPlayer, PlayCardTask::Minion(card));
    CHECK_EQ(curField[0]->HasTaunt(), true);
    CHECK_EQ(curField[0]->GetAttack(), 4);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, HeroPowerTask(card));
    CHECK_EQ(curField[0]->GetAttack(), 6);

    game.Process(opPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, HeroPowerTask(card));
    CHECK_EQ(curField[0]->GetAttack(), 8);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_726] Dragonmaw Sky Stalker - COST:6 [ATK:5/HP:6]
//  - Race: Dragon, Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Deathrattle:</b> Summon a 3/4 Dragonrider.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
// --------------------------------------------------------
TEST_CASE("[Neutral : Minion] - BT_726 : Dragonmaw Sky Stalker")
{
    GameConfig config;
    config.player1Class = CardClass::PRIEST;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Dragonmaw Sky Stalker"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Dragonmaw Sky Stalker");

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Dragonrider");
    CHECK_EQ(curField[0]->GetAttack(), 3);
    CHECK_EQ(curField[0]->GetHealth(), 4);
}

// --------------------------------------- MINION - NEUTRAL
// [BT_728] Disguised Wanderer - COST:4 [ATK:3/HP:3]
//  - Race: Demon, Set: BLACK_TEMPLE, Rarity: Common
// --------------------------------------------------------
// Text: <b>Deathrattle:</b> Summon a 9/1 Inquisitor.
// --------------------------------------------------------
// GameTag:
//  - DEATHRATTLE = 1
// --------------------------------------------------------
TEST_CASE("[Netural : Minion] - BT_728 : Disguised Wanderer")
{
    GameConfig config;
    config.player1Class = CardClass::WARRIOR;
    config.player2Class = CardClass::MAGE;
    config.startPlayer = PlayerType::PLAYER1;
    config.doFillDecks = true;
    config.autoRun = false;

    Game game(config);
    game.Start();
    game.ProcessUntil(Step::MAIN_ACTION);

    Player* curPlayer = game.GetCurrentPlayer();
    Player* opPlayer = game.GetOpponentPlayer();
    curPlayer->SetTotalMana(10);
    curPlayer->SetUsedMana(0);
    opPlayer->SetTotalMana(10);
    opPlayer->SetUsedMana(0);

    auto& curField = *(curPlayer->GetFieldZone());

    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::FindCardByName("Disguised Wanderer"));
    const auto card2 =
        Generic::DrawCard(opPlayer, Cards::FindCardByName("Fireball"));

    game.Process(curPlayer, PlayCardTask::Minion(card1));

    game.Process(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_ACTION);

    game.Process(opPlayer, PlayCardTask::SpellTarget(card2, card1));
    CHECK_EQ(curField.GetCount(), 1);
    CHECK_EQ(curField[0]->card->name, "Rustsworn Inquisitor");
    CHECK_EQ(curField[0]->GetAttack(), 9);
    CHECK_EQ(curField[0]->GetHealth(), 1);
}