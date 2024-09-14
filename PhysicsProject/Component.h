#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "GameObject.h"
class GameObject; // Forward declaration

class Component {
public:
    virtual ~Component() = default;
};

class TransformComponent : public Component {
public:
    TransformComponent(float x, float y) : position(x, y) {}

    sf::Vector2f position;
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
            // Handle texture loading error
            throw std::runtime_error("Failed to load texture: " + texturePath);
        }
        m_sprite.setTexture(m_texture);
    }

    void setScale(float scaleX, float scaleY) {
        m_sprite.setScale(scaleX, scaleY);
    }

    void setOrigin(float x, float y) {
        m_sprite.setOrigin(x, y);
    }

    sf::Sprite& getSprite() {
        return m_sprite;
    }

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;
};