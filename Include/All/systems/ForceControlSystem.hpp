/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** ForceControlSystem.hpp - Gère les inputs du joueur pour contrôler la Force
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class ForceControlSystem : public ISystem {
    std::string _registerGroup;
    float _init_timer = 0.5f;  // Ignorer les inputs pendant 500ms au démarrage
    bool _toggle_key_was_down = false;  // État précédent de la touche pour debounce manuel
public:
    ForceControlSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        
        // Réduire le timer d'initialisation
        if (_init_timer > 0.0f) {
            _init_timer -= dt;
            return;  // Ignorer tous les inputs pendant l'initialisation
        }
        
        auto& forces = registry.get_components<Component::Gameplay::Force>();
        auto& force_tags = registry.get_components<Component::Gameplay::ForceTag>();
        auto& positions = registry.get_components<Component::Core::Position>();
        auto& velocities = registry.get_components<Component::Core::Velocity>();
        auto& controllables = registry.get_components<Component::Gameplay::Controllable>();
        auto& player_tags = registry.get_components<Component::Gameplay::PlayerTag>();
        
        // Détecter l'état actuel de la touche G
        bool g_key_down_now = gameEng.getInputManager().isKeyDown(KeyCode::G);
        bool g_just_pressed = g_key_down_now && !_toggle_key_was_down;
        
        // DEBUG ULTRA-VERBOSE
        static int frame_count = 0;
        frame_count++;
        if (g_key_down_now || g_just_pressed) {
            std::cout << "🔑 [Frame " << frame_count << "] G key state: down=" << g_key_down_now 
                      << " just_pressed=" << g_just_pressed 
                      << " _was_down=" << _toggle_key_was_down << std::endl;
        }
        
        // Trouver le joueur contrôlable
        size_t player_id = (size_t)-1;
        for (size_t i = 0; i < player_tags.size(); ++i) {
            if (player_tags[i] && controllables[i]) {
                player_id = i;
                break;
            }
        }
        
        if (player_id == (size_t)-1) return;
        
        // Trouver la Force du joueur
        for (size_t i = 0; i < forces.size(); ++i) {
            // DEBUG: Afficher TOUTES les Forces dans les arrays
            if (g_just_pressed) {
                std::cout << "  📋 Scanning Force slot #" << i << ": "
                          << "force=" << (forces[i] ? "✓" : "✗") << " "
                          << "tag=" << (force_tags[i] ? "✓" : "✗") << " "
                          << "pos=" << (positions[i] ? "✓" : "✗") << std::endl;
            }
            
            if (!forces[i] || !force_tags[i] || !positions[i]) continue;
            
            auto& force = *forces[i];
            
            // DEBUG: Toujours afficher l'état de chaque Force trouvée
            if (g_key_down_now || g_just_pressed) {
                std::cout << "  🔍 Force #" << i << ": active=" << force.active 
                          << " owner=" << force.owner_entity 
                          << " player=" << player_id << std::endl;
            }
            
            // Vérifier que cette Force appartient au joueur
            if (force.owner_entity != player_id) {
                if (g_key_down_now || g_just_pressed) {
                    std::cout << "  ❌ Force #" << i << " SKIPPED: owner mismatch" << std::endl;
                }
                continue;
            }
            
            // === CONTRÔLE : Touche G (Go/Toggle) - Activer/Désactiver ===
            if (g_just_pressed) {
                std::cout << "  ⚡ TOGGLE TRIGGERED for Force #" << i << " (was: " << force.active << ")" << std::endl;
                force.active = !force.active;
                std::cout << "  ⚡ TOGGLE RESULT: Force #" << i << " now: " << force.active << std::endl;
                
                if (force.active) {
                    std::cout << "🔵 Force: ACTIVATED" << std::endl;
                } else {
                    std::cout << "⚫ Force: DEACTIVATED" << std::endl;
                }
            }
            
            // Si la Force est désactivée, on ignore les autres contrôles
            if (!force.active) continue;
            
            // === CONTRÔLE : Touche T (Toggle) - Changer d'état d'attachement ===
            if (gameEng.getInputManager().isKeyPressed(KeyCode::T)) {
                switch (force.state) {
                    case Component::Gameplay::ForceState::DETACHED:
                        // Tab : Attacher à l'avant
                        force.state = Component::Gameplay::ForceState::ATTACHED_FRONT;
                        std::cout << "Force: Attached to FRONT" << std::endl;
                        break;
                        
                    case Component::Gameplay::ForceState::ATTACHED_FRONT:
                        // Tab : Attacher à l'arrière
                        force.state = Component::Gameplay::ForceState::ATTACHED_BACK;
                        std::cout << "Force: Attached to BACK" << std::endl;
                        break;
                        
                    case Component::Gameplay::ForceState::ATTACHED_BACK:
                        // Tab : Détacher
                        force.state = Component::Gameplay::ForceState::DETACHED;
                        std::cout << "Force: DETACHED" << std::endl;
                        break;
                        
                    case Component::Gameplay::ForceState::LAUNCHED:
                        // Pas de changement pendant le lancement
                        break;
                }
            }
            
            // === CONTRÔLE : Touche L (Launch) pour LANCER ===
            if (gameEng.getInputManager().isKeyPressed(KeyCode::L)) {
                if (force.state == Component::Gameplay::ForceState::ATTACHED_FRONT ||
                    force.state == Component::Gameplay::ForceState::ATTACHED_BACK) {
                    
                    // Lancer la Force vers la droite
                    force.state = Component::Gameplay::ForceState::LAUNCHED;
                    force.launch_duration = 2.0f;  // Vole pendant 2 secondes
                    
                    if (velocities[i]) {
                        velocities[i]->dx = force.launch_speed;
                        velocities[i]->dy = 0;
                    }
                    
                    std::cout << "Force: LAUNCHED!" << std::endl;
                }
            }
        }
        
        // Mettre à jour l'état de la touche À LA FIN (une seule fois par run())
        if (g_just_pressed) {
            _toggle_key_was_down = true;
        } else if (!g_key_down_now) {
            _toggle_key_was_down = false;
        }
    }
};
