/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** ForceSystem.hpp - Gère la mécanique iconique de la Force de R-Type
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>
#include <cmath>

class ForceSystem : public ISystem {
    std::string _registerGroup;
public:
    ForceSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        
        auto& forces = registry.get_components<Component::Gameplay::Force>();
        auto& positions = registry.get_components<Component::Core::Position>();
        auto& velocities = registry.get_components<Component::Core::Velocity>();
        auto& shooters = registry.get_components<Component::Gameplay::Shooter>();
        auto& weapons = registry.get_components<Component::Gameplay::Weapon>();
        
        for (size_t i = 0; i < forces.size(); ++i) {
            if (!forces[i] || !positions[i]) continue;
            
            auto& force = *forces[i];
            auto& force_pos = *positions[i];
            
            // === GESTION DE LA VISIBILITÉ ===
            auto& circles = registry.get_components<Component::Graphics::CircleShape>();
            
            if (force.active) {
                // Force ACTIVE → Doit être VISIBLE
                // Si pas de CircleShape, on le crée
                if (i < circles.size() && !circles[i]) {
                    registry.add_component<Component::Graphics::CircleShape>(
                        Entity(i, &registry),
                        Component::Graphics::CircleShape(16.0f, {255, 100, 0, 255})  // Orange, radius 16
                    );
                }
                // Sinon le CircleShape existe déjà, on ne fait rien (il reste visible)
            } else {
                // Force INACTIVE → Doit être INVISIBLE
                // Si un CircleShape existe, on le retire
                if (i < circles.size() && circles[i]) {
                    registry.remove_component<Component::Graphics::CircleShape>(Entity(i, &registry));
                }
                continue;  // Ne pas traiter le reste (mouvement, tir, etc.)
            }
            
            // Vérifier que le propriétaire existe toujours
            if (force.owner_entity >= positions.size() || !positions[force.owner_entity]) {
                continue;
            }
            
            auto& owner_pos = *positions[force.owner_entity];
            
            // === GESTION DE L'ÉTAT ===
            switch (force.state) {
                case Component::Gameplay::ForceState::ATTACHED_FRONT: {
                    // Position à l'avant du vaisseau (décalage positif en X)
                    force_pos.x = owner_pos.x + 40.0f + force.offset_x;
                    force_pos.y = owner_pos.y + force.offset_y;
                    
                    // Annuler la vélocité
                    if (velocities[i]) {
                        velocities[i]->dx = 0;
                        velocities[i]->dy = 0;
                    }
                    break;
                }
                
                case Component::Gameplay::ForceState::ATTACHED_BACK: {
                    // Position à l'arrière du vaisseau (décalage négatif en X)
                    force_pos.x = owner_pos.x - 40.0f + force.offset_x;
                    force_pos.y = owner_pos.y + force.offset_y;
                    
                    // Annuler la vélocité
                    if (velocities[i]) {
                        velocities[i]->dx = 0;
                        velocities[i]->dy = 0;
                    }
                    break;
                }
                
                case Component::Gameplay::ForceState::DETACHED: {
                    // La Force flotte librement, suit le joueur doucement
                    float dx = owner_pos.x - force_pos.x;
                    float dy = owner_pos.y - force_pos.y;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    
                    // Si trop loin du joueur, se rapproche doucement
                    if (distance > 150.0f && velocities[i]) {
                        float speed = 100.0f;
                        velocities[i]->dx = (dx / distance) * speed;
                        velocities[i]->dy = (dy / distance) * speed;
                    } else if (velocities[i]) {
                        velocities[i]->dx = 0;
                        velocities[i]->dy = 0;
                    }
                    break;
                }
                
                case Component::Gameplay::ForceState::LAUNCHED: {
                    // La Force vole dans une direction pendant un temps limité
                    force.launch_duration -= dt;
                    
                    if (force.launch_duration <= 0) {
                        // Fin du lancement, repasse en mode DETACHED
                        force.state = Component::Gameplay::ForceState::DETACHED;
                        if (velocities[i]) {
                            velocities[i]->dx = 0;
                            velocities[i]->dy = 0;
                        }
                    }
                    // La vélocité reste celle définie lors du lancement
                    break;
                }
            }
            
            // === TIR AUTOMATIQUE ===
            if (force.auto_fire) {
                force.fire_timer -= dt;
                if (force.fire_timer <= 0) {
                    force.fire_timer = force.fire_rate;
                    
                    // Déterminer quel projectile tirer : celui de l'arme du joueur
                    std::string projectile_archetype = "projectiles.force_laser";  // Par défaut
                    
                    // Récupérer l'arme du joueur
                    if (force.owner_entity < shooters.size() && shooters[force.owner_entity]) {
                        auto& player_shooter = *shooters[force.owner_entity];
                        
                        // Récupérer l'arme actuelle du joueur
                        if (player_shooter.current_weapon_entity != static_cast<size_t>(-1) &&
                            player_shooter.current_weapon_entity < weapons.size() &&
                            weapons[player_shooter.current_weapon_entity]) {
                            
                            auto& player_weapon = *weapons[player_shooter.current_weapon_entity];
                            
                            // Utiliser le projectile normal de l'arme du joueur
                            if (!player_weapon.projectile_archetype.empty()) {
                                projectile_archetype = player_weapon.projectile_archetype;
                            }
                        }
                    }
                    
                    // Tirer le projectile depuis la position de la Force
                    Component::Core::Position shoot_pos(force_pos.x, force_pos.y);
                    Entity projectile = gameEng.spawn_from_archetype(projectile_archetype, _registerGroup, shoot_pos);
                    
                    // Si la Force est attachée DERRIÈRE, inverser la direction du projectile
                    if (force.state == Component::Gameplay::ForceState::ATTACHED_BACK) {
                        auto& projectile_velocities = registry.get_components<Component::Core::Velocity>();
                        size_t proj_id = static_cast<size_t>(projectile);
                        
                        if (proj_id < projectile_velocities.size() && projectile_velocities[proj_id]) {
                            // Inverser la direction horizontale
                            projectile_velocities[proj_id]->dx = -projectile_velocities[proj_id]->dx;
                            
                            std::cout << "Force (BACK): Firing backwards with velocity dx=" 
                                      << projectile_velocities[proj_id]->dx << std::endl;
                        }
                    }
                }
            }
        }
    }
};
