/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** GameEngine.hpp
*/

#pragma once

#include <memory>
#include "../../All/interfaces/IWindow.hpp"
#include "../../All/interfaces/IInputManager.hpp"
#include "../../All/interfaces/INetwork.hpp"
#include "../managers/ResourceManager.hpp"
#include "../managers/LevelManager.hpp"
#include "../managers/ScriptingManager.hpp"
#include "../Registry.hpp"
#include "../managers/ReplayManager.hpp"
#include "Clock.hpp"
#include <iostream>
#include "json.hpp"
#include "../../All/Component.hpp"
#include <optional> 
#include <fstream> 
#include <iostream>

using json = nlohmann::json;

class SceneManager;

class GameEngine {
    public:
    void load_archetypes(const std::string& path) {
        std::cout << "GameEngine: Loading archetypes from '" << path << "'..." << std::endl;
        std::ifstream f(path);
        if (!f.is_open()) {
            throw std::runtime_error("GameEngine: Failed to open archetypes file: " + path);
        }
        try {
            _archetypes = json::parse(f);
            std::cout << "GameEngine: Archetypes loaded successfully." << std::endl;
        } catch (json::parse_error& e) {
            throw std::runtime_error("GameEngine: Failed to parse archetypes JSON: " + std::string(e.what()));
        }
    }

