/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** MovementSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class MovementSystem : public ISystem {
std::string _registerGroup;
public:
    MovementSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        auto& positions = registry.get_components<Component::Core::Position>();
        auto& velocities = registry.get_components<Component::Core::Velocity>();
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

        for (size_t i = 0; i < positions.size(); ++i) {
            if (i < dead_tags.size() && dead_tags[i]) continue;
            
            if (positions[i] && velocities[i]) {
                positions[i]->x += velocities[i]->dx * dt;
                positions[i]->y += velocities[i]->dy * dt;
            }
        }
    }
};