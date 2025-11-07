/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** GameScene.cpp
*/

#include <iostream>
#include <random>
#include <algorithm>
#include "../../Include/GameEngine_Include/core/GameEngine.hpp"
#include "../../Include/All/Scenes/TestGameScene.hpp"
#include "../../Include/All/Component.hpp"
#include "../../Include/All/interfaces/ISprite.hpp"
#include "../../Include/All/interfaces/ISound.hpp"
#include "../../Include/All/systems/ForceSystem.hpp"
#include "../../Include/All/systems/ForceControlSystem.hpp"
#include "../../Include/All/systems/ForceCollisionSystem.hpp"
#include "../../Include/All/systems/ForceInitSystem.hpp"
#include "../../Include/All/systems/OffScreenDeathSystem.hpp"


void TestGameScene::init(GameEngine& gameSceneEngine)
{
    std::cout << "Initializing TestGameScene..." << std::endl;

    gameSceneEngine.createRegistry(group);
    auto &registry = gameSceneEngine.getRegistry(group);
    auto &levelManager = gameSceneEngine.getLevelManager();

    // Les composants sont maintenant enregistrés automatiquement dans le constructeur du Registry

    // Enregistrer les systèmes
    registry.add_system("update", PlayerControlSystem(group));
    registry.add_system("update", MovementSystem(group));
    registry.add_system("update", AISystem(group));
    registry.add_system("update", EnemyShootingSystem(group));
    registry.add_system("update", CollisionSystem(group));
    registry.add_system("update", AnimationSystem(group));
    registry.add_system("update", ParallaxSystem(group));
    registry.add_system("update", HealthBarSystem(group));
    registry.add_system("update", SoundSystem(group));
    registry.add_system("update", MusicSystem(group));
    registry.add_system("update", PlayerShootingSystem(group));

    // Force Systems 🔵
    registry.add_system("update", ForceInitSystem(group));  // S'exécute une fois pour lier la Force au joueur
    registry.add_system("update", ForceSystem(group));
    registry.add_system("update", ForceControlSystem(group));
    registry.add_system("update", ForceCollisionSystem(group));
    
    // Cleanup System 🧹
    registry.add_system("update", OffScreenDeathSystem(group));  // Détruit les entités hors écran

    registry.add_system("update", WeaponSystem(group));
    registry.add_system("update", PowerUpSystem(group));
    registry.add_system("update", LifespanSystem(group));
    
    registry.add_system("render", RenderSystem(group));
    registry.add_system("update", DeathSystem(group));

    registry.add_system("update", SaveLoadSystem(group));
    
    registry.add_system("update", ScriptSystem(group));
    // registry.add_system("update", BossCollisionSystem(group));

    levelManager.set_register_group(group);
    // levelManager.load("Config/levels/level_1.json");  // 🎮 TEST DU BOSS
    levelManager.load("Config/levels/test_powerups.json");  // 🎮 TEST DES POWERUPS
    // levelManager.load("Config/levels/test_projectiles_only.json");
    
    std::cout << "TestGameScene Initialized." << std::endl;
}

void TestGameScene::update(GameEngine& gameSceneEngine, float delta_time)
{
    auto &registry = gameSceneEngine.getRegistry(group);
    auto& replay_manager = gameSceneEngine.getReplayManager();

    if (replay_manager.get_state() == ReplayState::PLAYING) {
        // En mode lecture, on ne fait TOURNER AUCUN SYSTÈME de logique.
        // On laisse le ReplayManager mettre à jour le Registry.
        replay_manager.update(delta_time);

    } else {
        // En mode normal ou enregistrement, on exécute toute la logique de jeu.
        registry.run_systems("update", gameSceneEngine, delta_time);
        gameSceneEngine.getLevelManager().update(delta_time, group);

        // Si on est en mode enregistrement, on dit au manager de prendre sa photo
        // MAINTENANT, *après* que tous les systèmes de logique ont tourné.
        if (replay_manager.get_state() == ReplayState::RECORDING) {
            replay_manager.update(delta_time);
        }
    }
}

void TestGameScene::render(GameEngine& gameEngine)
{
    auto &registry = gameEngine.getRegistry(group);
    registry.run_systems("render",gameEngine, 0.0f);
}