    Entity spawn_from_archetype(const std::string& archetype_path,std::string registerGroup = "default", std::optional<Component::Core::Position> override_pos = std::nullopt) {
        Registry& registry = getRegistry(registerGroup);
        
        const json& archetype_def = find_archetype_def(archetype_path);
        Entity e = registry.spawn_entity();

        if (archetype_def.contains("components")) {
            for (auto& [comp_name, comp_data] : archetype_def["components"].items()) {
                if (comp_name == "Position" && override_pos.has_value()) {
                    Component::Core::Position &pos = override_pos.value();
                    registry.add_component(e, std::move(pos));
                    continue;
                }
                
                if (_component_factories.count(comp_name)) {
                    // ON PASSE LE REGISTRE À L'USINE ICI
                    _component_factories.at(comp_name)(registry, e, comp_data);
                } else {
                    std::cerr << "Warning: Factory for component '" << comp_name << "' not found." << std::endl;
                }
            }
        }
        
        if (!e.has_component<Component::Core::Position>()) {
            registry.add_component<Component::Core::Position>(e, {0.0f, 0.0f});
        }
        return e;
    }

private:
    void register_component_factories(std::string registerGroup = "default") {
        std::cout << "Registering component factories for register of the group '"<< registerGroup <<"'..." << std::endl;
        // --- HELPER MAPS FOR ENUMS (pour convertir string -> enum) ---
        const std::unordered_map<std::string, Component::Gameplay::AIMovementType> ai_type_map = {
            {"STRAIGHT", Component::Gameplay::AIMovementType::STRAIGHT},
            {"VERTICAL_OSC", Component::Gameplay::AIMovementType::VERTICAL_OSC},
            {"CIRCULAR", Component::Gameplay::AIMovementType::CIRCULAR},
            {"LEMNISCATE", Component::Gameplay::AIMovementType::LEMNISCATE},
            {"SINUSOIDAL", Component::Gameplay::AIMovementType::SINUSOIDAL}
        };
        const std::unordered_map<std::string, Component::Gameplay::OwnerType> owner_type_map = {
            {"PLAYER", Component::Gameplay::OwnerType::PLAYER},
            {"ENEMY", Component::Gameplay::OwnerType::ENEMY}
        };

        const std::unordered_map<std::string, Component::Gameplay::FirePatternType> pattern_type_map = {
            {"SIMPLE", Component::Gameplay::FirePatternType::SIMPLE},
            {"SPREAD", Component::Gameplay::FirePatternType::SPREAD},
            {"LASER", Component::Gameplay::FirePatternType::LASER}
        };
        const std::unordered_map<std::string, Component::Gameplay::PowerUpType> powerup_type_map = {
            {"CHANGE_WEAPON", Component::Gameplay::PowerUpType::CHANGE_WEAPON},
            {"HEAL", Component::Gameplay::PowerUpType::HEAL},
            {"ADD_SCORE", Component::Gameplay::PowerUpType::ADD_SCORE},
            {"SHIELD", Component::Gameplay::PowerUpType::SHIELD},
            {"SPEED_BOOST", Component::Gameplay::PowerUpType::SPEED_BOOST}
        };

        // =========================================================================
        //                            --- CORE COMPONENTS ---
        // =========================================================================
        _component_factories["Position"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Core::Position>(Component::Core::Position(j.value("x", 0.0f), j.value("y", 0.0f)));
        };
        _component_factories["Velocity"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Core::Velocity>(Component::Core::Velocity(j.value("dx", 0.0f), j.value("dy", 0.0f)));
        };
        _component_factories["Scale"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Core::Scale>(Component::Core::Scale(j.value("width", 1.0f), j.value("height", 1.0f)));
        };
        _component_factories["Rotation"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Core::Rotation>(Component::Core::Rotation(j.value("angle", 0.0f)));
        };
        _component_factories["Hitbox"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Core::Hitbox>(Component::Core::Hitbox(j.value("width", 0.0f), j.value("height", 0.0f)));
        };

        // =========================================================================
        //                         --- GAMEPLAY COMPONENTS ---
        // =========================================================================
        _component_factories["Health"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::Health>(Component::Gameplay::Health(j.value("hp", 1), j.value("max_hp", 1)));
        };
        _component_factories["Damage"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::Damage>(Component::Gameplay::Damage(j.value("amount", 0)));
        };
        _component_factories["Score"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::Score>(Component::Gameplay::Score(j.value("value", 0)));
        };
        _component_factories["PlayerTag"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::PlayerTag>(Component::Gameplay::PlayerTag(j.value("player_id", 0)));
        };
        _component_factories["Controllable"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::Controllable>(Component::Gameplay::Controllable());
        };
        _component_factories["EnemyTag"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::EnemyTag>(Component::Gameplay::EnemyTag(j.value("type", "")));
        };
        _component_factories["MissileTag"] = [this, owner_type_map](Registry& registry, Entity e, const json& j){
            std::string owner_str = j.value("owner", "PLAYER");
            e.add_component<Component::Gameplay::MissileTag>(Component::Gameplay::MissileTag(owner_type_map.at(owner_str), 0)); // owner_id est dynamique, on met 0 par défaut
        };
        _component_factories["ParallaxLayer"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::ParallaxLayer>(Component::Gameplay::ParallaxLayer(j.value("scroll_speed", 0.0f)));
        };
        _component_factories["AI"] = [this, ai_type_map](Registry& registry, Entity e, const json& j) {
            std::string type_str = j.value("type", "STRAIGHT");
            e.add_component<Component::Gameplay::AI>(Component::Gameplay::AI(ai_type_map.at(type_str), 0.0f)); // origin_y est dynamique
            auto& ai = e.get_component<Component::Gameplay::AI>();
            ai.amplitude = j.value("amplitude", 50.0f);
            ai.frequency = j.value("frequency", 2.0f);
        };
        _component_factories["Shooter"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::Shooter>(Component::Gameplay::Shooter());
            auto& shooter = e.get_component<Component::Gameplay::Shooter>();
            shooter.fire_rate = j.value("fire_rate", 0.5f);
            shooter.charge_duration = j.value("charge_duration", 1.5f);
        };

        _component_factories["Force"] = [this](Registry& registry, Entity e, const json& j) {
            auto& opt_force = registry.add_component<Component::Gameplay::Force>(e, {});
            if (opt_force) {
                opt_force->auto_fire = j.value("auto_fire", true);
                opt_force->fire_rate = j.value("fire_rate", 0.15f);
                opt_force->launch_speed = j.value("launch_speed", 600.0f);
            }
        };

        _component_factories["ForceTag"] = [this](Registry& registry, Entity e, const json& j) {
            registry.add_component<Component::Gameplay::ForceTag>(e, {});
        };

        _component_factories["Serializable"] = [this](Registry& registry, Entity e, const json& j) {
            registry.add_component<Component::Core::Serializable>(e, {});
        };

        _component_factories["Weapon"] = [this, pattern_type_map](Registry& registry, Entity e, const json& j) {
            std::string pattern_str = j.value("pattern", "SIMPLE");
            
            auto& opt_weapon = registry.add_component<Component::Gameplay::Weapon>(e, {});
            auto& weapon = *opt_weapon;
            weapon.pattern = pattern_type_map.at(pattern_str);
            weapon.projectile_archetype = j.value("projectile_archetype", "");
            weapon.charged_shot_enabled = j.value("charged_shot_enabled", false);
            weapon.charged_projectile_archetype = j.value("charged_projectile_archetype", "");

            if (j.contains("pattern_data")) {
                weapon.pattern_data = j["pattern_data"];
            }
        };

        _component_factories["PowerUp"] = [this, powerup_type_map](Registry& registry, Entity e, const json& j) {
            std::string type_str = j.value("type", "CHANGE_WEAPON");

            auto& opt_powerup = registry.add_component<Component::Gameplay::PowerUp>(e, {});
            auto& powerup = *opt_powerup; 
            powerup.type = powerup_type_map.at(type_str);
            if (j.contains("data")) {
                powerup.data = j["data"];
            }
        };

        _component_factories["TimedLifespan"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Gameplay::TimedLifespan>(
                Component::Gameplay::TimedLifespan(j.value("time", 1.0f))
            );
        };

        _component_factories["Script"] = [this](Registry& registry, Entity e, const json& j) {
            std::string script_path = j.value("script_path", "");
            e.add_component<Component::Gameplay::Script>(script_path);
        };

        // =========================================================================
        //                         --- GRAPHICS COMPONENTS ---
        // =========================================================================
        _component_factories["Sprite"] = [this](Registry& registry, Entity e, const json& j) {
            // --- LA CORRECTION EST ICI ---
            // On dit à notre ouvrier de chercher "texture_path", pas "texture".
            std::string texture_path = j.value("texture_path", "");

            // --- ON AJOUTE UNE SÉCURITÉ (BONUS) ---
            // Si le chemin est vide, on lève une erreur claire au lieu de crasher plus loin.
            if (texture_path.empty()) {
                throw std::runtime_error("Sprite component in archetype is missing a valid 'texture_path'.");
            }

            // Le reste du code est maintenant en sécurité.
            try {
                size_t texture_id = _resource_module.load_texture(texture_path);
                
                e.add_component<Component::Graphics::Sprite>(
                    Component::Graphics::Sprite(
                        texture_id,
                        j.value("frame_width", 0),
                        j.value("frame_height", 0),
                        j.value("z_index", 0),
                        j.value("rect_x", 0),
                        j.value("rect_y", 0)
                    )
                );
            } catch (const std::exception& load_error) {
                // Si le fichier n'existe pas, on donne une erreur encore plus précise.
                throw std::runtime_error("Failed to load texture '" + texture_path + "': " + load_error.what());
            }
        };
        _component_factories["Animation"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Graphics::Animation>(Component::Graphics::Animation(
                j.value("start_frame", 0),
                j.value("num_frames", 1),
                j.value("duration", 0.1f),
                j.value("loops", true)
            ));
        };
        _component_factories["RectangleShape"] = [this](Registry& registry, Entity e, const json& j) {
            auto color_arr = j.value("color", json::array({128, 128, 128, 255}));
            e.add_component<Component::Graphics::RectangleShape>(Component::Graphics::RectangleShape(
                Component::Core::Color(color_arr[0], color_arr[1], color_arr[2], color_arr[3])
            ));
        };
        _component_factories["CircleShape"] = [this](Registry& registry, Entity e, const json& j) {
            auto color_arr = j.value("color", json::array({128, 128, 128, 255}));
            e.add_component<Component::Graphics::CircleShape>(Component::Graphics::CircleShape(
                j.value("radius", 10.0f),
                Component::Core::Color(color_arr[0], color_arr[1], color_arr[2], color_arr[3])
            ));
        };
        _component_factories["UIText"] = [this](Registry& registry, Entity e, const json& j) {
            std::string font_path = j.value("font_path", "Assets/fonts/r-type.ttf");
            size_t font_id = _resource_module.load_font(font_path);
            auto color_arr = j.value("color", json::array({255, 255, 255, 255}));
            e.add_component<Component::Graphics::UIText>(Component::Graphics::UIText(
                j.value("content", ""),
                font_id,
                j.value("character_size", 16),
                Component::Core::Color(color_arr[0], color_arr[1], color_arr[2], color_arr[3])
            ));
        };
        _component_factories["ButtonStateComponent"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Graphics::ButtonStateComponent>(Component::Graphics::ButtonStateComponent());
        };
        _component_factories["Clickable"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Graphics::Clickable>(Component::Graphics::Clickable());
        };
        _component_factories["Action"] = [this](Registry& registry, Entity e, const json& j) {
            e.add_component<Component::Graphics::Action>(Component::Graphics::Action());
            // Note: Les callbacks seront définis après la création de l'entité dans le code de la scène
        };

        // =========================================================================
        //                          --- AUDIO COMPONENTS ---
        // =========================================================================
        _component_factories["SoundEffect"] = [this](Registry& registry, Entity e, const json& j) {
            // Mêmes remarques que pour le Sprite
            std::string sound_path = j.value("sound", "");
            size_t sound_id = _resource_module.load_sound(sound_path); // Hypothèse d'accès
            e.add_component<Component::Audio::SoundEffect>(Component::Audio::SoundEffect(sound_id, false)); // 'to_play' sera géré par les systèmes
        };

        _component_factories["Music"] = [this](Registry& registry, Entity e, const json& j) {
            std::string music_path = j.value("music_path", "");
            
            if (music_path.empty()) {
                throw std::runtime_error("Music component in archetype is missing a valid 'music_path'.");
            }

            try {
                size_t music_id = _resource_module.load_music(music_path);
                
                e.add_component<Component::Audio::Music>(
                    Component::Audio::Music(
                        music_id,
                        j.value("volume", 50.0f),
                        j.value("loop", true)
                    )
                );

                auto& music_component = e.get_component<Component::Audio::Music>();
                music_component.should_play = j.value("auto_play", false);
                
            } catch (const std::exception& load_error) {
                throw std::runtime_error("Failed to load music '" + music_path + "': " + load_error.what());
            }
        };
        
        // NOTE: Les composants purement relationnels comme HealthBar ne sont pas listés ici,
        // car ils dépendent d'une autre entité (le 'owner') qui est créée dynamiquement.
        // Il vaut mieux les créer en C++ à côté de leur propriétaire.
        
        std::cout << "Registered " << _component_factories.size() << " component factories." << std::endl;
    }

