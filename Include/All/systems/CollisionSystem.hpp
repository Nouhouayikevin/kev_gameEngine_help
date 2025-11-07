/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** CollisionSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include "../../GameEngine_Include/core/LuaEntity.hpp"
#include <string>
#include <iostream>

class CollisionSystem : public ISystem {
    std::string _registerGroup;

public:
    CollisionSystem(std::string registerGroup = "default") : _registerGroup(registerGroup) {}

    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        
        auto& positions = registry.get_components<Component::Core::Position>();
        auto& hitboxes = registry.get_components<Component::Core::Hitbox>();
        auto& scales = registry.get_components<Component::Core::Scale>();
        auto& healths = registry.get_components<Component::Gameplay::Health>();
        auto& damages = registry.get_components<Component::Gameplay::Damage>();
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

        auto& missile_tags = registry.get_components<Component::Gameplay::MissileTag>();
        auto& enemy_tags = registry.get_components<Component::Gameplay::EnemyTag>();
        auto& player_tags = registry.get_components<Component::Gameplay::PlayerTag>();
        auto& scripts = registry.get_components<Component::Gameplay::Script>();
        auto& scores = registry.get_components<Component::Gameplay::Score>();
        auto& boss_tags = registry.get_components<Component::Gameplay::BossTag>();


        // ====================================================================================
        // SECTION 1 : Missiles du Joueur contre Toutes les Cibles (Ennemis ET Boss)
        // ====================================================================================
        for (size_t m_idx = 0; m_idx < missile_tags.size(); ++m_idx) {
            if (!missile_tags[m_idx] || missile_tags[m_idx]->owner != Component::Gameplay::OwnerType::PLAYER ||
                !positions[m_idx] || !hitboxes[m_idx] || !damages[m_idx] || (m_idx < dead_tags.size() && dead_tags[m_idx])) {
                continue;
            }

            auto& missile_pos = *positions[m_idx];
            auto& missile_hitbox = *hitboxes[m_idx];
            auto& missile_damage = *damages[m_idx];

            for (size_t t_idx = 0; t_idx < positions.size(); ++t_idx) {

                if (!enemy_tags[t_idx] || !positions[t_idx] || !hitboxes[t_idx] || !healths[t_idx] || (t_idx < dead_tags.size() && dead_tags[t_idx])) {
                    continue;
                }

                auto& target_pos = *positions[t_idx];
                auto& target_hitbox = *hitboxes[t_idx];
                auto& target_health = *healths[t_idx];

                // --- CALCUL DE LA HITBOX EFFECTIVE (prenant en compte le Scale) ---
                float target_scale_w = 1.0f, target_scale_h = 1.0f;
                if (t_idx < scales.size() && scales[t_idx]) {
                    target_scale_w = scales[t_idx]->width;
                    target_scale_h = scales[t_idx]->height;
                }
                float effective_target_width = target_hitbox.width * target_scale_w;
                float effective_target_height = target_hitbox.height * target_scale_h;
                
                float effective_missile_width = missile_hitbox.width;
                float effective_missile_height = missile_hitbox.height;


                // --- Test de Collision ---
                bool collision = 
                    missile_pos.x < target_pos.x + effective_target_width &&
                    missile_pos.x + effective_missile_width > target_pos.x &&
                    missile_pos.y < target_pos.y + effective_target_height &&
                    missile_pos.y + effective_missile_height > target_pos.y;

                if (collision) {
                    int damage_amount = missile_damage.amount;
                    target_health.hp -= damage_amount;

                    registry.add_component(Entity(m_idx, &registry), Component::Gameplay::Dead{});

                    if (t_idx < scripts.size() && scripts[t_idx]) {
                        try {
                            auto& lua_state = gameEng.getScriptingManager().getLuaState();
                            sol::function on_damage_func = lua_state["on_damage"];
                            if (on_damage_func.valid()) {
                                LuaEntity self(gameEng, t_idx);
                                on_damage_func(self, damage_amount);
                            }
                        } catch (const sol::error& e) {
                            std::cerr << "LUA ERROR in on_damage: " << e.what() << std::endl;
                        }
                    }

                    if (target_health.hp <= 0) {
                        if (scores[t_idx]) {
                            for (size_t p_idx = 0; p_idx < player_tags.size(); ++p_idx) {
                                if (player_tags[p_idx] && scores[p_idx]) {
                                    scores[p_idx]->value += scores[t_idx]->value;
                                    break; 
                                }
                            }
                        }
                        registry.add_component(Entity(t_idx, &registry), Component::Gameplay::Dead{});
                    }
                    break;
                }
            }
        }
// ====================================================================================
        // SECTION 2 : Joueur contre Toutes les Cibles (Collision Kamikaze)
        // ====================================================================================
        for (size_t p_idx = 0; p_idx < player_tags.size(); ++p_idx) {
            // Est-ce un joueur valide ?
            if (!player_tags[p_idx] || !positions[p_idx] || !hitboxes[p_idx] || !healths[p_idx] || (p_idx < dead_tags.size() && dead_tags[p_idx])) {
                continue;
            }

            auto& player_pos = *positions[p_idx];
            auto& player_hitbox = *hitboxes[p_idx];
            auto& player_health = *healths[p_idx];

            // --- Calcul de la Hitbox Effective du Joueur ---
            float player_scale_w = 1.0f, player_scale_h = 1.0f;
            if (p_idx < scales.size() && scales[p_idx]) {
                player_scale_w = scales[p_idx]->width;
                player_scale_h = scales[p_idx]->height;
            }
            float effective_player_width = player_hitbox.width * player_scale_w;
            float effective_player_height = player_hitbox.height * player_scale_h;


            // On boucle sur TOUTES les entités potentiellement ciblables
            for (size_t t_idx = 0; t_idx < positions.size(); ++t_idx) {
                // Est-ce une cible valide (ennemi OU boss) qui peut infliger des dégâts de contact ?
                if (!enemy_tags[t_idx] || !positions[t_idx] || !hitboxes[t_idx] || !damages[t_idx] || (t_idx < dead_tags.size() && dead_tags[t_idx])) {
                    continue;
                }

                auto& target_pos = *positions[t_idx];
                auto& target_hitbox = *hitboxes[t_idx];
                auto& target_damage = *damages[t_idx];

                // --- Calcul de la Hitbox Effective de la Cible ---
                float target_scale_w = 1.0f, target_scale_h = 1.0f;
                if (t_idx < scales.size() && scales[t_idx]) {
                    target_scale_w = scales[t_idx]->width;
                    target_scale_h = scales[t_idx]->height;
                }
                float effective_target_width = target_hitbox.width * target_scale_w;
                float effective_target_height = target_hitbox.height * target_scale_h;
                
                // --- Test de Collision ---
                bool collision = 
                    player_pos.x < target_pos.x + effective_target_width &&
                    player_pos.x + effective_player_width > target_pos.x &&
                    player_pos.y < target_pos.y + effective_target_height &&
                    player_pos.y + effective_player_height > target_pos.y;

                if (collision) {
                    // 1. Le joueur prend des dégâts
                    player_health.hp -= target_damage.amount;
                    
                    // 2. La cible est détruite dans la collision (sauf si c'est un boss, car ça n'aurait pas de sens)
                    if (!(t_idx < boss_tags.size() && boss_tags[t_idx])) {
                        registry.add_component(Entity(t_idx, &registry), Component::Gameplay::Dead{});
                    }

                    // 3. Gérer la mort du joueur
                    if (player_health.hp <= 0) {
                        registry.add_component(Entity(p_idx, &registry), Component::Gameplay::Dead{});
                        // Si le joueur est mort, on arrête de tester les collisions pour lui
                        break; 
                    }
                }
            }
        }

