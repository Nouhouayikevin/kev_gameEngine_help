/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** ScriptSystem.hpp
*/

#pragma once
#include "../interfaces/ISystem.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include "../../GameEngine_Include/core/LuaEntity.hpp"
#include <string>
class ScriptSystem : public ISystem {
public:
    ScriptSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {

        auto& registry = gameEng.getRegistry(_registerGroup);
        auto& lua_state = gameEng.getScriptingManager().getLuaState();
        auto& scripts = registry.get_components<Component::Gameplay::Script>();

        for (size_t i = 0; i < scripts.size(); ++i) {
            if (!scripts[i]) continue;
            
            auto& script = *scripts[i];

            LuaEntity self(gameEng, i);

            try {
                if (!script.is_initialized) {
                    lua_state.script_file("Assets/scripts/" + script.script_path);

                    sol::function init_func = lua_state["init"];
                    if (init_func.valid()) {
                        init_func(self);
                    }
                    script.is_initialized = true;
                }

                sol::function update_func = lua_state["on_update"];
                if (update_func.valid()) {
                    update_func(self, dt);
                }
            } catch (const sol::error& e) {
                std::cerr << "LUA ERROR in " << script.script_path << ": " << e.what() << std::endl;
                scripts[i].reset();
            }
        }
    };
private:
    std::string _registerGroup;
};