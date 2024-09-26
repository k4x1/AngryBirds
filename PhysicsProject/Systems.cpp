#pragma once
#include "Systems.h"
#include <cmath>
#include "EventSystem.h"







void RenderSystem::update(sf::RenderWindow& window) {
    for (auto& gameObject : GameObject::getAllObjects()) {
        drawGameObject(window, gameObject);
    }
}

void RenderSystem::drawGameObject(sf::RenderWindow& window, GameObject* gameObject) {
    auto transform = gameObject->getComponent<TransformComponent>();

    auto render = gameObject->getComponent<RenderComponent>();
    if (transform && render) {
        render->shape.setPosition(transform->position);
        render->shape.setScale(transform->scale);
        render->shape.setRotation(transform->rotation);
        render->shape.setFillColor(render->color);
        window.draw(render->shape);
    }

    auto spriteRenderer = gameObject->getComponent<SpriteRendererComponent>();
    if (transform && spriteRenderer) {
        spriteRenderer->updateTransformScale(transform->scale);
        spriteRenderer->getSprite().setPosition(transform->position);
        spriteRenderer->getSprite().setRotation(transform->rotation);
        window.draw(spriteRenderer->getSprite());
    }

    auto boxCollider = gameObject->getComponent<BoxColliderComponent>();
    if (boxCollider) {
        boxCollider->debugDraw(window);
    }

    auto circleCollider = gameObject->getComponent<CircleColliderComponent>();
    if (circleCollider) {
        circleCollider->debugDraw(window);
    }

    auto launcherRope = gameObject->getComponent<BirdLauncherComponent>();
    if (launcherRope) {
        launcherRope->drawRope(window);
    }

    auto textRenderer = gameObject->getComponent<TextRendererComponent>();
    if (textRenderer) {
        textRenderer->draw(window);
    }

    auto buttonComponent = gameObject->getComponent<ButtonComponent>();
    if (buttonComponent) {
        buttonComponent->draw(window);
    }
}
PhysicsSystem::PhysicsSystem() {
    // Create ground body
    b2BodyDef groundBodyDef;

    groundBodyDef.position.Set(0.0f, 20.0f);
    m_groundBody = m_world.CreateBody(&groundBodyDef);

    // Create walls
    createWall(0, 0, 0, SCREEN_HEIGHT / PIXELS_PER_METER);  // Left wall
    createWall(SCREEN_WIDTH / PIXELS_PER_METER, 0, SCREEN_WIDTH / PIXELS_PER_METER, SCREEN_HEIGHT / PIXELS_PER_METER);  // Right wall
    createWall(0, 0, SCREEN_WIDTH / PIXELS_PER_METER, 0);  // Top wall
    createWall(0, SCREEN_HEIGHT / PIXELS_PER_METER, SCREEN_WIDTH / PIXELS_PER_METER, SCREEN_HEIGHT / PIXELS_PER_METER);  // Bottom wall
}
void PhysicsSystem::createWall(float x1, float y1, float x2, float y2) {
    b2BodyDef wallBodyDef;
    wallBodyDef.position.Set(0, 0);
    b2Body* wallBody = m_world.CreateBody(&wallBodyDef);

    b2EdgeShape wallShape;
    wallShape.SetTwoSided(b2Vec2(x1, y1), b2Vec2(x2, y2));
    wallBody->CreateFixture(&wallShape, 0.0f);
}
void PhysicsSystem::update(float deltaTime) {
    m_world.Step(deltaTime, 6, 2);

    // Update GameObject positions based on Box2D simulation
    for (auto& gameObject : GameObject::getAllObjects()) {
        auto rigidBody = gameObject->getComponent<RigidBodyComponent>();
        auto transform = gameObject->getComponent<TransformComponent>();
        if (rigidBody && transform) {
             
            if (!rigidBody->GetBody()) {
                continue;
            }
            
            b2Body* body = rigidBody->GetBody();
            b2Vec2 position = body->GetPosition();
            float angle = body->GetAngle();
            transform->position = sf::Vector2f(position.x * 30.0f, position.y * 30.0f); // convert to pixels
            transform->rotation = angle * 180.0f / b2_pi;
        }
    }

    // Handle collisions
    for (b2Contact* contact = m_world.GetWorld()->GetContactList(); contact; contact = contact->GetNext()) {
        if (contact->IsTouching()) {
            resolveCollision(contact);
        }
    }

    
}
void PhysicsSystem::resolveCollision(b2Contact* contact) {

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    GameObject* objA = reinterpret_cast<GameObject*>(bodyA->GetUserData().pointer);
    GameObject* objB = reinterpret_cast<GameObject*>(bodyB->GetUserData().pointer);

    if (objA && objB) {
        auto colliderA = dynamic_cast<ICollider*>(objA->getComponent<ICollider>());
        auto colliderB = dynamic_cast<ICollider*>(objB->getComponent<ICollider>());

        if (colliderA) colliderA->onCollision(objB);
        if (colliderB) colliderB->onCollision(objA);
    } 

} 



EventSystem& EventSystem::getInstance() {
    static EventSystem instance;
    return instance;
}

void EventSystem::addListener(Component* component) {
    m_listeners.push_back(component);
}

void EventSystem::removeListener(Component* component) {
    m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), component), m_listeners.end());
}

void EventSystem::dispatchEvent(const sf::Event& event) {
    for (auto listener : m_listeners) {
        listener->handleEvent(event);
    }
}


