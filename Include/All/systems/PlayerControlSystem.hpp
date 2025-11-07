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

class PlayerControlSystem : public ISystem {
std::string _registerGroup;
public:
    PlayerControlSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        Registry & registry = gameEng.getRegistry(_registerGroup);
        auto& velocities = registry.get_components<Component::Core::Velocity>();
        auto& controllables = registry.get_components<Component::Gameplay::Controllable>();

        for (size_t i = 0; i < velocities.size() && i < controllables.size(); i++) {
            auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

            if (i < dead_tags.size() && dead_tags[i]) continue;
            if (velocities[i] && controllables[i]) {
                velocities[i]->dx = 0;
                velocities[i]->dy = 0;
                float player_speed = 200.0f;

                if (gameEng.getInputManager().isKeyDown(KeyCode::Up)) velocities[i]->dy = -player_speed;
                if (gameEng.getInputManager().isKeyDown(KeyCode::Down)) velocities[i]->dy = player_speed;
                if (gameEng.getInputManager().isKeyDown(KeyCode::Left)) velocities[i]->dx = -player_speed;
                if (gameEng.getInputManager().isKeyDown(KeyCode::Right)) velocities[i]->dx = player_speed;

                if (gameEng.getInputManager().isKeyPressed(KeyCode::Space)) {
                     auto& positions = registry.get_components<Component::Core::Position>();
                    if (positions[i]) {
                        // Entity missile = registry.spawn_entity();
                        // registry.add_component(missile, Component::Core::Hitbox(10, 10));
                        // registry.add_component(missile, Component::Gameplay::Damage(20));
                        // registry.add_component(missile, Component::Core::Position(positions[i]->x + 50, positions[i]->y + 20));
                        // registry.add_component(missile, Component::Core::Velocity(400.0f, 0.0f));
                        // registry.add_component(missile, Component::Gameplay::MissileTag(Component::Gameplay::OwnerType::PLAYER, 0)); 
                        // registry.add_component(missile, Component::Audio::SoundEffect(laser_sound_id, true));
                        // missile.add_component<Component::Graphics::CircleShape>(Component::Graphics::CircleShape(5.0f, Component::Core::Color(255, 0, 0, 255)));
                    }
                }
            }
        }
    }
};