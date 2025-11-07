/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** MenuScene.cpp
*/

#include "../../Include/All/Scenes/MenuScene.hpp"
#include "../../Include/All/Component.hpp"
#include "../../Include/All/systems/ParallaxSystem.hpp"
#include "../../Include/All/systems/ButtonSystem.hpp"
#include "../../Include/All/systems/RenderSystem.hpp"
#include <iostream>

void MenuScene::init(GameEngine& gameSceneEngine)
{
    std::cout << "Initializing MenuScene..." << std::endl;

    gameSceneEngine.createRegistry(group);
    auto& registry = gameSceneEngine.getRegistry(group);

    // Enregistrer les systèmes
    registry.add_system("update", ParallaxSystem(group));
    registry.add_system("update", ButtonSystem(group));
    registry.add_system("render", RenderSystem(group));

    // Spawner les entités du menu depuis les archétypes
    // Background parallax
    gameSceneEngine.spawn_from_archetype("UI.parallax_layer", group, Component::Core::Position(0, 0));
    gameSceneEngine.spawn_from_archetype("UI.parallax_layer", group, Component::Core::Position(1280, 0));
    
    // Titres
    Entity title = gameSceneEngine.spawn_from_archetype("UI.menu_title", group);
    title.set_name("GameTitle");
    
    Entity subtitle = gameSceneEngine.spawn_from_archetype("UI.menu_subtitle", group);
    subtitle.set_name("SubTitle");
    
    // Boutons
    Entity online_btn = gameSceneEngine.spawn_from_archetype("UI.button_online", group);
    online_btn.set_name("OnlineButton");
    
    Entity offline_btn = gameSceneEngine.spawn_from_archetype("UI.button_offline", group);
    offline_btn.set_name("OfflineButton");
    
    Entity quit_btn = gameSceneEngine.spawn_from_archetype("UI.button_quit", group);
    quit_btn.set_name("QuitButton");
    
    // Configuration post-spawn: lier les actions aux boutons
    if (online_btn.has_component<Component::UI::Action>()) {
        auto& action = online_btn.get_component<Component::UI::Action>();
        action.on_click = []() {
            std::cout << "✅ [MENU] ONLINE button clicked!" << std::endl;
            // TODO: Activer plus tard
            // SetMode("online", "", 0);
        };
    }
    
    if (offline_btn.has_component<Component::UI::Action>()) {
        auto& action = offline_btn.get_component<Component::UI::Action>();
        action.on_click = []() {
            std::cout << "✅ [MENU] OFFLINE button clicked!" << std::endl;
            // TODO: Activer plus tard
            // SetMode("offline", "", 0);
        };
    }
    
    if (quit_btn.has_component<Component::UI::Action>()) {
        auto& action = quit_btn.get_component<Component::UI::Action>();
        action.on_click = [&gameSceneEngine]() {
            std::cout << "✅ [MENU] QUIT button clicked! Closing window..." << std::endl;
            gameSceneEngine.getWindowManager().close();
        };
    }
    
    std::cout << "MenuScene initialized." << std::endl;
}

void MenuScene::update(GameEngine& gameSceneEngine, float delta_time)
{
    auto& registry = gameSceneEngine.getRegistry(group);
    registry.run_systems("update", gameSceneEngine, delta_time);
}

void MenuScene::render(GameEngine& gameEngine)
{
    auto& registry = gameEngine.getRegistry(group);
    registry.run_systems("render", gameEngine, 0.0f);
}
