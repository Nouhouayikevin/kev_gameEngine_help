/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-24
** File description:
** RenderSystem.hpp
*/


#include "../interfaces/ISystem.hpp"
#include "../Component.hpp"
#include "../../GameEngine_Include/core/GameEngine.hpp"
#include "../interfaces/ISprite.hpp"

#include <string>

class RenderSystem : public ISystem {
std::string _registerGroup;
public:
    RenderSystem(std::string registerGroup = "default") {
        _registerGroup = registerGroup;
    }
    void run(GameEngine& gameEng, float dt) override {
        ResourceManager& resources = gameEng.getResourceManager();
        IWindow& window = gameEng.getWindowManager();
        Registry& registry = gameEng.getRegistry(_registerGroup);

        enum class RenderType { SPRITE, RECTANGLE, CIRCLE, TEXT };

        struct Renderable {
            int z_index;
            RenderType type;

            const Component::Core::Position* pos;
            const Component::Core::Scale* scale = nullptr;
            const Component::Core::Rotation* rotation = nullptr;

            union {
                const Component::Graphics::Sprite* sprite_data;
                const Component::Graphics::RectangleShape* rect_data;
                const Component::Graphics::CircleShape* circle_data;
                const Component::Graphics::UIText* text_data;
            };
            const Component::Core::Hitbox* hitbox = nullptr;
        };

        std::vector<Renderable> renderables;


        auto& positions = registry.get_components<Component::Core::Position>();
        auto& scales = registry.get_components<Component::Core::Scale>();
        auto& rotations = registry.get_components<Component::Core::Rotation>();
        auto& hitboxes = registry.get_components<Component::Core::Hitbox>();

        auto& sprites = registry.get_components<Component::Graphics::Sprite>();
        auto& rects = registry.get_components<Component::Graphics::RectangleShape>();
        auto& circles = registry.get_components<Component::Graphics::CircleShape>();
        auto& texts = registry.get_components<Component::Graphics::UIText>();
        
        size_t entity_count = registry.get_entity_count(); 
    
        for (size_t i = 0; i < entity_count; ++i) {
            if (!positions[i]) continue;

            Renderable item;
            item.pos = &(*positions[i]);
            item.scale = i < scales.size() && scales[i] ? &(*scales[i]) : nullptr;
            item.rotation = i < rotations.size() && rotations[i] ? &(*rotations[i]) : nullptr;
            item.hitbox = i < hitboxes.size() && hitboxes[i] ? &(*hitboxes[i]) : nullptr;

            if (sprites[i]) {
                item.type = RenderType::SPRITE;
                item.z_index = sprites[i]->z_index;
                item.sprite_data = &(*sprites[i]);
                renderables.push_back(item);
                
                // Debug: log entities with MissileTag
                if (i < registry.get_components<Component::Gameplay::MissileTag>().size() && 
                    registry.get_components<Component::Gameplay::MissileTag>()[i]) {
                    std::cout << "RenderSystem: Adding missile entity " << i << " to renderables at pos (" 
                              << item.pos->x << ", " << item.pos->y << ") z_index=" << item.z_index << std::endl;
                }
            } else if (rects[i] && item.hitbox) { 
                item.type = RenderType::RECTANGLE;
                item.z_index = 50;
                item.rect_data = &(*rects[i]);
                renderables.push_back(item);
            } else if (circles[i]) {
                item.type = RenderType::CIRCLE;
                item.z_index = 50;
                item.circle_data = &(*circles[i]);
                renderables.push_back(item);
            } else if (texts[i]) {
                item.type = RenderType::TEXT;
                item.z_index = 100; // Texte au-dessus
                item.text_data = &(*texts[i]);
                renderables.push_back(item);
            }
        }

        std::sort(renderables.begin(), renderables.end(), [](const auto& a, const auto& b) {
            return a.z_index < b.z_index;
        });

        for (const auto& renderable : renderables) {
            switch (renderable.type) {
                case RenderType::SPRITE: {
                    if (!renderable.sprite_data) break;
                    
                    std::unique_ptr<ISprite> display_sprite = resources.createSprite();
                    try {

                        ITexture& texture = resources.get_texture(renderable.sprite_data->texture_id);
                        display_sprite->setTexture(texture);
                    } catch(const std::exception& e) {
                        std::cerr << "RenderSystem: Failed to get texture ID " << renderable.sprite_data->texture_id << std::endl;
                        continue;
                    }

                    display_sprite->setPosition({renderable.pos->x, renderable.pos->y});

                    if (renderable.scale) {
                        display_sprite->setScale({renderable.scale->width, renderable.scale->height});
                        std::cout << "RenderSystem: Applying scale (" << renderable.scale->width << ", " << renderable.scale->height 
                                  << ") to sprite at (" << renderable.pos->x << ", " << renderable.pos->y << ")" << std::endl;
                    }
                    if (renderable.rotation) {
                        display_sprite->setRotation(renderable.rotation->angle);
                    }

                    display_sprite->setTextureRect({
                        renderable.sprite_data->source_rect.x,
                        renderable.sprite_data->source_rect.y,
                        renderable.sprite_data->source_rect.width,
                        renderable.sprite_data->source_rect.height
                    });
                    
                    window.draw(*display_sprite);
                    break;
                }
                case RenderType::RECTANGLE: {
                    if (!renderable.rect_data || !renderable.hitbox) break;

                    window.drawRectangle(
                        renderable.pos->x, renderable.pos->y,
                        renderable.hitbox->width, renderable.hitbox->height,
                        renderable.rect_data->fill_color
                    );
                    break;
                }
                case RenderType::CIRCLE: {
                    if (!renderable.circle_data) break;

                    window.drawCircle(
                        renderable.pos->x, renderable.pos->y,
                        renderable.circle_data->radius,
                        renderable.circle_data->fill_color
                    );
                    break;
                }
                case RenderType::TEXT: {
                    if (!renderable.text_data) break;

                    try {
                        IFont& font = resources.get_font(renderable.text_data->font_id);
                        
                        // Offset for button text centering
                        float x_offset = 20;
                        float y_offset = 10;
                        
                        window.drawText(
                            renderable.text_data->content,
                            renderable.pos->x + x_offset,
                            renderable.pos->y + y_offset,
                            font,
                            renderable.text_data->character_size,
                            renderable.text_data->color
                        );
                    } catch(const std::exception& e) {
                        // Font not found, skip rendering
                    }
                    break;
                }
            }
        }
    }
};