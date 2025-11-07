/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** PlayerShootingSystem.hpp
*/



#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class PlayerShootingSystem : public ISystem {
    std::string _registerGroup;
public:
    PlayerShootingSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
            auto& registry = gameEng.getRegistry(_registerGroup);
        
        // On a besoin des shooters, et de quoi identifier que c'est bien un joueur contrôlable
        auto& shooters = registry.get_components<Component::Gameplay::Shooter>();
        auto& player_tags = registry.get_components<Component::Gameplay::PlayerTag>();
        auto& controllables = registry.get_components<Component::Gameplay::Controllable>();

        for (size_t i = 0; i < shooters.size(); ++i) {
            // On s'assure qu'on a affaire à un joueur actif
            if (!shooters[i] || !player_tags[i] || !controllables[i]) continue;
            
            auto& shooter = *shooters[i];
            
            // --- GESTION DE L'ARME INITIALE ---
            // Le joueur doit avoir une arme assignée dans son archétype
            if (shooter.current_weapon_entity == (size_t)-1) {
                std::cout << "PlayerShootingSystem: Player has no weapon. Forging default gun..." << std::endl;
                // Créer une arme par défaut
                Entity weapon_entity = gameEng.spawn_from_archetype("weapons.default_gun", _registerGroup);
                shooter.current_weapon_entity = static_cast<size_t>(weapon_entity);
                std::cout << "PlayerShootingSystem: Default weapon created with ID " << shooter.current_weapon_entity << std::endl;
            }

            // On met à jour le cooldown du tir normal
            // IMPORTANT : On s'arrête à 0.0001 pour éviter de déclencher le tir automatiquement
            if (shooter.fire_timer > 0.0001f) {
                shooter.fire_timer -= dt;
                if (shooter.fire_timer < 0.0001f) {
                    shooter.fire_timer = 0.0001f;
                }
            }

            // --- TRADUCTION DE L'INPUT EN ÉTAT ---
            // Priorité 1 : Relâchement du bouton (pour le tir chargé ou normal)
            if (gameEng.getInputManager().isKeyReleased(KeyCode::Space)) {
                if (shooter.is_charging && shooter.charge_timer >= 0.5f) {
                    // Tir chargé : on a maintenu assez longtemps (>= 0.5s)
                    std::cout << "PlayerShootingSystem: CHARGED SHOT released after " << shooter.charge_timer << "s" << std::endl;
                    shooter.fire_timer = -1.0f;  // Signal pour WeaponSystem
                    // NE PAS réinitialiser is_charging ici ! WeaponSystem en a besoin !
                } else if (shooter.is_charging && shooter.charge_timer < 0.5f) {
                    // Tir normal : relâché trop tôt (< 0.5s)
                    std::cout << "PlayerShootingSystem: Normal shot (released too early at " << shooter.charge_timer << "s)" << std::endl;
                    shooter.fire_timer = -1.0f;  // Tir normal
                    shooter.is_charging = false;
                    shooter.charge_timer = 0.0f;
                } else {
                    // Pas de tir en cours, juste un relâchement sans charge
                    shooter.is_charging = false;
                    shooter.charge_timer = 0.0f;
                }
                
            // Priorité 2 : Maintien du bouton (charge)
            } else if (gameEng.getInputManager().isKeyDown(KeyCode::Space)) {
                // Le joueur MAINTIENT le bouton : on charge
                if (!shooter.is_charging) {
                    // Première frame du maintien
                    shooter.is_charging = true;
                    shooter.charge_timer = 0.0f;
                    std::cout << "PlayerShootingSystem: Started charging..." << std::endl;
                } else {
                    // Frames suivantes : on continue de charger
                    shooter.charge_timer += dt;
                }
                
            } else {
                // Aucun input : reset de l'état de charge
                if (shooter.is_charging) {
                    std::cout << "PlayerShootingSystem: Charge cancelled" << std::endl;
                }
                shooter.is_charging = false;
                shooter.charge_timer = 0.0f;
            }
        }
    }
};