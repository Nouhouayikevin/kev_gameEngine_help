/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** MusicSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../interfaces/IMusic.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

/**
 * @brief Système de gestion de la musique
 * 
 * Ce système gère la lecture, l'arrêt et le volume des musiques
 * basé sur le composant Component::Audio::Music.
 * Il utilise le ResourceManager pour accéder aux musiques chargées.
 */
class MusicSystem : public ISystem {
    std::string _registerGroup;
public:
    MusicSystem(std::string registerGroup = "default") : _registerGroup(registerGroup) {}
    
    void run(GameEngine& gameEng, float dt) override {
        auto& registry = gameEng.getRegistry(_registerGroup);
        auto& resourceManager = gameEng.getResourceManager();
        
        auto& musics = registry.get_components<Component::Audio::Music>();
        auto& dead_tags = registry.get_components<Component::Gameplay::Dead>();

        for (size_t i = 0; i < musics.size(); ++i) {
            // Ignorer les entités mortes ou sans composant
            if (!musics[i] || (i < dead_tags.size() && dead_tags[i])) {
                continue;
            }

            auto& music_component = *musics[i];
            
            // Récupérer l'instance IMusic depuis le ResourceManager
            try {
                IMusic& music = resourceManager.get_music(music_component.music_id);

                // Gérer la commande d'arrêt
                if (music_component.should_stop) {
                    music.stop();
                    music_component.is_playing = false;
                    music_component.should_stop = false;
                    continue;
                }

                // Gérer la commande de lecture
                if (music_component.should_play && !music_component.is_playing) {
                    music.setVolume(music_component.volume);
                    music.setLoop(music_component.loop);
                    music.play();
                    music_component.is_playing = true;
                    music_component.should_play = false;
                }

                // Synchroniser l'état de lecture
                music_component.is_playing = music.isPlaying();

                // Synchroniser le volume si changé
                if (music.getVolume() != music_component.volume) {
                    music.setVolume(music_component.volume);
                }

            } catch (const std::exception& e) {
                std::cerr << "MusicSystem: Error accessing music ID " 
                          << music_component.music_id << ": " << e.what() << std::endl;
            }
        }
    }
};
