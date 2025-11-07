/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** MenuScene.hpp
*/

#ifndef MENUSCENE_HPP
#define MENUSCENE_HPP

#include "../interfaces/IScene.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include <string>

class MenuScene : public IScene {
private:
    std::string group = "MenuScene";

public:
    void init(GameEngine& engine) override;
    void update(GameEngine& engine, float delta_time) override;
    void render(GameEngine& engine) override;
};

#endif // MENUSCENE_HPP