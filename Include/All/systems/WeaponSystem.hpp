/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** WeaponSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class WeaponSystem : public ISystem {
    std::string _registerGroup;
public:
    WeaponSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        
        // On récupère toutes les pièces dont on a besoin
        auto& shooters = registry.get_components<Component::Gameplay::Shooter>();
        auto& weapons = registry.get_components<Component::Gameplay::Weapon>();
        auto& positions = registry.get_components<Component::Core::Position>();

        for (size_t i = 0; i < shooters.size(); ++i) {
            // Est-ce une entité capable de tirer et qui a une position ?
            if (!shooters[i] || !positions[i]) continue;
            
            auto& shooter = *shooters[i];
            
            // Est-ce qu'une arme est équipée ?
            if (shooter.current_weapon_entity == (size_t)-1) continue;
            
            size_t weapon_id = shooter.current_weapon_entity;

            // L'entité arme est-elle valide ?
            if (weapon_id >= weapons.size() || !weapons[weapon_id]) continue;
            
            auto& weapon = *weapons[weapon_id];
            auto& shooter_pos = *positions[i];

            // --- DÉCISION DE TIR ---
            // Le PlayerShootingSystem a mis le timer à -1 pour signaler une action.
            if (shooter.fire_timer < 0) {
                
                // DEBUG: Afficher l'état avant de décider
                std::cout << "🎯 WeaponSystem: Fire triggered! is_charging=" << shooter.is_charging 
                          << " charge_timer=" << shooter.charge_timer 
                          << " charged_enabled=" << weapon.charged_shot_enabled 
                          << " archetype='" << weapon.charged_projectile_archetype << "'" << std::endl;
                
                // --- GESTION DU TIR CHARGÉ ---
                // C'est un cas spécial, on le traite en priorité.
                if (shooter.is_charging && weapon.charged_shot_enabled && shooter.charge_timer > 0.5f) {
                    std::cout << "🔵 WeaponSystem: Firing CHARGED SHOT (" << weapon.charged_projectile_archetype << ")" << std::endl;
                    gameEng.spawn_from_archetype(weapon.charged_projectile_archetype, _registerGroup, shooter_pos);
                    
                    // On a tiré, on reset tout et on arrête là pour cette entité.
                    shooter.fire_timer = shooter.fire_rate; // Appliquer le cooldown normal
                    shooter.is_charging = false;
                    shooter.charge_timer = 0.0f;
                    continue; 
                }

                // --- GESTION DU TIR NORMAL ---
                // On applique le cooldown immédiatement pour éviter les tirs en rafale non contrôlés.
                shooter.fire_timer = shooter.fire_rate;
                
                std::cout << "WeaponSystem: Firing '" << weapon.projectile_archetype << "' with pattern " << (int)weapon.pattern << std::endl;

                switch (weapon.pattern) {
                    case Component::Gameplay::FirePatternType::SIMPLE: {
                        std::cout << "  → Spawning bullet at position (" << shooter_pos.x << ", " << shooter_pos.y << ")" << std::endl;
                        Entity bullet = gameEng.spawn_from_archetype(weapon.projectile_archetype, _registerGroup, shooter_pos);
                        
                        // Debug: vérifier que le projectile a bien un sprite
                        if (bullet.has_component<Component::Graphics::Sprite>()) {
                            auto& sprite = bullet.get_component<Component::Graphics::Sprite>();
                            std::cout << "  → Bullet has sprite with texture_id=" << sprite.texture_id << " z_index=" << sprite.z_index << std::endl;
                        } else {
                            std::cout << "  → WARNING: Bullet has NO SPRITE!" << std::endl;
                        }
                        break;
                    }
                    case Component::Gameplay::FirePatternType::SPREAD: {
                        // On lit les instructions depuis le JSON de l'arme
                        int count = weapon.pattern_data.value("count", 3);
                        float total_angle_deg = weapon.pattern_data.value("angle", 30.0f);
                        float base_speed = 400.0f; // On pourrait aussi mettre ça dans le JSON
                        
                        // On s'assure de ne pas diviser par zéro si count < 2
                        float angle_step = (count > 1) ? (total_angle_deg / (count - 1)) : 0;
                        float start_angle = -total_angle_deg / 2.0f;

                        for (int j = 0; j < count; ++j) {
                            Entity bullet = gameEng.spawn_from_archetype(weapon.projectile_archetype, _registerGroup, shooter_pos);
                            
                            float current_angle_deg = start_angle + (j * angle_step);
                            float angle_rad = current_angle_deg * (M_PI / 180.0f);
                            
                            // On calcule la nouvelle vélocité en fonction de l'angle
                            auto& vel = bullet.get_component<Component::Core::Velocity>();
                            vel.dx = base_speed * cos(angle_rad);
                            vel.dy = base_speed * sin(angle_rad);
                        }
                        break;
                    }
                    case Component::Gameplay::FirePatternType::LASER: {
                        Component::Core::Position laser_pos(shooter_pos.x + 20, shooter_pos.y + 5);
                        gameEng.spawn_from_archetype(weapon.projectile_archetype, _registerGroup,laser_pos);
                        break;
                    }
                }
            }
        }
    }
};