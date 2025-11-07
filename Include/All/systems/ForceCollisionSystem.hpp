/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** ForceCollisionSystem.hpp - La Force détruit les projectiles et blesse les ennemis
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class ForceCollisionSystem : public ISystem {
    std::string _registerGroup;
public:
    ForceCollisionSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        
        auto& force_tags = registry.get_components<Component::Gameplay::ForceTag>();
        auto& positions = registry.get_components<Component::Core::Position>();
        auto& hitboxes = registry.get_components<Component::Core::Hitbox>();
        auto& missiles = registry.get_components<Component::Gameplay::MissileTag>();
        auto& enemies = registry.get_components<Component::Gameplay::EnemyTag>();
        auto& healths = registry.get_components<Component::Gameplay::Health>();
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();
        auto& force_components = registry.get_components<Component::Gameplay::Force>();
        
        // Trouver toutes les Forces
        for (size_t force_id = 0; force_id < force_tags.size(); ++force_id) {
            if (!force_tags[force_id] || !positions[force_id] || !hitboxes[force_id]) continue;
            
            // Ignorer les Forces désactivées
            if (force_components[force_id] && !force_components[force_id]->active) continue;
            
            auto& force_pos = *positions[force_id];
            auto& force_hitbox = *hitboxes[force_id];
            
            // === 1. DÉTRUIRE LES PROJECTILES ENNEMIS ===
            for (size_t missile_id = 0; missile_id < missiles.size(); ++missile_id) {
                if (!missiles[missile_id] || !positions[missile_id] || !hitboxes[missile_id]) continue;
                if (missile_id < dead_tags.size() && dead_tags[missile_id]) continue;
                
                auto& missile = *missiles[missile_id];
                
                // On ne s'intéresse qu'aux projectiles ENNEMIS
                if (missile.owner != Component::Gameplay::OwnerType::ENEMY) continue;
                
                auto& missile_pos = *positions[missile_id];
                auto& missile_hitbox = *hitboxes[missile_id];
                
                // Détection de collision AABB
                if (force_pos.x < missile_pos.x + missile_hitbox.width &&
                    force_pos.x + force_hitbox.width > missile_pos.x &&
                    force_pos.y < missile_pos.y + missile_hitbox.height &&
                    force_pos.y + force_hitbox.height > missile_pos.y) {
                    
                    // La Force ABSORBE le projectile ennemi !
                    registry.kill_entity(Entity(missile_id, &registry));
                    std::cout << "Force: Absorbed enemy projectile!" << std::endl;
                }
            }
            
            // === 2. INFLIGER DES DÉGÂTS AUX ENNEMIS AU CONTACT ===
            for (size_t enemy_id = 0; enemy_id < enemies.size(); ++enemy_id) {
                if (!enemies[enemy_id] || !positions[enemy_id] || !hitboxes[enemy_id] || !healths[enemy_id]) continue;
                if (enemy_id < dead_tags.size() && dead_tags[enemy_id]) continue;
                
                auto& enemy_pos = *positions[enemy_id];
                auto& enemy_hitbox = *hitboxes[enemy_id];
                auto& enemy_health = *healths[enemy_id];
                
                // Détection de collision AABB
                if (force_pos.x < enemy_pos.x + enemy_hitbox.width &&
                    force_pos.x + force_hitbox.width > enemy_pos.x &&
                    force_pos.y < enemy_pos.y + enemy_hitbox.height &&
                    force_pos.y + force_hitbox.height > enemy_pos.y) {
                    
                    // La Force inflige des dégâts continus (10 dégâts par frame)
                    enemy_health.hp -= 10;
                    
                    if (enemy_health.hp <= 0) {
                        registry.kill_entity(Entity(enemy_id, &registry));
                        std::cout << "Force: Destroyed enemy by contact!" << std::endl;
                    }
                }
            }
        }
    }
};
