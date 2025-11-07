/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** ScoreScene.hpp
*/

#pragma once

#include "../interfaces/IScene.hpp"
#include "../../GameEngine_Include/Registry.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"

class ScoreScene : public IScene {
private:
    Registry _registry;

public:
    void init(GameEngine& context) override;
    void update(GameEngine& context, float delta_time) override;
    void render(GameEngine& context) override;

};
