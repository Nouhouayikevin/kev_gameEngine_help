/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** SceneManager.cpp
*/

#include "../../Include/All/managers/SceneManager.hpp"

#include <iostream> 

/**
 * @brief Ajoute une nouvelle scène au gestionnaire.
 * 
 * Stocke la scène dans la map avec le nom fourni comme clé.
 * Si une scène avec le même nom existe déjà, elle sera remplacée.
 *
 * @param name Le nom unique pour identifier la scène.
 * @param scene Un shared_ptr vers l'instance de la scène à ajouter.
 */
void SceneManager::add_scene(const std::string& name, std::shared_ptr<IScene> scene) {
    if (scene) {
        _scenes_map[name] = scene;
    }
}

/**
 * @brief Change la scène active.
 *
 * Recherche la scène par son nom dans la map. Si elle est trouvée,
 * elle devient la scène courante et sa méthode `init()` est appelée
 * pour la préparer.
 *
 * @param name Le nom de la scène vers laquelle basculer.
 * @param context Le contexte du moteur, nécessaire pour l'initialisation de la scène.
 */
void SceneManager::switch_to_scene(const std::string& name, GameEngine& context) {
    auto it = _scenes_map.find(name);
    
    if (it != _scenes_map.end()) {
        _current_scene = it->second;
        // Initialise la scène avec le contexte du moteur avant de l'utiliser.
        _current_scene->init(context); 
    } else {
        // Gère le cas où la scène demandée n'existe pas.
        std::cerr << "Erreur : Tentative de basculer vers une scène inconnue : " << name << std::endl;
        _current_scene = nullptr;
    }
}

/**
 * @brief Met à jour la logique de la scène courante.
 *
 * Si une scène est actuellement active, sa méthode `update()` est appelée,
 * en lui passant le contexte et le temps écoulé depuis la dernière frame (delta_time).
 *
 * @param context Le contexte du moteur.
 * @param delta_time Le temps écoulé, pour des mises à jour indépendantes de la framerate.
 */
void SceneManager::update_current(GameEngine& context, float delta_time) {
    // S'assure qu'il y a bien une scène active avant d'essayer de la mettre à jour.
    if (_current_scene) {
        _current_scene->update(context, delta_time);
    }
}

/**
 * @brief Gère le rendu graphique de la scène courante.
 *
 * Si une scène est actuellement active, sa méthode `render()` est appelée
 * pour dessiner ses éléments à l'écran, en lui passant le contexte.
 *
 * @param context Le contexte du moteur, contenant notamment la fenêtre de rendu.
 */
void SceneManager::render_current(GameEngine& context) {
    // S'assure qu'il y a bien une scène active avant d'essayer de la dessiner.
    if (_current_scene) {
        _current_scene->render(context);
    }
}

std::shared_ptr<IScene> SceneManager::get_current_scene() const {
    return _current_scene;
}