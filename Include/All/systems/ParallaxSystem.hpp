/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** parralax.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class ParallaxSystem : public ISystem {
std::string _registerGroup;
public:
    ParallaxSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        Registry & registry = gameEng.getRegistry(_registerGroup);

        auto& positions = registry.get_components<Component::Core::Position>();
        auto& parallaxes = registry.get_components<Component::Gameplay::ParallaxLayer>();
        
        for (size_t i = 0; i < positions.size() && i < parallaxes.size(); i++) {
            auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

            if (i < dead_tags.size() && dead_tags[i]) continue;
            if (positions[i] && parallaxes[i]) {
                if (positions[i]->x < -1920) {
                    positions[i]->x += 1920 * 2;
                }
            }
        }
    }
};