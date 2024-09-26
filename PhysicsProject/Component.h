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
#include <memory> 
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
    virtual void start() {}
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
    virtual void start() {}
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
    virtual void start() {}
    void createBody();
    void applyForce(const sf::Vector2f& force);
    void toggleGravity(bool turnedOn);
    void applyImpulse(const sf::Vector2f& impulse);
    void setVelocity(const sf::Vector2f& velocity);
    sf::Vector2f getVelocity() const;
    void moveTowards(const sf::Vector2f& targetPosition, float speed);
    float getSpeed() const;

    b2Body* GetBody();

    float GetMass() const;
    void SetMass(float mass);
    Box2DWorld* GetWorld();

    float GetGravityScale() const;
    float GetRestitution() const;
    float GetMaxSpeed() const;

    void SetGravityScale(float scale);
    void SetRestitution(float restitution);
    void SetMaxSpeed(float maxSpeed);

private:
    bool m_gravityOn;
    Box2DWorld* m_world;
    float m_mass;
    float m_gravityScale;
    float m_restitution;
    float m_maxSpeed;
    b2Body* m_body;
};
class SpriteRendererComponent : public Component {
public:
    virtual void start() {}
    SpriteRendererComponent(const std::string& texturePath) {
        m_spritePath = texturePath;
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
    std::string getSpritePath() {
        return m_spritePath;
    }
private:
    void updateScale() {
        float scaleX = m_desiredSize.x / m_originalSize.x;
        float scaleY = m_desiredSize.y / m_originalSize.y;
        m_sprite.setScale(scaleX, scaleY);
    }
    std::string m_spritePath;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    sf::Vector2u m_originalSize;
    sf::Vector2f m_desiredSize;
};


class CircleColliderComponent : public Component, public ICollider {
public:
    CircleColliderComponent(float radius, const sf::Vector2f& localPosition = sf::Vector2f(10, 10))
        : m_radius(radius), m_localPosition(localPosition) {}
    virtual void start() {}
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
    virtual void start() {}
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
    virtual void start() {}
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
    virtual void start() {}
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
        float mass = rb->GetMass();   
    
        m_damagePerCollision = (rb && speed > 3.0f) ? speed * mass : 0;
        
        m_currentHealth -= m_damagePerCollision;
        m_currentHealth = std::max(0.0f, m_currentHealth);

        if (m_currentHealth <= 0) {
            getOwner()->destroy();
        }
       /* std::cout << getOwner()->getName() << " collided with: " << other->getName() << "| I took "<< m_damagePerCollision << " health" << std::endl;*/
     
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
class TimerComponent : public Component {
public:
    TimerComponent(float duration);
    void update(float deltaTime) override;
    void start();
    void pause();
    void resume();
    void reset();
    bool isFinished() const;
    bool isRunning() const;
    float getRemainingTime() const;
    float getElapsedTime() const;
    float getDuration() const;

private:
    float m_duration;
    bool m_isRunning;
    float m_remainingTime;
};

class BirdLauncherComponent : public Component {
public:
    using BirdCreationFunction = std::function<GameObject* (const sf::Vector2f&, const std::string&)>;

    BirdLauncherComponent(sf::RenderWindow* window, Box2DWorld* world, const sf::Vector2f& spawnPosition, BirdCreationFunction createBirdFunction, const std::string& spritePath);
    ~BirdLauncherComponent();
    void start() override;
    void update(float deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    void drawRope(sf::RenderWindow& window);

private:
    void spawnBird();
    void createSlingJoint();
    void updateBirdPosition(const sf::Vector2f& mousePos);
    void launchBird(const sf::Vector2f& releasePos);
    void resetLauncher();
    sf::Vector2f m_dragStart;
    sf::Vector2f m_launchPosition;
    sf::RenderWindow* m_window;
    sf::Vector2f m_spawnPosition;
    BirdCreationFunction m_createBirdFunction;
    std::string m_spritePath;
    GameObject* m_bird;
    bool m_isDragging;
    b2MouseJoint* m_mouseJoint;
    b2Joint* m_slingJoint;
    Box2DWorld* m_world;
    sf::Vector2f m_anchorPosition;
    std::unique_ptr<TimerComponent> m_resetTimer;
    sf::Vector2f m_currentMousePos;
    float m_maxPullDistance = 100.0f;
    bool m_birdLaunched;
    int m_thrownBirds = 0;

};

class AbilityComponent : public Component {
public:
    AbilityComponent() : m_launched(false), m_clickedAfterLaunch(false) {}

    virtual void update(float deltaTime) override {

        if (m_launched && !m_clickedAfterLaunch) {
            checkForClick();
        }
    }

