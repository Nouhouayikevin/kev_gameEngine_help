/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** AnimationSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class AnimationSystem : public ISystem {
    std::string _registerGroup;
public:
    AnimationSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        auto& animations = registry.get_components<Component::Graphics::Animation>();
        auto& sprites = registry.get_components<Component::Graphics::Sprite>();

        for (size_t i = 0; i < animations.size() && i < sprites.size(); i++) {
            auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

            if (i < dead_tags.size() && dead_tags[i]) continue;
            if (animations[i] && sprites[i]) {
                auto& anim = *animations[i];
                auto& sprite = *sprites[i];

                anim.timer += (dt);

                if (anim.timer >= anim.frame_duration) {
                    anim.timer -= anim.frame_duration;           
                    anim.current_frame++;      
                    if (anim.current_frame >= anim.num_frames) {
                        if (anim.loops) {
                            anim.current_frame = 0; 
                        } else {
                            anim.current_frame = anim.num_frames - 1; 
                            
                        }
                    }
                }

                int total_frame_index = anim.start_frame + anim.current_frame;
                // Calcul du rect_x en fonction de l'animation + offset initial
                sprite.source_rect.x = sprite.initial_rect_x + (total_frame_index * sprite.frame_width);
                // Le rect_y reste celui défini initialement
                sprite.source_rect.y = sprite.initial_rect_y;
                sprite.source_rect.width = sprite.frame_width;
                sprite.source_rect.height = sprite.frame_height;
            }
        }
    }
};