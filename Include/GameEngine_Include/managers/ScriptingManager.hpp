/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** ScriptingManager.hpp
*/

#ifndef SCRIPTINGMANAGER_HPP_
#define SCRIPTINGMANAGER_HPP_

#include <string>
#include <sol/sol.hpp>

class GameEngine;

class ScriptingManager {
    public:
        ScriptingManager(GameEngine& engine);
        sol::state& getLuaState();

    private:
        GameEngine& _engine;
        sol::state _lua;
};

#endif // SCRIPTINGMANAGER_HPP_