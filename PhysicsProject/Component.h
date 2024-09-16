#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "GameObject.h"
#include <iostream>
#include <functional>
//All components are here because it feels easier to work with over having them all on separate files
class GameObject; 

class Component {
public:
    virtual ~Component() = default;
};

class TransformComponent : public Component {
public:
    TransformComponent(float x, float y) : position(x, y) {}

    sf::Vector2f position;
    sf::Vector2f scale = sf::Vector2f(1.0f,1.0f);
    float rotation = 0.0f;
};

class RenderComponent : public Component {
public:
    RenderComponent(const sf::Color& color) : shape(sf::Vector2f(50, 50)), color(color) {
        shape.setFillColor(color);
    }

    sf::RectangleShape shape;
    sf::Color color;
};

class RigidBodyComponent : public Component {
public:
    RigidBodyComponent(float mass = 1.0f, float gravityScale = 1.0f)
        : mass(mass), gravityScale(gravityScale), velocity(0.0f, 0.0f) {}

    void applyForce(const sf::Vector2f& force) {
        velocity += force / mass;
    }

    void update(GameObject* gameObject, float deltaTime) {
        auto transform = gameObject->getComponent<TransformComponent>();
        if (transform) {
            // Apply gravity
            velocity.y += 98.1f * gravityScale * deltaTime;

            // Update position
            transform->position += velocity * deltaTime;
        }
    }

    float mass;
    float gravityScale;
    sf::Vector2f velocity;
};

class SpriteRendererComponent : public Component {
public:
    SpriteRendererComponent(const std::string& texturePath) {
        if (!m_texture.loadFromFile(texturePath)) {
            throw std::runtime_error("Failed to load texture: " + texturePath);
        }
        m_sprite.setTexture(m_texture);
        m_originalSize = m_texture.getSize();
        m_desiredSize = sf::Vector2f(m_originalSize);
        updateScale();
    }

    void setDesiredSize(float width, float height) {
        m_desiredSize = sf::Vector2f(width, height);
        updateScale();
    }

    void updateTransformScale(const sf::Vector2f& transformScale) {
        m_desiredSize = sf::Vector2f(transformScale.x * 100, transformScale.y * 100); //100 pixels = 1 unit
        updateScale();
    }

    sf::Sprite& getSprite() {
        return m_sprite;
    }

private:
    void updateScale() {
        float scaleX = m_desiredSize.x / m_originalSize.x;
        float scaleY = m_desiredSize.y / m_originalSize.y;
        m_sprite.setScale(scaleX, scaleY);
    }

    sf::Texture m_texture;
    sf::Sprite m_sprite;
    sf::Vector2u m_originalSize;
    sf::Vector2f m_desiredSize;
};

class BoxColliderComponent : public Component {
public:
    BoxColliderComponent(TransformComponent* transform, float width, float height)
        : m_transform(transform), m_width(width), m_height(height) {}

    sf::FloatRect getBounds() const {
        return sf::FloatRect(
            m_transform->position.x - m_width / 2,
            m_transform->position.y - m_height / 2,
            m_width * m_transform->scale.x,
            m_height * m_transform->scale.y
        );
    }

    bool intersects(const BoxColliderComponent& other) const {
        
        return getBounds().intersects(other.getBounds());
    }
     
    void setSize(float width, float height) {
        m_width = width;
        m_height = height;
    }
    std::function<void(GameObject*)> onCollision;   
private:
    TransformComponent* m_transform;
    float m_width;
    float m_height;
};
