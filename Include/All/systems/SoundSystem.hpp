/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** SoundSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include "../interfaces/ISound.hpp"
#include <string>

class SoundSystem : public ISystem {
std::string _registerGroup;
public:
    SoundSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        auto& sounds = gameEng.getRegistry(_registerGroup).get_components<Component::Audio::SoundEffect>();
        for (auto& sound : sounds) {
            if (sound && sound->to_play) {
                gameEng.getResourceManager().get_sound(sound->sound_id).play();
                sound->to_play = false;
            }
        }
    }
};