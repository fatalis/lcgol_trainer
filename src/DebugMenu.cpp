#include "Trainer.h"
#include "assert.h"
#include "DebugMenu.h"
#include "EntityViewer.h"
#include <memory>
#include "Game.h"

enum CameraChoices
{
    CAMERA_GAME,
    CAMERA_FREE,
};

DebugMenu::DebugMenu(std::shared_ptr<hl::Drawer> drawer,
        std::shared_ptr<Input> input,
        std::shared_ptr<EntityViewer> entity_viewer,
        std::shared_ptr<Trainer> trainer) : Menu(drawer, input)
{
    m_name = "lcgol trainer by fatalis";

    m_menu_items.push_back(std::make_shared<MenuItem>("---- Debug Menu ----"));
    m_menu_items.push_back(std::make_shared<MenuItem>("* = not implemented yet"));

    m_menu_items.push_back(std::make_shared<BooleanMenuItem>("Noclip", [](bool on) {
        auto& flags = Game::Player::Get()->getFlags();
        flags.Set(Game::Player::PlayerFlags::FlyMode, on);
    }, []() {
        const auto& flags = Game::Player::Get()->getFlags();
        return flags.Get(Game::Player::PlayerFlags::FlyMode);
    }));

    m_menu_items.push_back(std::make_shared<BooleanMenuItem>("Entity Viewer", [=](bool on) {
        entity_viewer->m_visible = on;
    }, [=]() {
        return entity_viewer->m_visible;
    }));
    m_selected_item = m_menu_items.back();

    m_menu_items.push_back(std::make_shared<BooleanMenuItem>("Wireframe", [=](bool on) {
        trainer->m_wireframe = on;
    }, [=]() {
        return trainer->m_wireframe;
    }));

    m_menu_items.push_back(std::make_shared<BooleanMenuItem>("* Ignore Wall Collision"));
    m_menu_items.push_back(std::make_shared<BooleanMenuItem>("* Disable Triggers"));
    m_menu_items.push_back(std::make_shared<BooleanMenuItem>("* God Mode"));

    m_menu_items.push_back(std::make_shared<ActionMenuItem>("Crash", []() { assert(0); }));

    m_menu_items.push_back(std::make_shared<ChoiceMenuItem>("* Camera", std::vector<ChoiceMenuItem::MenuChoice> {
        { CAMERA_GAME, "Game" },
        { CAMERA_FREE, "Free" }
    }, [](int choice) {
        
    }, []() {
        return CAMERA_FREE;
    }));
}