        // =====================================================================
        // SECTION 3 : Missiles Ennemis contre Joueur
        // =====================================================================
        for (size_t m_idx = 0; m_idx < missile_tags.size(); ++m_idx) {
            if (!missile_tags[m_idx] || missile_tags[m_idx]->owner != Component::Gameplay::OwnerType::ENEMY) continue;
            if (m_idx < dead_tags.size() && dead_tags[m_idx]) continue;
            if (!positions[m_idx] || !hitboxes[m_idx] || !damages[m_idx]) continue;

            auto& missile_pos = *positions[m_idx];
            auto& missile_hitbox = *hitboxes[m_idx];
            auto& missile_damage = *damages[m_idx];

            for (size_t p_idx = 0; p_idx < player_tags.size(); ++p_idx) {
                if (p_idx < dead_tags.size() && dead_tags[p_idx]) continue;
                if (!player_tags[p_idx] || !positions[p_idx] || !hitboxes[p_idx] || !healths[p_idx]) continue;

                auto& player_pos = *positions[p_idx];
                auto& player_hitbox = *hitboxes[p_idx];
                auto& player_health = *healths[p_idx];

                // --- Logique de Collision ---
                bool collision = 
                    missile_pos.x < player_pos.x + player_hitbox.width &&
                    missile_pos.x + missile_hitbox.width > player_pos.x &&
                    missile_pos.y < player_pos.y + player_hitbox.height &&
                    missile_pos.y + missile_hitbox.height > player_pos.y;

                if (collision) {
                    player_health.hp -= missile_damage.amount;
                    registry.add_component(Entity(m_idx, &registry), Component::Gameplay::Dead{});

                    if (player_health.hp <= 0) {
                        registry.add_component(Entity(p_idx, &registry), Component::Gameplay::Dead{});
                    }
                    break;
                }
            }
        }
    }
};