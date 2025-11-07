/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** OffScreenDeathSystem.hpp - Marque comme mortes les entités hors écran
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <iostream>

/**
 * @brief Système de destruction des entités hors écran
 * 
 * Ce système marque automatiquement les entités (sauf le joueur) comme mortes
 * lorsqu'elles sortent du champ visuel de la fenêtre.
 * 
 * Zone de mort :
 * - X < -200 (trop à gauche)
 * - X > WINDOW_WIDTH + 200 (trop à droite)
 * - Y < -200 (trop en haut)
 * - Y > WINDOW_HEIGHT + 200 (trop en bas)
 * 
 * Exemples d'entités affectées :
 * - Ennemis sortis de l'écran
 * - Projectiles perdus
 * - Power-ups non ramassés
 * 
 * Entités ignorées :
 * - PlayerTag (le joueur)
 * - ParallaxLayer (fond d'écran)
 */
class OffScreenDeathSystem : public ISystem {
    std::string _registerGroup;
    
    // Dimensions de la fenêtre (ajustables)
    const float WINDOW_WIDTH = 1920.0f;
    const float WINDOW_HEIGHT = 1080.0f;
    const float MARGIN = 200.0f;  // Marge pour laisser sortir un peu avant destruction
    
public:
    OffScreenDeathSystem(std::string registerGroup = "default") 
        : _registerGroup(registerGroup) {}
    
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        
        auto& positions = registry.get_components<Component::Core::Position>();
        auto& player_tags = registry.get_components<Component::Gameplay::PlayerTag>();
        auto& parallax_tags = registry.get_components<Component::Gameplay::ParallaxLayer>();
        auto& force_tags = registry.get_components<Component::Gameplay::ForceTag>();
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();
        
        for (size_t i = 0; i < positions.size(); ++i) {
            if (!positions[i]) continue;
            
            // Ignorer le joueur
            if (i < player_tags.size() && player_tags[i]) continue;
            
            // Ignorer les Forces (elles ont leur propre logique de désactivation)
            if (i < force_tags.size() && force_tags[i]) continue;
            
            // Ignorer les couches de parallax (fond d'écran)
            if (i < parallax_tags.size() && parallax_tags[i]) continue;
            
            // Ignorer les entités déjà mortes
            if (i < dead_tags.size() && dead_tags[i]) continue;
            
            auto& pos = *positions[i];
            
            // Vérifier si l'entité est hors écran
            bool off_screen = false;
            
            if (pos.x < -MARGIN) {
                off_screen = true;
                // std::cout << "OffScreen: Entity " << i << " too far left (x=" << pos.x << ")" << std::endl;
            }
            else if (pos.x > WINDOW_WIDTH + MARGIN) {
                off_screen = true;
                // std::cout << "OffScreen: Entity " << i << " too far right (x=" << pos.x << ")" << std::endl;
            }
            else if (pos.y < -MARGIN) {
                off_screen = true;
                // std::cout << "OffScreen: Entity " << i << " too far up (y=" << pos.y << ")" << std::endl;
            }
            else if (pos.y > WINDOW_HEIGHT + MARGIN) {
                off_screen = true;
                // std::cout << "OffScreen: Entity " << i << " too far down (y=" << pos.y << ")" << std::endl;
            }
            
            // Marquer comme morte si hors écran
            if (off_screen) {
                registry.add_component<Component::Gameplay::Dead>(Entity(i, &registry), {});
            }
        }
    }
};
