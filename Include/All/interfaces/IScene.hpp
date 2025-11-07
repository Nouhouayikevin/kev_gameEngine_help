/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** IScene.hpp
*/

#ifndef INCLUDED_ISCENE_HPP
#define INCLUDED_ISCENE_HPP

#include <string>

class GameEngine;

class IScene {
public:
    virtual ~IScene() = default;
    virtual void init(GameEngine& context) = 0;
    virtual void update(GameEngine& context, float delta_time) = 0;
    virtual void render(GameEngine& context) = 0;
};

#endif