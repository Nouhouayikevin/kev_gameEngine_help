/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** Application.cpp
*/

#include "../Include/All/sfml/SfmlWindow.hpp"
#include "../Include/All/sfml/SfmlInputManager.hpp"
#include "../Include/All/sfml/SfmlResourceFactory.hpp"

#include "../Include/All/Scenes/TestGameScene.hpp"
#include "../Include/All/Scenes/MenuScene.hpp"


#include "../Include/GameEngine_Include/testClass/TestApplication.hpp"
#include <iostream>
#include <stdexcept>


TestApplication::TestApplication() {
    std::cout << "TestApplication created." << std::endl;
}

TestApplication::~TestApplication() {
    std::cout << "TestApplication destroyed." << std::endl;
}

void TestApplication::initialize() {
    std::cout << "Initializing TestApplication..." << std::endl;

    // --- 1. Créer les modules concrets (SFML) ---
    auto window = std::make_unique<SfmlWindow>(1280, 720, "R-Type");
    
    // L'InputManager a besoin d'une référence vers la fenêtre pour fonctionner
    auto input = std::make_unique<SfmlInputManager>(*window); 

    // --- 2. "Brancher" les modules dans le moteur ---
    _engine.setWindow(std::move(window));
    _engine.setInputManager(std::move(input));

    // 3. Créer une instance de l'usine SFML
    auto resource_factory = std::make_unique<SfmlResourceFactory>();
    
    // 4. L'injecter dans le ResourceManager du moteur
    _engine.getResourceManager().setResourceFactory(std::move(resource_factory));

    // --- 5. Initialiser le moteur ---
    _engine.initialize();

    // --- 6. Configurer le SceneManager avec les scènes du jeu ---
    _scene_manager.add_scene("menu", std::make_shared<MenuScene>());
    _scene_manager.add_scene("game", std::make_shared<TestGameScene>());

    // --- 7. Démarrer sur la scène du menu ---
    _scene_manager.switch_to_scene("game", _engine);

}

void TestApplication::run() {
    std::cout << "Starting game loop..." << std::endl;

    Clock& clock = _engine.getClock();
    
    IWindow& window = _engine.getWindowManager();
    IInputManager& input = _engine.getInputManager();

    while (window.isOpen()) {
        float dt = clock.restart();

        // 1. Mettre à jour les entrées (poll events)
        input.update(); 
        if (input.isWindowClosed()) {
            window.close();
        }
        

        // 2. Mettre à jour la logique de la scène courante
        _scene_manager.update_current(_engine, dt);

        // 3. Rendu
        window.clear();
        _scene_manager.render_current(_engine);
        window.display();
    }
}

void TestApplication::shutdown() {
    std::cout << "Shutting down TestApplication..." << std::endl;
    _engine.shutdown();
}