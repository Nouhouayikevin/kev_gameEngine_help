/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** LuaEntity.cpp
*/

#include "../../Include/GameEngine_Include/core/LuaEntity.hpp"
#include "../../Include/GameEngine_Include/core/GameEngine.hpp"
#include "../../Include/All/Component.hpp"

LuaEntity::LuaEntity(GameEngine& engine, size_t id) : _game_engine(engine), _entity_id(id) {}

sol::table LuaEntity::getPosition() const {
    auto& registry = _game_engine.getRegistry();
    auto& pos = registry.get_components<Component::Core::Position>()[_entity_id];
    
    sol::state_view lua(_game_engine.getScriptingManager().getLuaState());
    sol::table position_table = lua.create_table();
    if (pos) {
        position_table["x"] = pos->x;
        position_table["y"] = pos->y;
    }
    return position_table;
}

void LuaEntity::setPosition(float x, float y) {
    auto& registry = _game_engine.getRegistry();
    if (_entity_id < registry.get_components<Component::Core::Position>().size()) {
         registry.get_components<Component::Core::Position>()[_entity_id].value().x = x;
         registry.get_components<Component::Core::Position>()[_entity_id].value().y = y;
    }
}

sol::table LuaEntity::getVelocity() const {
    auto& registry = _game_engine.getRegistry();
    auto& vel = registry.get_components<Component::Core::Velocity>()[_entity_id];

    sol::state_view lua(_game_engine.getScriptingManager().getLuaState());
    sol::table velocity_table = lua.create_table();
    if (vel) {
        velocity_table["dx"] = vel->dx;
        velocity_table["dy"] = vel->dy;
    }
    return velocity_table;
}

void LuaEntity::setVelocity(float dx, float dy) {
    auto& registry = _game_engine.getRegistry();
     if (_entity_id < registry.get_components<Component::Core::Velocity>().size()) {
        registry.get_components<Component::Core::Velocity>()[_entity_id].value().dx = dx;
        registry.get_components<Component::Core::Velocity>()[_entity_id].value().dy = dy;
    }
}

int LuaEntity::getHealth() const {
    auto& registry = _game_engine.getRegistry();
    auto& health = registry.get_components<Component::Gameplay::Health>()[_entity_id];
    return health ? health->hp : 0;
}

std::size_t LuaEntity::getId() const {
    return _entity_id;
}