#include "Component.h"
#include "Box2DWorld.h"
#include <SFML/System/Vector2.hpp>
#include "box2d/box2d.h"
#include <memory> // for std::unique_ptr

BirdLauncherComponent::BirdLauncherComponent(sf::RenderWindow* window, Box2DWorld* world, const sf::Vector2f& spawnPosition, BirdCreationFunction createBirdFunction, const std::string& spritePath)
    : m_window(window), m_world(world), m_spawnPosition(spawnPosition), m_anchorPosition(spawnPosition), m_createBirdFunction(createBirdFunction), m_spritePath(spritePath), m_bird(nullptr), m_isDragging(false)
    {
        m_resetTimer = std::make_unique<TimerComponent>(3.0f);

    }

void BirdLauncherComponent::start() {
    std::cout << "BirdLauncherComponent::start() called" << std::endl;
    spawnBird();
    std::cout << "Bird spawned" << std::endl;


}
void BirdLauncherComponent::update(float deltaTime)  {
    if (!m_bird) {
        std::cout << "m_bird is null in update()" << std::endl;
        return;
    }
    m_bird->getComponent<RigidBodyComponent>()->toggleGravity(false);
 
    if (m_resetTimer->isRunning()) {
        m_resetTimer->update(deltaTime);
        if (m_resetTimer->isFinished()) {
            resetLauncher();
        }
    }
    if (m_isDragging) {
        // Update bird position while dragging
        updateBirdPosition(sf::Vector2f(m_currentMousePos.x, m_currentMousePos.y));
    }


}
void BirdLauncherComponent::handleEvent(const sf::Event& event)  {
    if (!m_bird) {
        std::cout << "m_bird is null in handleEvent()" << std::endl;
        return;
    }

    auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
    if (!rigidBody) {
        std::cout << "RigidBodyComponent not found in handleEvent()" << std::endl;
        return;
    }

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
        m_currentMousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
    }
}

void BirdLauncherComponent::spawnBird() {
    std::cout << "BirdLauncherComponent::spawnBird() called" << std::endl;
    std::cout << "Spawning bird at position: " << m_spawnPosition.x << ", " << m_spawnPosition.y << std::endl;

    m_bird = m_createBirdFunction(m_spawnPosition, m_spritePath);
    if (!m_bird) {
        std::cout << "Error: Failed to create bird" << std::endl;
        return;
    }
    std::cout << "Bird created successfully" << std::endl;

    auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
    if (!rigidBody) {
        std::cout << "Error: RigidBodyComponent not found on bird" << std::endl;
        return;
    }
    std::cout << "RigidBodyComponent found on bird" << std::endl;

    // Initialize the RigidBodyComponent
    rigidBody->init();

    std::cout << "Calling createSlingJoint()..." << std::endl;
    createSlingJoint();

    std::cout << "BirdLauncherComponent::spawnBird() completed" << std::endl;
}

void BirdLauncherComponent::createSlingJoint() {
    if (!m_bird) {
        std::cout << "Error: No bird to attach sling joint to." << std::endl;
        return;
    }

    auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
    if (!rigidBody) {
        std::cout << "Error: RigidBodyComponent not found on bird." << std::endl;
        return;
    }
    rigidBody->toggleGravity(false);
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

    b2DistanceJointDef jointDef;
    jointDef.Initialize(birdBody, anchorBody, birdBody->GetPosition(), anchorBody->GetPosition());
    jointDef.collideConnected = true;
    jointDef.stiffness = 0.01f;
    jointDef.damping = 25.0f;

    // Create the joint
    m_slingJoint = (b2DistanceJoint*)m_world->GetWorld()->CreateJoint(&jointDef);

    if (m_slingJoint == nullptr) {
        std::cout << "Error: Failed to create sling joint." << std::endl;
    }
    else {
        std::cout << "Sling joint created successfully." << std::endl;
    }
}
void BirdLauncherComponent::updateBirdPosition(const sf::Vector2f& mousePos) {

    if (!m_bird) return;

    auto transform = m_bird->getComponent<TransformComponent>();
    if (!transform) return;

    // Calculate the direction from anchor to mouse
    sf::Vector2f direction = m_anchorPosition - mousePos;

    // Limit the pull distance
    float currentDistance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (currentDistance > m_maxPullDistance) {
        direction *= (m_maxPullDistance / currentDistance);
    }

    // Set the bird's position
    transform->position = m_anchorPosition - direction;

    // Update the physics body position
    auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
    if (rigidBody && rigidBody->GetBody()) {
        rigidBody->GetBody()->SetTransform(b2Vec2(transform->position.x / 30.0f, transform->position.y / 30.0f), rigidBody->GetBody()->GetAngle());
    }

}
void BirdLauncherComponent::launchBird(const sf::Vector2f& releasePos) {
    if (!m_bird) return;

    auto rigidBody = m_bird->getComponent<RigidBodyComponent>();
    if (!rigidBody) return;

    // Calculate launch vector
    sf::Vector2f launchVector = m_anchorPosition - releasePos;
    float launchForce = std::min(launchVector.x * launchVector.x + launchVector.y * launchVector.y, 500.0f); 
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
    m_resetTimer->start();  
}
void BirdLauncherComponent::resetLauncher() {
    // Destroy the old bird
    if (m_bird) {
        m_bird->destroy();
        m_bird = nullptr;
    }

    // Spawn a new bird
    spawnBird();
    m_resetTimer->reset();
}

void BirdLauncherComponent::drawRope(sf::RenderWindow& window)
{
    if (!m_bird || !m_isDragging) return;

    auto transform = m_bird->getComponent<TransformComponent>();
    if (!transform) return;

    sf::Vertex line[] = {
        sf::Vertex(m_anchorPosition),
        sf::Vertex(transform->position)
    };

    line[0].color = sf::Color::Green;
    line[1].color = sf::Color::Red;

    window.draw(line, 2, sf::Lines);
}
