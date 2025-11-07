/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** EnemyShootingSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class EnemyShootingSystem : public ISystem {
std::string _registerGroup;
public:
    EnemyShootingSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);

        // On cherche les entités qui sont des ennemis ET qui peuvent tirer.
        auto& shooters = registry.get_components<Component::Gameplay::Shooter>();
        auto& enemy_tags = registry.get_components<Component::Gameplay::EnemyTag>();
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

        static bool first_log = true;
        int enemy_shooter_count = 0;

        for (size_t i = 0; i < shooters.size(); ++i) {
            if (!shooters[i] || !enemy_tags[i] || (i < dead_tags.size() && dead_tags[i])) {
                continue;
            }

            enemy_shooter_count++;
            auto& shooter = *shooters[i];

            // --- GESTION DE L'ARME INITIALE (exactement comme le joueur) ---
            if (shooter.current_weapon_entity == (size_t)-1) {
                // Pour les ennemis, le composant Weapon est souvent sur la même entité
                // On vérifie si cette entité a un composant Weapon
                auto& weapons = registry.get_components<Component::Gameplay::Weapon>();
                if (i < weapons.size() && weapons[i]) {
                    // L'arme est sur cette même entité, on l'utilise directement
                    shooter.current_weapon_entity = i;
                    std::cout << "EnemyShootingSystem: Enemy " << i << " using self-weapon (ID " << i << ")" << std::endl;
                } else {
                    continue;  // Skip this entity if it has no weapon
                }
            }

            // Gestion du cooldown et de la décision de tir
            if (shooter.fire_timer > 0) {
                shooter.fire_timer -= dt;
            } else {
                // Si l'ennemi peut faire des tirs chargés et n'est pas déjà en train de charger
                if (shooter.can_charged_shot && !shooter.is_ai_charging) {
                    // Décision aléatoire de commencer un tir chargé
                    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                    if (random < shooter.charge_probability) {
                        shooter.is_ai_charging = true;
                        shooter.charge_timer = 0.0f;
                        shooter.is_charging = true;
                        continue;
                    }
                }

                // Si on est en train de charger
                if (shooter.is_ai_charging) {
                    shooter.charge_timer += dt;
                    // Si le temps de charge est suffisant, on tire
                    if (shooter.charge_timer >= shooter.charge_duration) {
                        shooter.fire_timer = -1.0f;
                        shooter.is_ai_charging = false;
                    }
                } else {
                    // Tir normal
                    shooter.fire_timer = -1.0f;
                }
            }
        }

        if (first_log && enemy_shooter_count > 0) {
            std::cout << "EnemyShootingSystem: Found " << enemy_shooter_count << " enemy shooters" << std::endl;
            first_log = false;
        }
    }
};