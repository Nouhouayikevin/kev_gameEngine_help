/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** HealthBarSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class HealthBarSystem : public ISystem {
std::string _registerGroup;
public:
    HealthBarSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);

        // On récupère les composants des entités "barre de vie"
        auto& healths = registry.get_components<Component::Gameplay::Health>();
        auto& health_bars = registry.get_components<Component::UI::HealthBar>();
        auto& positions = registry.get_components<Component::Core::Position>();
        auto& rect_shapes = registry.get_components<Component::Graphics::RectangleShape>();
        auto& hitboxes = registry.get_components<Component::Core::Hitbox>();

        // On a aussi besoin des composants des entités "propriétaires"
        auto& owner_healths = registry.get_components<Component::Gameplay::Health>();
        auto& owner_positions = registry.get_components<Component::Core::Position>();
        // *** LA LIGNE CLÉ : On a besoin de savoir si le propriétaire est mort ***
        auto& owner_dead_tags = registry.get_components<Component::Gameplay::Dead>();

        for (size_t i = 0; i < healths.size(); ++i) {
            if (healths[i] && (i >= health_bars.size() || !health_bars[i])) {
                // Cette entité a de la vie, mais pas de barre de vie ! On en crée une.
                Entity owner = Entity(i, &registry);
                bool is_player = owner.has_component<Component::Gameplay::PlayerTag>();

                // On demande au moteur de spawner une barre de vie depuis l'archétype
                Entity bar = gameEng.spawn_from_archetype("ui.health_bar", _registerGroup);

                // On ajoute le composant de lien manquant
                float offset_y = is_player ? -20.0f : -15.0f;
                bar.add_component<Component::UI::HealthBar>(Component::UI::HealthBar(owner, 0, offset_y));
            }
        }
        for (size_t i = 0; i < health_bars.size(); ++i) {
            // On ne traite que les entités qui sont des barres de vie valides
            if (!health_bars[i] || !positions[i] || !rect_shapes[i] || !hitboxes[i]) continue;

            auto& bar = *health_bars[i];
            size_t owner_id = bar.owner;

            // --- DÉBUT DE LA CORRECTION ---
            // On vérifie si le propriétaire est valide ET s'il est marqué comme mort.
            bool owner_is_dead = (owner_id < owner_dead_tags.size() && owner_dead_tags[owner_id]);
            bool owner_is_invalid = (owner_id >= owner_healths.size() || !owner_healths[owner_id]);

            if (owner_is_dead || owner_is_invalid) {
                // Si le propriétaire est mort ou n'existe plus, on marque la barre de vie pour qu'elle soit détruite.
                registry.add_component(Entity(i, &registry), Component::Gameplay::Dead{});
                continue; // On passe à la barre de vie suivante, inutile de la mettre à jour.
            }
            // --- FIN DE LA CORRECTION ---

            // Si on arrive ici, le propriétaire est vivant et valide. On continue comme avant.
            auto& pos = *positions[i];
            auto& shape = *rect_shapes[i];
            auto& hitbox = *hitboxes[i];
            
            auto& owner_health = *owner_healths[owner_id];
            auto& owner_pos = *owner_positions[owner_id];

            // Mettre à jour la position
            pos.x = owner_pos.x + bar.offset_x;
            pos.y = owner_pos.y + bar.offset_y;

            // Calculer le pourcentage de vie
            float health_percent = static_cast<float>(owner_health.hp) / static_cast<float>(owner_health.max_hp);
            health_percent = std::max(0.0f, std::min(1.0f, health_percent)); 

            // Mettre à jour la largeur de la barre (on utilise la hitbox de la barre elle-même pour la largeur max)
            const float max_bar_width = hitbox.width;
            // On modifie directement la hitbox qui est utilisée par le render system
            hitbox.width = max_bar_width * health_percent;

            // Mettre à jour la couleur
            if (health_percent > 0.66f) {
                shape.fill_color = {0, 255, 0, 220}; // Vert
            } else if (health_percent > 0.33f) {
                shape.fill_color = {255, 255, 0, 220}; // Jaune
            } else {
                shape.fill_color = {255, 0, 0, 220}; // Rouge
            }
        }
    }
};