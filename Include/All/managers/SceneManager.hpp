/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** SceneManager.hpp
*/

#ifndef INCLUDED_SCENEMANAGER_HPP
#define INCLUDED_SCENEMANAGER_HPP

#include <string>
#include <memory>
#include <map>
#include "../interfaces/IScene.hpp"
#include "../../../Include/GameEngine_Include/core/GameEngine.hpp"

class SceneManager {
public:
    void add_scene(const std::string& name, std::shared_ptr<IScene> scene);
    void switch_to_scene(const std::string& name, GameEngine& context);
    void update_current(GameEngine& context, float delta_time);
    std::shared_ptr<IScene> get_current_scene() const;
    void render_current(GameEngine& context);

private:
    std::map<std::string, std::shared_ptr<IScene>> _scenes_map;
    std::shared_ptr<IScene> _current_scene = nullptr;
};

#endif