    virtual void onLaunch() {
        m_launched = true;
        m_clickedAfterLaunch = false;
    }

    virtual void onClickAfterLaunch() {
        //Override for effect in inherited functions
    }

    bool isLaunched() const { return m_launched; }
    bool hasClickedAfterLaunch() const { return m_clickedAfterLaunch; }

    virtual void reset() {
        m_launched = false;
        m_clickedAfterLaunch = false;
    }

protected:
    bool m_launched;
    bool m_clickedAfterLaunch;

private:
    void checkForClick() {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        std::cout << "click" << std::endl;
            m_clickedAfterLaunch = true;
            onClickAfterLaunch();
        }
    }
};
class DoubleMassAbility : public AbilityComponent {
public:
    DoubleMassAbility() : m_originalMass(0.0f) {}

    void onLaunch() override {
        AbilityComponent::onLaunch();

        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (rigidBody) {
            m_originalMass = rigidBody->GetMass();
        }
    }

    void onClickAfterLaunch() override {
        std::cout << "Double Mass ability activated!" << std::endl;

        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (rigidBody) {
            float newMass = rigidBody->GetMass() * 2.0f;
            rigidBody->SetMass(newMass);
            std::cout << "Mass increased from " << m_originalMass << " to " << newMass << std::endl;
            auto transform = getOwner()->getComponent<TransformComponent>();
            if (transform) {
                transform->setScale(0.5f, 0.5f);
            }
        }
        else {
            std::cout << "Error: RigidBodyComponent not found" << std::endl;
        }
    }

    void reset() override {
        AbilityComponent::reset();

        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (rigidBody && m_originalMass > 0.0f) {
            rigidBody->SetMass(m_originalMass);
            std::cout << "Mass reset to original value: " << m_originalMass << std::endl;
        }
    }

private:
    float m_originalMass;
};
class BoostAbility : public AbilityComponent {
public:
    BoostAbility(float boostFactor = 2.0f) : m_boostFactor(boostFactor) {}

    void onLaunch() override {
        AbilityComponent::onLaunch();
    }

    void onClickAfterLaunch() override {
        std::cout << "Boost ability activated!" << std::endl;

        auto rigidBody = getOwner()->getComponent<RigidBodyComponent>();
        if (rigidBody) {
            sf::Vector2f currentVelocity = rigidBody->getVelocity();
            sf::Vector2f boostedVelocity = currentVelocity * m_boostFactor;
            rigidBody->setVelocity(boostedVelocity);
            std::cout << "Velocity increased from (" << currentVelocity.x << ", " << currentVelocity.y
                << ") to (" << boostedVelocity.x << ", " << boostedVelocity.y << ")" << std::endl;
        }
        else {
            std::cout << "Error: RigidBodyComponent not found" << std::endl;
        }
    }

    void reset() override {
        AbilityComponent::reset();
    }

private:
    float m_boostFactor;
};
class SplitAbility : public AbilityComponent {
public:
    SplitAbility(int splitCount = 3) : m_splitCount(splitCount) {}

    ~SplitAbility() {
        destroySplitBirds();
    }

    void onLaunch() override {
        AbilityComponent::onLaunch();
    }

    void onClickAfterLaunch() override {
        std::cout << "Split ability activated!" << std::endl;

        auto originalTransform = getOwner()->getComponent<TransformComponent>();
        auto originalRigidBody = getOwner()->getComponent<RigidBodyComponent>();

        if (originalTransform && originalRigidBody) {
            sf::Vector2f originalPosition = originalTransform->position;
            sf::Vector2f originalVelocity = originalRigidBody->getVelocity();

            for (int i = 0; i < m_splitCount - 1; ++i) {  // -1 because we already have the original
                auto newBird = GameObject::create(originalPosition, "SplitBird");

                // Copy components from the original bird
                newBird->addComponent<TransformComponent>(originalPosition.x, originalPosition.y + (i * 10));
                newBird->addComponent<SpriteRendererComponent>(getOwner()->getComponent<SpriteRendererComponent>()->getSpritePath());
                auto newRigidBody = newBird->addComponent<RigidBodyComponent>(originalRigidBody->GetWorld(), originalRigidBody->GetMass(), originalRigidBody->GetGravityScale());
                newBird->addComponent<BoxColliderComponent>(1.0f, 1.0f);  // Assuming original bird size

                newRigidBody->init();
                // Set slightly different velocity for each split bird
                float angleOffset = (i + 1) * 60.0f;  // degrees
                sf::Vector2f newVelocity = rotateVector(originalVelocity, angleOffset);
                newRigidBody->setVelocity(originalVelocity);

                m_splitBirds.push_back(newBird);

                std::cout << "Created split bird " << i + 1 << " at position (" << originalPosition.x << ", " << originalPosition.y << ")" << std::endl;
            }
        }
        else {
            std::cout << "Error: Required components not found" << std::endl;
        }
    }

