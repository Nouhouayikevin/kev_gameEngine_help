/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** LifespanSystem.hpp
*/


#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class LifespanSystem : public ISystem {
    std::string _registerGroup;
public:
    LifespanSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        auto& lifespans = registry.get_components<Component::Gameplay::TimedLifespan>();
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

        for (size_t i = 0; i < lifespans.size(); ++i) {
            if (!lifespans[i] || (i < dead_tags.size() && dead_tags[i])) continue;

            lifespans[i]->remaining_time -= dt;
            if (lifespans[i]->remaining_time <= 0) {
                registry.add_component(Entity(i, &registry), Component::Gameplay::Dead{});
            }
        }
    }
};