#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "GameObject.h"
#include <iostream>
#include <cmath>
#include <functional>
#include "Systems.h"
#include "EventSystem.h"
#include "ComponentManager.h"
#include "Box2DWorld.h"
#include "box2d/box2d.h"
//All components are here because it feels easier to work with over having them all on separate files
class GameObject;
class ComponentManager;


class ICollider {
public:
    virtual ~ICollider() = default;
    virtual void onCollision(GameObject* other) = 0;
};

class Component {
public:
    virtual ~Component() {
        EventSystem::getInstance().removeListener(this);
        
    }
    virtual void update(float deltaTime) {}
    virtual void init() {}
    virtual void onCollision(GameObject* other) {}
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
    sf::Vector2f scale = sf::Vector2f(0.25f,0.25f);
    float rotation = 0.0f;
    void setPosition(float x, float y)
    {
        position = sf::Vector2f(x, y);
    }
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
    RigidBodyComponent(Box2DWorld* world, float mass = 1.0f, float gravityScale = 1.0f, float restitution = 0.5f, float maxSpeed = 10.0f);
    ~RigidBodyComponent();

    void init() override;
    void update(float deltaTime) override;

    void createBody();
    void applyForce(const sf::Vector2f& force);
    void applyImpulse(const sf::Vector2f& impulse);
    void setVelocity(const sf::Vector2f& velocity);
    sf::Vector2f getVelocity() const;
    void moveTowards(const sf::Vector2f& targetPosition, float speed);
    float getSpeed() const;

    b2Body* GetBody();

    float GetMass() const;
    float GetGravityScale() const;
    float GetRestitution() const;
    float GetMaxSpeed() const;

    void SetMass(float mass);
    void SetGravityScale(float scale);
    void SetRestitution(float restitution);
    void SetMaxSpeed(float maxSpeed);
    b2Body* m_body;

private:
    Box2DWorld* m_world;
    float m_mass;
    float m_gravityScale;
    float m_restitution;
    float m_maxSpeed;
    b2Fixture* m_fixture;
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
        m_desiredSize = sf::Vector2f(transformScale.x * 100, transformScale.y * 100); 
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



class BoxColliderComponent : public Component, public ICollider {
public:
    BoxColliderComponent(TransformComponent* transform, float width, float height);

    void createBox2dShape();
    sf::FloatRect getBounds() const;
    void update(float deltaTime) override;
    bool intersects(const BoxColliderComponent& other) const;
    void setSize(float width, float height);
    void setVisible(bool visible);
    void draw(sf::RenderWindow& window);
    void onCollision(GameObject* other) override;

private:
    void updateSFMLShape();

    TransformComponent* m_transform;
    float m_width;
    float m_height;
    bool m_visible;
    sf::RectangleShape m_shape;
    bool m_fixtureCreated;
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
                m_rigidbody->moveTowards(worldPosition,100.0f);
            //getOwner()->getComponent<TransformComponent>()->position = worldPosition;
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



class BreakableComponent : public Component {
public:
    BreakableComponent(int maxHealth = 300, float damagePerCollision = 0.0f)
        : m_maxHealth(maxHealth), m_currentHealth(maxHealth), m_damagePerCollision(damagePerCollision) {}

    void update(float deltaTime) override {
        auto renderComponent = getOwner()->getComponent<RenderComponent>();
        if (renderComponent) {
            updateColor(renderComponent);
        }

    }

    void onCollision(GameObject* other) {
       
        auto rb = other->getComponent<RigidBodyComponent>();
        if (rb) {
            m_damagePerCollision = rb->getSpeed();
        }
        m_currentHealth -= m_damagePerCollision;
        m_currentHealth = std::max(0.0f, m_currentHealth);

        if (m_currentHealth <= 0) {
            getOwner()->destroy();
        }
     
    }

private:
    void updateColor(RenderComponent* renderComponent) {
        float healthPercentage = m_currentHealth / m_maxHealth;
        sf::Color originalColor = sf::Color::White;
        sf::Color damageColor = sf::Color::Red;


        sf::Uint8 r = static_cast<sf::Uint8>(originalColor.r + (damageColor.r - originalColor.r) * (1 - healthPercentage));
        sf::Uint8 g = static_cast<sf::Uint8>(originalColor.g + (damageColor.g - originalColor.g) * (1 - healthPercentage));
        sf::Uint8 b = static_cast<sf::Uint8>(originalColor.b + (damageColor.b - originalColor.b) * (1 - healthPercentage));

        renderComponent->color = sf::Color(r, g, b);
    }

    int m_maxHealth;
    float m_currentHealth;
    float m_damagePerCollision;
};
class RevoluteJointComponent : public Component {
public:
    RevoluteJointComponent(Box2DWorld* world, RigidBodyComponent* bodyA, RigidBodyComponent* bodyB,
        const b2Vec2& anchor) {
        b2RevoluteJointDef jointDef;
        jointDef.Initialize(bodyA->GetBody(), bodyB->GetBody(), anchor);
        m_joint = (b2RevoluteJoint*)world->GetWorld()->CreateJoint(&jointDef);
    }

private:
    b2RevoluteJoint* m_joint;
};

// Implement similar classes for other joint types (PrismaticJoint, PulleyJoint, etc.)
