/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** Component.hpp
*/


#include <string>
#include <functional>
#include <map>
#include "../GameEngine_Include/core/json.hpp"

using json = nlohmann::json;

#ifndef COMPONENTS_HPP_
    #define COMPONENTS_HPP_
        namespace Component {
            namespace Core {
                struct Position {
                    float x;
                    float y;
                    Position() : x(0), y(0) {}
                    Position(float new_x, float new_y) : x(new_x), y(new_y) {}
                };

                struct Velocity {
                    float dx;
                    float dy;
                    Velocity() : dx(0), dy(0) {}
                    Velocity(float new_dx, float new_dy) : dx(new_dx), dy(new_dy) {}
                };

                struct Scale {
                    float width;
                    float height;
                    Scale(): width(0), height(0) {}
                    Scale(float new_width, float new_height): width(new_width), height(new_height) {}
                };

                struct Rotation {
                    float angle;
                    Rotation() : angle(0) {}
                    Rotation(float new_angle) : angle(new_angle) {}
                };
                struct IntRect {
                    int x, y, width, height;
                    IntRect(): x(0), y(0), width(0), height(0) {}
                    IntRect(int x, int y, int width, int height): x(x), y(y), width(width), height(height) {}
                };
                struct Color {
                    unsigned char r, g, b, a; // 0-255
                    Color(unsigned char r=255, unsigned char g=255, unsigned char b=255, unsigned char a=255)
                        : r(r), g(g), b(b), a(a) {}
                };

                struct Serializable {};

                struct Hitbox {
                    float width;
                    float height;
                    Hitbox() : width(0), height(0) {}
                    Hitbox(float w, float h) : width(w), height(h) {}
                };

                enum class ButtonState {
                    IDLE, 
                    HOVER,
                    PRESSED
                };
            }

            namespace Gameplay {
                enum class AIMovementType {
                    STRAIGHT,
                    VERTICAL_OSC,
                    CIRCULAR,          // En cercle
                    LEMNISCATE,        // En signe infini (Lissajous)
                    SINUSOIDAL         // En vague
                };

                struct Health {
                    int hp;
                    int max_hp;
                    Health() : hp(100), max_hp(100) {}
                    Health(int new_hp, int new_max_hp = 100) : hp(new_hp), max_hp(new_max_hp) {}
                };
            
                struct Damage {
                    int amount;
                    Damage() : amount(0) {}
                    Damage(int new_amount) : amount(new_amount) {}
                };
            
                struct Score {
                    int value;
                    Score() : value(0) {}
                    Score(int new_value) : value(new_value) {}
                };
            
                struct PlayerTag {
                    size_t player_id;
                    PlayerTag() : player_id(0) {}
                    PlayerTag(size_t id) : player_id(id) {}
                };
            
                struct Controllable {
                };
            
                struct EnemyTag {
                    std::string type;
                    EnemyTag() : type("") {}
                    EnemyTag(const std::string& new_type) : type(new_type) {}
                };
            
                enum class OwnerType { PLAYER, ENEMY };

                struct MissileTag {
                    OwnerType owner;
                    size_t owner_id; // L'ID de l'entité qui a tiré

                    MissileTag(OwnerType own_type, size_t own_id) : owner(own_type), owner_id(own_id) {}
                };
            
                struct ParallaxLayer {
                    float scroll_speed;
                    ParallaxLayer() : scroll_speed(0) {}
                    ParallaxLayer(float speed) : scroll_speed(speed) {}
                };

                struct Dead {};
                struct BossTag {};
            
                struct TimedLifespan {
                    float remaining_time;
                    TimedLifespan(float time) : remaining_time(time) {}
                };

                struct AI {
                    AIMovementType type;

                    // Paramètres
                    float origin_y;         // Le "centre" vertical du mouvement
                    float amplitude = 50.0f;  // La "hauteur" du mouvement
                    float frequency = 2.0f;   // La "vitesse" du mouvement
                    float time_elapsed = 0.0f;// Un timer interne pour suivre la progression

                    AI() : type(AIMovementType::STRAIGHT), origin_y(0.0f) {}
                    AI(AIMovementType t, float start_y) : type(t), origin_y(start_y) {}
                };

                struct Shooter {
                    // L'ID de l'entité "arme" actuellement équipée
                    size_t current_weapon_entity = -1; 
                    
                    float fire_rate = 0.5f;   // Cadence de tir
                    float fire_timer = 0.0f;  // Cooldown

                    // État du tir chargé
                    bool is_charging = false;
                    float charge_timer = 0.0f;
                    float charge_duration = 1.5f;

                    // Paramètres pour l'IA des ennemis
                    bool can_charged_shot = false;    // Si l'ennemi peut faire des tirs chargés
                    float charge_probability = 0.1f;  // Probabilité de commencer un tir chargé (0.0-1.0)
                    bool is_ai_charging = false;      // Si l'ennemi est en train de charger

                    Shooter() {}
                    Shooter(float rate, float charge_dur = 1.5f, bool can_charge = false, float charge_prob = 0.1f)
                        : fire_rate(rate), charge_duration(charge_dur), 
                          can_charged_shot(can_charge), charge_probability(charge_prob) {}
                };

                // L'ARME : Quel est le comportement du tir ?
                // Ce composant vit sur une entité "logique" qui est l'arme elle-même.
                enum class FirePatternType {
                    SIMPLE,
                    SPREAD,
                    LASER
                };

                struct Weapon {
                    FirePatternType pattern;
                    std::string projectile_archetype; // Plan de la balle
                    
                    bool charged_shot_enabled = false;
                    std::string charged_projectile_archetype; // Plan du tir chargé

                    // Données flexibles pour les patterns (ex: angle du spread, durée du laser...)
                    json pattern_data;
                };

                // LE CADEAU : Que fait ce power-up ?
                // Ce composant vit sur l'entité que le joueur ramasse.
                enum class PowerUpType {
                    CHANGE_WEAPON,
                    HEAL,
                    ADD_SCORE,
                    SHIELD,
                    SPEED_BOOST
                };

                struct PowerUp {
                    PowerUpType type;
                    // Pour CHANGE_WEAPON, ceci est le plan de la nouvelle entité "arme" à forger.
                    json data; 
                    PowerUp() {}
                };

                struct Script {
                    std::string script_path;
                    bool is_initialized = false;

                    Script(const std::string& path) : script_path(path) {}
                };

                // === FORCE (R-Type iconic mechanic) ===
                
                enum class ForceState {
                    ATTACHED_FRONT,   // Attachée à l'avant du vaisseau
                    ATTACHED_BACK,    // Attachée à l'arrière du vaisseau
                    DETACHED,         // Flottante, contrôlable
                    LAUNCHED          // Lancée dans une direction
                };

                struct Force {
                    size_t owner_entity = (size_t)-1;  // L'entité du joueur propriétaire
                    ForceState state = ForceState::DETACHED;
                    
                    // Position relative quand attachée
                    float offset_x = 0.0f;
                    float offset_y = 0.0f;
                    
                    // Pour l'état LAUNCHED
                    float launch_speed = 500.0f;
                    float launch_duration = 0.0f;  // Temps restant en vol
                    
                    // Caractéristiques
                    bool active = true;            // Force activée/désactivée
                    bool auto_fire = true;         // Tire automatiquement
                    float fire_rate = 0.2f;
                    float fire_timer = 0.0f;
                    
                    Force() {}
                };

                struct ForceTag {};  // Tag pour identifier une entité Force

            }

            namespace Graphics {
                struct Sprite {
                    size_t texture_id = 0;
                    int z_index = 0;
                    Component::Core::IntRect source_rect; 
                    int frame_width = 0; 
                    int frame_height = 0;
                    int initial_rect_x = 0;  // Offset de base dans la spritesheet
                    int initial_rect_y = 0;  // Offset de base dans la spritesheet

                    Sprite(size_t tex_id, int f_width, int f_height, int z = 0, int rect_x = 0, int rect_y = 0)
                        : texture_id(tex_id), z_index(z), frame_width(f_width), frame_height(f_height),
                          initial_rect_x(rect_x), initial_rect_y(rect_y) {
                        source_rect = {rect_x, rect_y, frame_width, frame_height};
                    }

                    // Sprite(size_t tex_id, int z = 0) : texture_id(tex_id), z_index(z) {}
                    Sprite() :  texture_id(0), z_index(0), frame_width(0), frame_height(0),
                                initial_rect_x(0), initial_rect_y(0) {
                        source_rect = {0, 0, 0, 0};
                    }

                    
                };

                struct ButtonStateComponent {
                       Core:: ButtonState state = Core::ButtonState::IDLE;
                    };

                struct Clickable {};

                struct Action {
                    std::function<void()> on_click;
                    std::function<void()> on_hover_enter;
                    std::function<void()> on_hover_exit;

                    Action() {}
                    Action(std::function<void()> click_cb) : on_click(click_cb) {}
                };

                struct StatefulAppearance {
                    std::map<Core::ButtonState, Core::Color> color_map;
                };
            
                struct Animation {
                    int start_frame = 0;
                    int num_frames = 1;
                    float frame_duration = 0.0f;
                    bool loops = true;

                    int current_frame = 0;
                    float timer = 0.0f;

                    Animation(int start, int count, float duration, bool loop = true)
                        : start_frame(start), num_frames(count), frame_duration(duration), loops(loop),
                        current_frame(0), timer(0.0f) {}

                    Animation()
                        : start_frame(0), num_frames(0), frame_duration(0), loops(0),
                        current_frame(0), timer(0.0f) {}
                };
                
                struct RectangleShape {
                    Core::Color fill_color;
                    Core::Color outline_color;
                    float outline_thickness = 0.0f;
                    RectangleShape(Core::Color fill = {128, 128, 128, 255}) : fill_color(fill) {}
                };
                struct CircleShape {
                    float radius;
                    Core::Color fill_color;
                    Core::Color outline_color;
                    float outline_thickness = 0.0f;

                    CircleShape(float r = 0.0f, Core::Color fill = {128, 128, 128, 255}) : radius(r), fill_color(fill) {}
                };

                struct Text {
                    std::string content;
                    float font_size = 24.0f;
                    Core::Color color = {255, 255, 255, 255};
                    // Tu peux ajouter des choses comme le chemin de la police ici
                };

                struct UIText {
                    std::string content;
                    size_t font_id;
                    int character_size = 16;
                    Core::Color color;

                    UIText(const std::string& txt, size_t font, int size, Core::Color c = {255,255,255,255})
                        : content(txt), font_id(font), character_size(size), color(c) {}
                };
            }

            namespace Audio {
                struct SoundEffect {
                    size_t sound_id;
                    bool to_play = false;
                    SoundEffect() : sound_id(), to_play() {}
                    SoundEffect(size_t id, bool play) : sound_id(id), to_play(play) {}
                };

                struct Music {
                    size_t music_id;
                    float volume = 50.0f;        // Volume de 0 à 100
                    bool loop = true;            // La musique boucle-t-elle ?
                    bool is_playing = false;     // État actuel de lecture
                    bool should_play = false;    // Commande de lecture
                    bool should_stop = false;    // Commande d'arrêt
                    
                    Music() : music_id(0) {}
                    Music(size_t id, float vol = 50.0f, bool loop_music = true) 
                        : music_id(id), volume(vol), loop(loop_music) {}
                };
            }

            namespace UI {
                struct HealthBar {
                    size_t owner;      // L'ID de l'entité propriétaire (le joueur, un ennemi...)
                    float offset_x;    // Décalage horizontal par rapport au centre du propriétaire
                    float offset_y;    // Décalage vertical (ex: -20 pour être au-dessus)

                    HealthBar(size_t owner_entity, float off_x = 0, float off_y = -20.0f)
                        : owner(owner_entity), offset_x(off_x), offset_y(off_y) {}
                };
                struct Clickable {
                    float width;
                    float height;
                    Clickable() : width(), height() {}
                    Clickable(float new_width, float new_height) : width(new_width), height(new_height) {}
                };

                enum class ButtonState {
                };

                struct Action {
                    std::function<void()> on_click;
                    Action(std::function<void()> new_on_click) : on_click(new_on_click) {}
                };

                struct TextInput {
                    std::string buffer;
                    std::string placeholder;
                    bool is_focused = false;
                    size_t max_chars = 32;

                    TextInput() = default; // Garder le constructeur par défaut
                    TextInput(std::string new_placeholder) : placeholder(new_placeholder) {}
                };
            }
        }

#endif