    const json& find_archetype_def(const std::string& path) {
        std::stringstream ss(path);
        std::string segment;
        std::vector<std::string> segments;
        while(std::getline(ss, segment, '.')) {
        segments.push_back(segment);
        }

        json* current = &_archetypes;
        for (const auto& s : segments) {
            if (current->contains(s)) {
                current = &(*current)[s];
            } else {
                throw std::runtime_error("Archetype not found: segment '" + s + "' in path '" + path + "' does not exist.");
            }
        }
        return *current;
    }
    std::unique_ptr<IWindow> _window_module;
    std::unique_ptr<IInputManager> _input_module;
    std::unique_ptr<INetwork> _network_module;
    std::unique_ptr<SceneManager> _scene_manager;
    ResourceManager _resource_module;
    LevelManager _level_manager;
    std::unique_ptr<ScriptingManager> _scripting_manager;
    std::unordered_map<std::string, Registry> _registries;
    Clock _clock_module;
    std::unique_ptr<ReplayManager> _replay_manager;
    json _archetypes;
    std::unordered_map<std::string, std::function<void(Registry&, Entity, const json&)>> _component_factories;

public:
    GameEngine();
    ~GameEngine();
    void setWindow(std::unique_ptr<IWindow> window);
    void setInputManager(std::unique_ptr<IInputManager> input);
    void setNetwork(std::unique_ptr<INetwork> network);
    IWindow& getWindowManager();
    Registry& getRegistry(const std::string& group = "default") {
        if (_registries.find(group) == _registries.end()) {
            throw std::runtime_error("Registry group '" + group + "' does not exist.");
        }
        return _registries.at(group);
    }
    SceneManager& getSceneManager();
    IInputManager& getInputManager();
    ResourceManager& getResourceManager();
    void createRegistry(const std::string& group) {
        if (_registries.find(group) == _registries.end()) {
            _registries[group];
            std::cout << "GameEngine: Created new registry group '" << group << "'." << std::endl;
        }
    }
    ReplayManager& getReplayManager() {
        return *_replay_manager;
    }
    LevelManager& getLevelManager() {
        return _level_manager;
    }

    ScriptingManager& getScriptingManager() {
        return *_scripting_manager;
    }
    INetwork& getNetworkManager();
    Clock& getClock();
    void initialize(); 
    void shutdown();   
};