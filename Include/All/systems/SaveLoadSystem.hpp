#pragma once
#include "../interfaces/ISystem.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include "../../GameEngine_Include/core/Serializer.hpp"


class SaveLoadSystem : public ISystem {
public:
    SaveLoadSystem(std::string registerGroup = "default", std::string savePath = "savegame.sav")
    {
        _registerGroup = registerGroup;
        _savePath = savePath;
    }
    void run(GameEngine& gameEng, float dt) {
        auto& registry = gameEng.getRegistry(_registerGroup);

        // --- SAUVEGARDE ---
        if (gameEng.getInputManager().isKeyPressed(KeyCode::S)) {
            try {
                std::vector<char> save_data = Serializer::pack(registry);
                
                std::ofstream file(_savePath, std::ios::binary);
                file.write(save_data.data(), save_data.size());
                file.close();

                show_feedback_text(gameEng, "Game Saved!");
            } catch (const std::exception& e) {
                show_feedback_text(gameEng, "Save Failed!");
                std::cerr << "Save Error: " << e.what() << std::endl;
            }
        }

        // --- RESTAURATION ---
        if (gameEng.getInputManager().isKeyPressed(KeyCode::R)) {
            try {
                std::ifstream file(_savePath, std::ios::binary | std::ios::ate);
                if (!file.is_open()) throw std::runtime_error("Save file not found.");

                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                
                std::vector<char> buffer(size);
                if (!file.read(buffer.data(), size)) throw std::runtime_error("Could not read save file.");
                file.close();

                Serializer::unpack(buffer, registry);
                show_feedback_text(gameEng, "Game Loaded!");

            } catch (const std::exception& e) {
                show_feedback_text(gameEng, "Load Failed!");
                std::cerr << "Load Error: " << e.what() << std::endl;
            }
        }
    }

private:
    void show_feedback_text(GameEngine& gameEng, const std::string& message)
    {
        // Pour l'instant, on affiche juste dans la console
        // TODO: Créer une entité UI pour afficher le message dans le jeu
        std::cout << "SaveLoadSystem: " << message << std::endl;
    }
    
    std::string _registerGroup;
    std::string _savePath;
};