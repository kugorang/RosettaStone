/*************************************************************************
> File Name: Console.h
> Project Name: Hearthstone++
> Author: Chan-Ho Chris Ohk
> Purpose: Console version of Hearthstone++ game.
> Created Time: 2017/10/08
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef HEARTHSTONEPP_CONSOLE_H
#define HEARTHSTONEPP_CONSOLE_H

#include <Commons/Constants.h>

#include <array>
#include <functional>

namespace Hearthstonepp
{
	class Console
	{
	public:
		void ShowMenu();
		size_t InputMenuNum();
		int Play();

		void SearchCard();
		void ShowCardInfo();
		void MakeDeck();
		void LoadDeck();
		void StoreDeck();
		void SimulateGame();

	private:
		std::array<std::string, MENU_SIZE> m_menuStr =
		{
			"1. Search Card",
			"2. Show Card Information",
			"3. Make Deck",
			"4. Load Deck",
			"5. Store Deck",
			"6. Simulate Game",
			"7. Exit"
		};
		std::array<std::function<void(Console&)>, MENU_SIZE - 1> m_menuFuncs =
		{
			&Console::SearchCard,
			&Console::ShowCardInfo,
			&Console::MakeDeck,
			&Console::LoadDeck,
			&Console::StoreDeck,
			&Console::SimulateGame
		};
	};
}

#endif