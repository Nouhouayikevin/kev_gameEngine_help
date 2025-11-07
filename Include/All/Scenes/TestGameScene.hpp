/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** GameScene.hpp
*/

#pragma once

#include "../interfaces/IScene.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"

#include "../systems/PlayerControlSystem.hpp"
#include "../systems/EnemyShootingSystem.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/AnimationSystem.hpp"
#include "../systems/AISystem.hpp"
#include "../systems/DeathSystem.hpp"
#include "../systems/SoundSystem.hpp"
#include "../systems/MusicSystem.hpp"
#include "../systems/HealthBarSystem.hpp"
#include "../systems/CollisionSystem.hpp"
// #include "../systems/BossCollisionSystem.hpp"
#include "../systems/ParallaxSystem.hpp"
#include "../systems/LifespanSystem.hpp"
#include "../systems/PlayerShootingSystem.hpp"
#include "../systems/WeaponSystem.hpp"
#include "../systems/PowerUpSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/ScriptSystem.hpp"
#include "../systems/SaveLoadSystem.hpp"

#include <string>

class TestGameScene : public IScene {
private:
    std::string group = "TestGameScene";

public:
    void init(GameEngine& context) override;
    void update(GameEngine& context, float delta_time) override;
    void render(GameEngine& context) override;
};
