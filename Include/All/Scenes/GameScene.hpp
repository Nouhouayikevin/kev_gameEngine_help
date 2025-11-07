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

class GameScene : public IScene {
private:
    float _enemy_spawn_timer = 2.0f;
    //GameEngine gameSceneEngine;

public:
    void init(GameEngine& context) override;
    void update(GameEngine& context, float delta_time) override;
    void render(GameEngine& context) override;
};
