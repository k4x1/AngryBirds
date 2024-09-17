#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "GameObject.h"
#include <iostream>
#include <functional>
#include "Systems.h"
#include "EventSystem.h"
//All components are here because it feels easier to work with over having them all on separate files
class GameObject;


class Component {
public:
    virtual ~Component() {
        EventSystem::getInstance().removeListener(this);
        
    }
    virtual void update(float deltaTime) {}
    virtual void handleEvent(const sf::Event& event) {}

    void setOwner(GameObject* owner) {
        m_owner = owner;
        EventSystem::getInstance().addListener(this);
    }
    GameObject* getOwner() const { return m_owner; }

private:

    GameObject* m_owner = nullptr;
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
    RigidBodyComponent(float mass = 1.0f, float gravityScale = 1.0f, float restitution = 0.5f, float maxSpeed = 10000.0f)
        : mass(mass), gravityScale(gravityScale), velocity(0.0f, 0.0f), restitution(restitution), maxSpeed(maxSpeed) {}

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
    void moveTowards(const sf::Vector2f& targetPosition, float speed) {
        auto transform = getOwner()->getComponent<TransformComponent>();
        if (transform) {
            sf::Vector2f direction = targetPosition - transform->position;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (distance > 0) {
                direction /= distance; 

                sf::Vector2f desiredVelocity = direction * speed;

                sf::Vector2f steeringForce = desiredVelocity - velocity;

                applyForce(steeringForce);

       
                float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
                if (currentSpeed > maxSpeed) {
                    velocity = (velocity / currentSpeed) * maxSpeed;
                }
            }
        }
    }
    float restitution;
    float mass;
    float gravityScale;
    sf::Vector2f velocity;
    float maxSpeed;
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
    BoxColliderComponent(TransformComponent* transform)
        : m_transform(transform), m_width(100), m_height(100), m_visible(true) {
        updateShape();
    }
    sf::FloatRect getBounds() const {
        float scaledWidth = m_width * m_transform->scale.x;
        float scaledHeight = m_height * m_transform->scale.y;
        return sf::FloatRect(
            m_transform->position.x - scaledWidth / 2,
            m_transform->position.y - scaledHeight / 2,
            scaledWidth,
            scaledHeight
        );
    }


    bool intersects(const BoxColliderComponent& other) const {
        return getBounds().intersects(other.getBounds());
    }

    void setSize(float width, float height) {
        m_width = width;
        m_height = height;
        updateShape();
    }

    void setVisible(bool visible) {
        m_visible = visible;
    }

    void draw(sf::RenderWindow& window) {
        if (m_visible) {

            updateShape();
            window.draw(m_shape);
        }
    }

    std::function<void(GameObject*)> onCollision;

private:
    void updateShape() {
        sf::FloatRect bounds = getBounds();
        m_shape.setSize(sf::Vector2f(bounds.width, bounds.height));
        m_shape.setOrigin(0, 0);  
        m_shape.setPosition(m_transform->position); 
        m_shape.setRotation(m_transform->rotation);
        m_shape.setFillColor(sf::Color::Transparent);
        m_shape.setOutlineColor(sf::Color::Green);
        m_shape.setOutlineThickness(2);
    }


    TransformComponent* m_transform;
    float m_width;
    float m_height;
    bool m_visible; 
    sf::RectangleShape m_shape;
};

class FollowMouseComponent : public Component {
public:
    FollowMouseComponent(sf::RenderWindow* window) : m_window(window), m_isClicking(false) {}

    void update(float deltaTime) override {
        if (!m_rigidbody) {
            m_rigidbody = getOwner()->getComponent<RigidBodyComponent>();
            if (!m_rigidbody) return;
        }

        if (m_window&& m_isClicking) {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(*m_window);
            sf::Vector2f worldPosition = m_window->mapPixelToCoords(mousePosition);
            m_rigidbody->moveTowards(worldPosition,200.0f);
        }
    }

    void handleEvent(const sf::Event& event) override {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                m_isClicking = true;
              
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                m_isClicking = false;
            }
        }
    }

    bool isClicking() const { return m_isClicking; }

private:
    RigidBodyComponent* m_rigidbody = nullptr;
    sf::RenderWindow* m_window = nullptr;
    bool m_isClicking;
};

