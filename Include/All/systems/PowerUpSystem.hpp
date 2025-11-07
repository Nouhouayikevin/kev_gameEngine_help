/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** PowerUpSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class PowerUpSystem : public ISystem {
    std::string _registerGroup;
public:
    PowerUpSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
    auto& registry = gameEng.getRegistry(_registerGroup);

    // Entités qui peuvent ramasser : les joueurs
    auto& players = registry.get_components<Component::Gameplay::PlayerTag>();
    auto& player_positions = registry.get_components<Component::Core::Position>();
    auto& player_hitboxes = registry.get_components<Component::Core::Hitbox>();
    auto& player_shooters = registry.get_components<Component::Gameplay::Shooter>();
    auto& player_dead_tags = registry.get_components<Component::Gameplay::Dead>();

    auto& player_healths = registry.get_components<Component::Gameplay::Health>();
    auto& player_scores = registry.get_components<Component::Gameplay::Score>(); 

    // Entités qui peuvent être ramassées : les power-ups
    auto& powerups = registry.get_components<Component::Gameplay::PowerUp>();
    auto& powerup_positions = registry.get_components<Component::Core::Position>();
    auto& powerup_hitboxes = registry.get_components<Component::Core::Hitbox>();
    auto& powerup_dead_tags = registry.get_components<Component::Gameplay::Dead>();

    // On teste chaque joueur contre chaque power-up
    for (size_t p_idx = 0; p_idx < players.size(); ++p_idx) {
        if (!players[p_idx] || !player_positions[p_idx] || !player_hitboxes[p_idx] || !player_shooters[p_idx] || (p_idx < player_dead_tags.size() && player_dead_tags[p_idx])) {
            continue;
        }
        auto& player_pos = *player_positions[p_idx];
        auto& player_hitbox = *player_hitboxes[p_idx];

        for (size_t pu_idx = 0; pu_idx < powerups.size(); ++pu_idx) {
            if (!powerups[pu_idx] || !powerup_positions[pu_idx] || !powerup_hitboxes[pu_idx] || (pu_idx < powerup_dead_tags.size() && powerup_dead_tags[pu_idx])) {
                continue;
            }
            auto& powerup_pos = *powerup_positions[pu_idx];
            auto& powerup_hitbox = *powerup_hitboxes[pu_idx];

            // Détection de collision AABB
            bool collision = 
                player_pos.x < powerup_pos.x + powerup_hitbox.width &&
                player_pos.x + player_hitbox.width > powerup_pos.x &&
                player_pos.y < powerup_pos.y + powerup_hitbox.height &&
                player_pos.y + player_hitbox.height > powerup_pos.y;
            
            if (collision) {
                auto& powerup = *powerups[pu_idx];
                auto& shooter = *player_shooters[p_idx];
                Entity player_entity(p_idx, &registry);

                switch (powerup.type) {
                    case Component::Gameplay::PowerUpType::CHANGE_WEAPON: {
                        // 1. Détruire l'ancienne entité arme
                        if (shooter.current_weapon_entity != (size_t)-1) {
                            registry.kill_entity(Entity(shooter.current_weapon_entity, &registry));
                        }
                        
                        // 2. Forger la nouvelle arme depuis son archétype
                        Entity new_weapon = gameEng.spawn_from_archetype(powerup.data.value("weapon_archetype", ""), _registerGroup);
                        
                        // 3. Équiper la nouvelle arme
                        shooter.current_weapon_entity = static_cast<size_t>(new_weapon);
                        break;
                    }
                    case Component::Gameplay::PowerUpType::HEAL: {
                        if (player_healths[p_idx]) {
                            auto& health = *player_healths[p_idx];
                            int heal_amount = powerup.data.value("amount", 0);
                            health.hp = std::min(health.max_hp, health.hp + heal_amount);
                        }
                        break;
                    }
                    case Component::Gameplay::PowerUpType::ADD_SCORE: {
                        if (player_scores[p_idx]) {
                            auto& score = *player_scores[p_idx];
                            int score_value = powerup.data.value("value", 0);
                            score.value += score_value;
                        }
                        break;
                    }
                    case Component::Gameplay::PowerUpType::SHIELD: {
                        if (player_healths[p_idx]) {
                            auto& health = *player_healths[p_idx];
                            int shield_amount = powerup.data.value("amount", 100);
                            health.hp = std::min(health.max_hp, health.hp + shield_amount);
                        }
                        break;
                    }
                    case Component::Gameplay::PowerUpType::SPEED_BOOST: {
                        // Augmente temporairement la vitesse du joueur
                        auto& velocities = registry.get_components<Component::Core::Velocity>();
                        if (p_idx < velocities.size() && velocities[p_idx]) {
                            float boost_multiplier = powerup.data.value("multiplier", 1.5f);
                            // On pourrait stocker la vitesse originale et la restaurer après un timer
                            // Pour l'instant, on ne fait qu'un boost permanent jusqu'à implémentation d'un timer
                        }
                        break;
                    }
                }

                // Le power-up a été consommé, on le marque pour destruction
                registry.add_component(Entity(pu_idx, &registry), Component::Gameplay::Dead{});
                
                // Un seul joueur peut ramasser un power-up
                break;
            }
        }
    }
}
};