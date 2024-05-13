#include "stdafx.h"

#include "DebugMenu.h"
#include "Game.h"

enum CameraChoices
{
	CAMERA_GAME,
	CAMERA_FREE,
};

DebugMenu::DebugMenu(std::shared_ptr<hl::Drawer> drawer, std::shared_ptr<Input> input) : Menu(drawer, input)
{
	m_name = "Larry Craft Deboog";

	m_menu_items.push_back(std::make_shared<MenuItem>("---- Deboog Menu ----"));

	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("Fly Mode", [](bool on) {
		auto& flags = Game::Player::Get()->getFlags();
		flags.Set(Game::Player::PlayerFlags::FlyMode, on);
	}, []() {
		const auto& flags = Game::Player::Get()->getFlags();
		return flags.Get(Game::Player::PlayerFlags::FlyMode);
	}));
	m_selected_item = m_menu_items.back();

	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("Ignore Wall Collision"));
	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("Disable Triggers"));
	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("God Mode"));
	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("Lorem"));
	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("ipsum"));
	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("dolor"));
	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("sit"));
	m_menu_items.push_back(std::make_shared<BooleanMenuItem>("amet"));

	m_menu_items.push_back(std::make_shared<ActionMenuItem>("Crash", []() { assert(0); }));

	m_menu_items.push_back(std::make_shared<ChoiceMenuItem>("Camera", std::vector<ChoiceMenuItem::MenuChoice> {
		{ CAMERA_GAME, "Game" },
		{ CAMERA_FREE, "Free" }
	}, [](int choice) {
		
	}, []() {
		return CAMERA_FREE;
	}));
}
