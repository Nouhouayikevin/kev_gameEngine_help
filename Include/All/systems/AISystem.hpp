/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** AISystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class AISystem : public ISystem {
    std::string _registerGroup;
public:
    AISystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);

        auto& velocities = registry.get_components<Component::Core::Velocity>();
        auto& ais = registry.get_components<Component::Gameplay::AI>();

        for (size_t i = 0; i < ais.size(); i++) {
            auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

            if (i < dead_tags.size() && dead_tags[i]) continue;
            if (!ais[i] || !velocities[i]) continue;

            auto& ai = *ais[i];
            auto& vel = *velocities[i];

            ai.time_elapsed += dt;
            float t = ai.time_elapsed;

            float base_dx = -100.0f;
            vel.dx = base_dx;

            switch (ai.type) {
                case Component::Gameplay::AIMovementType::VERTICAL_OSC: {
                    vel.dy = ai.amplitude * ai.frequency * cos(ai.frequency * t);
                    break;
                }
                case Component::Gameplay::AIMovementType::SINUSOIDAL: {
                    vel.dy = ai.amplitude * ai.frequency * cos(ai.frequency * t);
                    break;
                }
                case Component::Gameplay::AIMovementType::CIRCULAR: {
                    vel.dx = base_dx - ai.amplitude * ai.frequency * sin(ai.frequency * t);
                    vel.dy = ai.amplitude * ai.frequency * cos(ai.frequency * t);
                    break;
                }
                case Component::Gameplay::AIMovementType::LEMNISCATE: {

                    vel.dx = base_dx + ai.amplitude * ai.frequency * cos(ai.frequency * t);
                    vel.dy = (ai.amplitude / 2.0f) * (ai.frequency * 2.0f) * cos(ai.frequency * 2.0f * t);
                    break;
                }
                case Component::Gameplay::AIMovementType::STRAIGHT:
                default:
                    break;
            }
        }
    }
};