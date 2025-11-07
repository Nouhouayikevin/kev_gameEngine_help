/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** DeathSystem.hpp
*/

/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** SoundSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class DeathSystem : public ISystem {
std::string _registerGroup;
public:
    DeathSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();
        
        std::vector<Entity> to_kill;
        for (size_t i = 0; i < dead_tags.size(); i++) {
            if (dead_tags[i]) {
                to_kill.emplace_back(Entity(i, &registry));
            }
        }

        for (const auto& e : to_kill) {
            registry.kill_entity(e);
        }
    }
};