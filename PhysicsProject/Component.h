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
class RigidBodyComponent;
class TransformComponent;

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
    virtual void start() {}
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
    TransformComponent(float x, float y);
    void setPosition(float x, float y);
    void setRotation(float angle);
    void setScale(float scaleX, float scaleY);

    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;

private:
    void updateBox2DBody();
    RigidBodyComponent* rigidBody;
};

class RenderComponent : public Component {
public:
    RenderComponent(const sf::Color& color) :  color(color) {
        init();
    }
    void init() override {
       

        if (!transform) {
            return;
        }
        shape = sf::RectangleShape(sf::Vector2f(60 * transform->scale.x, 60 * transform->scale.y)),
            shape.setFillColor(color);
    }
    void update(float deltaTime) override {
        if (!transform) {
             transform = getOwner()->getComponent<TransformComponent>();
            init();
            //d
            
        }
        
    }
    TransformComponent* transform;
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

private:
    Box2DWorld* m_world;
    float m_mass;
    float m_gravityScale;
    float m_restitution;
    float m_maxSpeed;
    b2Body* m_body;
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
        m_desiredSize = sf::Vector2f(transformScale.x * 60, transformScale.y * 60); 
        updateScale();
    }

    sf::Sprite& getSprite() {
        return m_sprite;
    }
    void setTint(const sf::Color& color) {
        m_sprite.setColor(color);
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


class CircleColliderComponent : public Component, public ICollider {
public:
    CircleColliderComponent(float radius, const sf::Vector2f& localPosition = sf::Vector2f(10, 10))
        : m_radius(radius), m_localPosition(localPosition) {}

    void init() override {
        auto transform = getOwner()->getComponent<TransformComponent>();
        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (!transform) {
            std::cout << "No TransformComponent found for CircleCollider" << std::endl;
            return;
        }
        if (!rigidBody) {
            std::cout << "No RigidBodyComponent found for CircleCollider" << std::endl;
            return;
        }

        b2CircleShape shape;
        shape.m_radius = m_radius * transform->scale.x; // radius based on scale
        shape.m_p = b2Vec2(m_localPosition.x / 30.0f, m_localPosition.y / 30.0f); 

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = rigidBody->GetMass();
        fixtureDef.restitution = rigidBody->GetRestitution();

        m_fixture = rigidBody->GetBody()->CreateFixture(&fixtureDef);
    }

    void onCollision(GameObject* other) override {
        getOwner()->OnCollision(other);
    }

    void debugDraw(sf::RenderWindow& window) {
        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (!rigidBody || !rigidBody->GetBody() || !m_fixture) {
            return;
        }

        b2CircleShape* shape = dynamic_cast<b2CircleShape*>(m_fixture->GetShape());
        if (!shape) {
            return;
        }

        b2Transform xf = rigidBody->GetBody()->GetTransform();
        b2Vec2 center = xf.p + b2Mul(xf.q, shape->m_p); 
        float radius = shape->m_radius;

        sf::CircleShape circleShape(radius * 30.0f);
        circleShape.setPosition(center.x * 30.0f - radius * 30.0f, center.y * 30.0f - radius * 30.0f);
        circleShape.setFillColor(sf::Color::Transparent);
        circleShape.setOutlineColor(sf::Color::Blue);
        circleShape.setOutlineThickness(2);

        window.draw(circleShape);
    }

private:
    float m_radius;
    sf::Vector2f m_localPosition;
    b2Fixture* m_fixture = nullptr;
};


class BoxColliderComponent : public Component, public ICollider {
public:
    BoxColliderComponent(float width, float height) : m_width(width), m_height(height) {}

    void init() override {
        auto transform = getOwner()->getComponent<TransformComponent>();
        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (!transform|| !rigidBody) {
            return;
        }

        b2PolygonShape shape;
        shape.SetAsBox((m_width) *transform->scale.x, (m_height) *transform->scale.y, b2Vec2(transform->scale.x, transform->scale.y), 0);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = rigidBody->GetMass();
        fixtureDef.restitution = rigidBody->GetRestitution();

        m_fixture = rigidBody->GetBody()->CreateFixture(&fixtureDef);
    }

    void onCollision(GameObject* other) override {
        getOwner()->OnCollision(other);
    }

    void debugDraw(sf::RenderWindow& window) {
        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (!rigidBody || !rigidBody->GetBody() || !m_fixture) {
            return;
        }

        b2PolygonShape* shape = dynamic_cast<b2PolygonShape*>(m_fixture->GetShape());
        if (!shape) {
            return;
        }

        b2Transform xf = rigidBody->GetBody()->GetTransform();
        int32 vertexCount = shape->m_count;
        b2Assert(vertexCount <= b2_maxPolygonVertices);
        b2Vec2 vertices[b2_maxPolygonVertices];

        for (int32 i = 0; i < vertexCount; ++i) {
            vertices[i] = b2Mul(xf, shape->m_vertices[i]);
        }

        sf::ConvexShape convexShape(vertexCount);
        for (int32 i = 0; i < vertexCount; ++i) {
            convexShape.setPoint(i, sf::Vector2f(vertices[i].x * 30.0f, vertices[i].y * 30.0f));
        }

        convexShape.setFillColor(sf::Color::Transparent);
        convexShape.setOutlineColor(sf::Color::Green);
        convexShape.setOutlineThickness(2);

        window.draw(convexShape);
    }

private:
    float m_width;
    float m_height;
 
    b2Fixture* m_fixture = nullptr;
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
    BreakableComponent(float maxHealth = 10, float damagePerCollision = 0.0f)
        : m_maxHealth(maxHealth), m_currentHealth(maxHealth), m_damagePerCollision(damagePerCollision) {}

    void update(float deltaTime) override {
        auto renderComponent = getOwner()->getComponent<RenderComponent>();
        if (renderComponent) {
            updateColor(renderComponent);
        }
        auto spriteRenderer = getOwner()->getComponent<SpriteRendererComponent>();
        if (spriteRenderer) {
            updateSpriteColor(spriteRenderer);
        }
    }

    void onCollision(GameObject* other) {
        auto rb = other->getComponent<RigidBodyComponent>();
        float speed = rb->getSpeed();   
    
        m_damagePerCollision = (rb && speed > 3.0f) ? speed : 0;
        
        m_currentHealth -= m_damagePerCollision;
        m_currentHealth = std::max(0.0f, m_currentHealth);

        if (m_currentHealth <= 0) {
            getOwner()->destroy();
        }
        std::cout << getOwner()->getName() << " collided with: " << other->getName() << "| I took "<< m_damagePerCollision << " health" << std::endl;
     
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
    void updateSpriteColor(SpriteRendererComponent* spriteRenderer) {
        float healthPercentage = m_currentHealth / m_maxHealth;
        sf::Color tint = interpolateColor(sf::Color::White, sf::Color::Red, 1 - healthPercentage);

        spriteRenderer->setTint(tint);
    }

    sf::Color interpolateColor(const sf::Color& color1, const sf::Color& color2, float factor) {
        return sf::Color(
            static_cast<sf::Uint8>(color1.r + (color2.r - color1.r) * factor),
            static_cast<sf::Uint8>(color1.g + (color2.g - color1.g) * factor),
            static_cast<sf::Uint8>(color1.b + (color2.b - color1.b) * factor)
        );
    }
    float m_maxHealth;
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
class BirdLauncherComponent : public Component {
public:
    using BirdCreationFunction = std::function<GameObject* (const sf::Vector2f&, const std::string&)>;

    BirdLauncherComponent(sf::RenderWindow* window, Box2DWorld* world, const sf::Vector2f& spawnPosition, BirdCreationFunction createBirdFunction, const std::string& spritePath)
        : m_window(window), m_world(world), m_spawnPosition(spawnPosition), m_anchorPosition(spawnPosition), m_createBirdFunction(createBirdFunction), m_spritePath(spritePath), m_bird(nullptr), m_isDragging(false) {}


    void start() override {
        spawnBird();

  
    }

    void update(float deltaTime) override {
        if (!m_bird) return;

        if (m_isDragging) {
            // Optionally, you can add visual feedback here (e.g., drawing a line)
        }
        else {
            // Check if the bird has moved significantly away from the launch position
            auto transform = m_bird->getComponent<TransformComponent>();
            if (transform) {
                float distance = std::sqrt(
                    std::pow(transform->position.x - m_launchPosition.x, 2) +
                    std::pow(transform->position.y - m_launchPosition.y, 2)
                );

                if (distance > 300.0f) { // Adjust as needed
                    // Bird has moved far enough, prepare for next launch
                    resetLauncher();
                }
            }
        }
    }
    void handleEvent(const sf::Event& event) override {
        if (!m_bird) return;

        auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
        if (!rigidBody) return;

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Start dragging
                m_isDragging = true;
                m_dragStart = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left && m_isDragging) {
                // Launch the bird
                launchBird(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                m_isDragging = false;
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            if (m_isDragging) {
                // Update bird position while dragging
                updateBirdPosition(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
            }
        }
    }

private:
    void spawnBird() {
        std::cout << "bird spanwd";
        m_bird = m_createBirdFunction(m_spawnPosition, m_spritePath);

    }
    void createSlingJoint() {
        if (!m_bird) {
            std::cout << "Error: No bird to attach sling joint to." << std::endl;
            return;
        }

        auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
        if (!rigidBody) {
            std::cout << "Error: RigidBodyComponent not found on bird." << std::endl;
            return;
        }

        b2Body* birdBody = rigidBody->GetBody();
        if (birdBody == nullptr) {
            std::cout << "Error: RigidBody's b2Body is null. Did you call init()?" << std::endl;
            return;
        }

        if (m_world == nullptr || m_world->GetWorld() == nullptr) {
            std::cout << "Error: Physics world is null." << std::endl;
            return;
        }

        // Create a static body for the anchor point
        b2BodyDef anchorBodyDef;
        anchorBodyDef.type = b2_staticBody;
        anchorBodyDef.position.Set(m_anchorPosition.x / 30.0f, m_anchorPosition.y / 30.0f);
        b2Body* anchorBody = m_world->GetWorld()->CreateBody(&anchorBodyDef);

        if (anchorBody == nullptr) {
            std::cout << "Error: Failed to create anchor body." << std::endl;
            return;
        }

        // Manually create the distance joint
        b2DistanceJointDef jointDef;
        jointDef.Initialize(birdBody, anchorBody, birdBody->GetPosition(), anchorBody->GetPosition());
        jointDef.collideConnected = true;
        jointDef.stiffness= 4.0f;
        jointDef.damping = 0.5f;

        // Create the joint
        m_slingJoint = (b2DistanceJoint*)m_world->GetWorld()->CreateJoint(&jointDef);

        if (m_slingJoint == nullptr) {
            std::cout << "Error: Failed to create sling joint." << std::endl;
        }
        else {
            std::cout << "Sling joint created successfully." << std::endl;
        }
    }
    void updateBirdPosition(const sf::Vector2f& mousePos) {
        if (!m_bird) return;

        auto transform = m_bird->getComponent<TransformComponent>();
        if (!transform) return;

        // Calculate the direction from anchor to mouse
        sf::Vector2f direction = m_anchorPosition - mousePos;

        // Limit the pull distance
        float maxPullDistance = 100.0f; // Adjust as needed
        float currentDistance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (currentDistance > maxPullDistance) {
            direction *= (maxPullDistance / currentDistance);
        }

        // Set the bird's position
        transform->position = m_anchorPosition - direction;

        // Update the physics body position
        auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
        if (rigidBody && rigidBody->GetBody()) {
            rigidBody->GetBody()->SetTransform(b2Vec2(transform->position.x / 30.0f, transform->position.y / 30.0f), rigidBody->GetBody()->GetAngle());
        }
    }
    void launchBird(const sf::Vector2f& releasePos) {
        if (!m_bird) return;

        auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
        if (!rigidBody) return;

        // Calculate launch vector
        sf::Vector2f launchVector = m_anchorPosition - releasePos;
        float launchForce = std::min(launchVector.x * launchVector.x + launchVector.y * launchVector.y, 10000.0f); // Limit max force

        // Normalize and apply force
        float length = std::sqrt(launchVector.x * launchVector.x + launchVector.y * launchVector.y);
        if (length > 0) {
            launchVector /= length;
            launchVector *= launchForce * 0.1f; // Adjust multiplier as needed
            rigidBody->applyImpulse(launchVector);
        }

        // Store launch position for distance checking
        m_launchPosition = m_bird->getComponent<TransformComponent>()->position;

        // Remove the distance joint
        if (m_slingJoint) {
            m_world->GetWorld()->DestroyJoint(m_slingJoint);
            m_slingJoint = nullptr;
        }
    }
    void resetLauncher() {
        // Destroy the old bird
        if (m_bird) {
            m_bird->destroy();
            m_bird = nullptr;
        }

        // Spawn a new bird
        spawnBird();
    }
    sf::Vector2f m_dragStart;
    sf::Vector2f m_launchPosition;
    sf::RenderWindow* m_window;
    sf::Vector2f m_spawnPosition;
    BirdCreationFunction m_createBirdFunction;
    std::string m_spritePath;
    GameObject* m_bird;
    bool m_isDragging;
    b2MouseJoint* m_mouseJoint = nullptr;
    b2Joint* m_slingJoint = nullptr;
    Box2DWorld* m_world = nullptr; 
    sf::Vector2f m_anchorPosition;
};

// Implement similar classes for other joint types (PrismaticJoint, PulleyJoint, etc.)
