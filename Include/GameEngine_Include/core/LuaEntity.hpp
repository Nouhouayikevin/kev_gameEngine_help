/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** LuaEntity.hpp
*/

#ifndef LUAENTITY_HPP
#define LUAENTITY_HPP

#include <sol/sol.hpp>

class GameEngine;

class LuaEntity {
    public:
    LuaEntity(GameEngine& engine, std::size_t id);

    void setPosition(float x, float y);
    sol::table getPosition() const;

    void setVelocity(float dx, float dy);
    sol::table getVelocity() const;

    int getHealth() const;

    std::size_t getId() const;

    private:
    GameEngine& _game_engine;
    std::size_t _entity_id;
};

#endif // LUAENTITY_HPP