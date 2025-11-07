/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Wed, Oct, 2025                                                     *
 * Title           - G-CPP-500-COT-5-1-rtype-24                                         *
 * Description     -                                                                    *
 *     OfflineApplication                                                               *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *       _|_|_|_|  _|_|_|    _|_|_|  _|_|_|_|_|  _|_|_|_|    _|_|_|  _|    _|           *
 *       _|        _|    _|    _|        _|      _|        _|        _|    _|           *
 *       _|_|_|    _|_|_|      _|        _|      _|_|_|    _|        _|_|_|_|           *
 *       _|        _|          _|        _|      _|        _|        _|    _|           *
 *       _|_|_|_|  _|        _|_|_|      _|      _|_|_|_|    _|_|_|  _|    _|           *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */


#include "../include/OfflineApplication.hpp"
#include "../All/sfml/SfmlWindow.hpp"
#include "../All/sfml/SfmlInputManager.hpp"
#include "../All/sfml/SfmlResourceFactory.hpp"
#include "../All/Scenes/MenuScene.hpp"
#include "../All/Scenes/GameScene.hpp"
#include "../All/Scenes/ConnectionScene.hpp"


OfflineApplication::OfflineApplication()
{
    std::cout << "OfflineApplication created." << std::endl;
}

OfflineApplication::~OfflineApplication()
{
    std::cout << "OfflineApplication destroyed." << std::endl;
}

void OfflineApplication::initialize()
{
    std::cout << "Initializing OfflineApplication..." << std::endl;
    
    engine->getSceneManager().add_scene("menu", std::make_shared<MenuScene>());
    //engine.getSceneManager().add_scene("connection", std::make_shared<>());
    engine->getSceneManager().add_scene("offline_game", std::make_shared<GameScene>());
    engine->getSceneManager().switch_to_scene("offline_game", *engine);

    engine->initialize();
    std::cout << "OfflineApplication initialized successfully." << std::endl;
}

void OfflineApplication::run() {
    std::cout << "Starting offline game loop..." << std::endl;
    Clock& clock = engine->getClock();
    IWindow& window = engine->getWindowManager();
    IInputManager& input = engine->getInputManager();

    while (window.isOpen()) {
        float dt = clock.restart();
        input.update();
        if (input.isWindowClosed())
            window.close();
        engine->getSceneManager().update_current(*engine, dt);
        window.clear();
        engine->getSceneManager().render_current(*engine);
        window.display();
    }
    std::cout << "Offline game loop finished." << std::endl;
}

void OfflineApplication::shutdown()
{
    std::cout << "Shutting down OfflineApplication..." << std::endl;
    engine->shutdown();
    std::cout << "OfflineApplication shutdown complete." << std::endl;
}