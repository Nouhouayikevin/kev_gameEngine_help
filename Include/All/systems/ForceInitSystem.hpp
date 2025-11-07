/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** ForceInitSystem.hpp - Initialise la Force au démarrage du jeu
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <iostream>

/**
 * @brief Système d'initialisation de la Force
 * 
 * Ce système s'exécute UNE SEULE FOIS pour trouver le joueur
 * et lier la Force à lui (en définissant owner_entity).
 * 
 * Logique:
 * - Cherche les entités avec ForceTag mais sans owner_entity défini
 * - Cherche l'entité avec PlayerTag
 * - Lie les deux en définissant force.owner_entity
 */
class ForceInitSystem : public ISystem {
    std::string _registerGroup;
    bool _initialized = false;
    
public:
    ForceInitSystem(std::string registerGroup = "default") 
        : _registerGroup(registerGroup) {}
    
    void run(GameEngine& gameEng, float dt) override {
        // Ne s'exécute qu'une seule fois
        if (_initialized) return;
        
        auto& registry = gameEng.getRegistry(_registerGroup);
        
        auto& forces = registry.get_components<Component::Gameplay::Force>();
        auto& force_tags = registry.get_components<Component::Gameplay::ForceTag>();
        auto& player_tags = registry.get_components<Component::Gameplay::PlayerTag>();
        
        std::cout << "🔵 ForceInitSystem: Checking for Forces and Player..." << std::endl;
        std::cout << "   - Forces size: " << forces.size() << std::endl;
        std::cout << "   - ForceTags size: " << force_tags.size() << std::endl;
        std::cout << "   - PlayerTags size: " << player_tags.size() << std::endl;
        
        // Trouver l'entité joueur
        size_t player_id = static_cast<size_t>(-1);
        for (size_t i = 0; i < player_tags.size(); ++i) {
            if (player_tags[i]) {
                player_id = i;
                std::cout << "   - Found player at ID: " << player_id << std::endl;
                break;
            }
        }
        
        // Si pas de joueur, réessayer au prochain frame
        if (player_id == static_cast<size_t>(-1)) {
            std::cout << "   - No player found yet, will retry next frame" << std::endl;
            return;
        }
        
        // Lier toutes les Forces au joueur
        int force_count = 0;
        for (size_t i = 0; i < forces.size(); ++i) {
            if (forces[i] && force_tags[i]) {
                std::cout << "   - Found Force at ID: " << i << " (owner_entity=" << forces[i]->owner_entity << ")" << std::endl;
                // Si owner_entity est invalide (valeur énorme = -1 en size_t, ou 0)
                if (forces[i]->owner_entity == 0 || forces[i]->owner_entity >= 100000) {
                    forces[i]->owner_entity = player_id;
                    std::cout << "🔵 Force #" << i << " linked to player #" << player_id << std::endl;
                    force_count++;
                }
            }
        }
        
        if (force_count > 0) {
            std::cout << "✅ ForceInitSystem: " << force_count << " Force(s) initialized" << std::endl;
            _initialized = true;
        }
    }
};