    void reset() override {
        AbilityComponent::reset();
        destroySplitBirds();
    }

private:
    int m_splitCount;
    std::vector<GameObject*> m_splitBirds;

    sf::Vector2f rotateVector(const sf::Vector2f& vector, float angleDegrees) {
        float angleRadians = angleDegrees * 3.14159f / 180.0f;
        float cs = std::cos(angleRadians);
        float sn = std::sin(angleRadians);
        return sf::Vector2f(vector.x * cs - vector.y * sn, vector.x * sn + vector.y * cs);
    }

    void destroySplitBirds() {
        for (auto bird : m_splitBirds) {
            if (bird) {
                bird->destroy();
            }
        }
        m_splitBirds.clear();
        std::cout << "All split birds destroyed" << std::endl;
    }
};


enum class TextOrigin {
    Center,
    TopLeft,
};

class TextRendererComponent : public Component {
public:
    TextRendererComponent(const std::string& text) : m_text(text) {
        if (!m_font.loadFromFile("font.ttf")) {
            std::cout << "Error loading font" << std::endl;
        }
        m_sfText.setFont(m_font);
        m_sfText.setString(m_text);
        m_sfText.setCharacterSize(24); // Default size
        m_sfText.setFillColor(sf::Color::White); // Default color
    }

    void setText(const std::string& text) {
        m_text = text;
        m_sfText.setString(m_text);
    }

    void setCharacterSize(unsigned int size) {
        m_sfText.setCharacterSize(size);
    }

    void setFillColor(const sf::Color& color) {
        m_sfText.setFillColor(color);
    }

    void setFont(const sf::Font& font) {
        m_sfText.setFont(font);
    }

    void setOrigin(TextOrigin origin) {
        sf::FloatRect bounds = m_sfText.getLocalBounds();
        switch (origin) {
        case TextOrigin::Center:
            m_sfText.setOrigin(bounds.width / 2, bounds.height / 2);
            break;
        case TextOrigin::TopLeft:
            m_sfText.setOrigin(0, 0);
            break;
            // Add other origin options as needed
        }
    }

    void update(float deltaTime) override {
        auto transform = getOwner()->getComponent<TransformComponent>();
        if (transform) {
            m_sfText.setPosition(transform->position);
            m_sfText.setRotation(transform->rotation);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(m_sfText);
    }

private:
    std::string m_text;
    sf::Text m_sfText;
    sf::Font m_font;
};

class ButtonComponent : public Component {
public:
    ButtonComponent(const std::string& text, std::function<void()> onClick)
        : m_text(text), m_onClick(onClick) {
        if (!m_font.loadFromFile("font.ttf")) {
            std::cout << "Error loading font" << std::endl;
        }
        m_sfText.setFont(m_font);
        m_sfText.setString(m_text);
        m_sfText.setCharacterSize(24); 
        m_sfText.setFillColor(sf::Color::Black); 

        m_shape.setSize(sf::Vector2f(200, 50)); 
        m_shape.setFillColor(sf::Color(200, 200, 200)); 
    }
    void setCharacterSize(unsigned int size) {
        m_sfText.setCharacterSize(size);
    }

  
    void setFont(const sf::Font& font) {
        m_sfText.setFont(font);
    }

    void setButtonColor(const sf::Color& color) {
        m_shape.setFillColor(color);
    }

    void setTextColor(const sf::Color& color) {
        m_sfText.setFillColor(color);
    }

    void setButtonSize(const sf::Vector2f& size) {
        m_shape.setSize(size);
        m_shape.setOrigin(size.x / 2, size.y / 2);
    }

    void update(float deltaTime) override {
        auto transform = getOwner()->getComponent<TransformComponent>();
        if (transform) {
            m_shape.setPosition(transform->position);
            m_shape.setRotation(transform->rotation);

            // Center the text in the button
            sf::FloatRect textBounds = m_sfText.getLocalBounds();
            m_sfText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            m_sfText.setPosition(transform->position);
        }
    }

    void handleEvent(const sf::Event& event) override {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (m_shape.getGlobalBounds().contains(mousePos)) {
                m_onClick();
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(m_shape);
        window.draw(m_sfText);
    }

private:
    std::string m_text;
    sf::Text m_sfText;
    sf::Font m_font;
    sf::RectangleShape m_shape;
    std::function<void()> m_onClick;
};

class PigComponent : public Component {
public:
    PigComponent() {}
    virtual void update(float deltaTime) override {}
    // This class is just to identify pigs easier to implement than tags for now
};