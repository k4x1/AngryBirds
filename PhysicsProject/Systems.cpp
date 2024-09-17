#include "Systems.h"
#include <cmath>
#include "EventSystem.h"
void RenderSystem::update(sf::RenderWindow& window) {
    for (auto& gameObject : GameObject::getAllObjects()) {
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

        // Draw box collider
        auto boxCollider = gameObject->getComponent<BoxColliderComponent>();
        if (boxCollider) {
            boxCollider->draw(window);
        }
    }
}

void PhysicsSystem::update(float deltaTime) {
    std::vector<GameObject*> collidableObjects;

    // First pass: update positions and collect collidable objects
    for (auto& gameObject : GameObject::getAllObjects()) {
        auto transform = gameObject->getComponent<TransformComponent>();
        auto rigidBody = gameObject->getComponent<RigidBodyComponent>();
        auto collider = gameObject->getComponent<BoxColliderComponent>();

        if (transform && rigidBody && collider) {
            rigidBody->update(gameObject, deltaTime);
            collidableObjects.push_back(gameObject);
        }
    }

    // Second pass: check and resolve collisions
    for (size_t i = 0; i < collidableObjects.size(); ++i) {
        for (size_t j = i + 1; j < collidableObjects.size(); ++j) {
            auto collider1 = collidableObjects[i]->getComponent<BoxColliderComponent>();
            auto collider2 = collidableObjects[j]->getComponent<BoxColliderComponent>();

            if (collider1->intersects(*collider2)) {
                resolveCollision(collidableObjects[i], collidableObjects[j]);
            }
        }

        // Check floor collision
        auto transform = collidableObjects[i]->getComponent<TransformComponent>();
        auto rigidBody = collidableObjects[i]->getComponent<RigidBodyComponent>();
        auto collider = collidableObjects[i]->getComponent<BoxColliderComponent>();
        checkFloorCollision(transform, rigidBody, collider);
    }
}

void PhysicsSystem::resolveCollision(GameObject* obj1, GameObject* obj2) {
    auto transform1 = obj1->getComponent<TransformComponent>();
    auto rigidBody1 = obj1->getComponent<RigidBodyComponent>();
    auto collider1 = obj1->getComponent<BoxColliderComponent>();

    auto transform2 = obj2->getComponent<TransformComponent>();
    auto rigidBody2 = obj2->getComponent<RigidBodyComponent>();
    auto collider2 = obj2->getComponent<BoxColliderComponent>();

    sf::FloatRect bounds1 = collider1->getBounds();
    sf::FloatRect bounds2 = collider2->getBounds();

    // Calculate collision normal
    sf::Vector2f center1(bounds1.left + bounds1.width / 2, bounds1.top + bounds1.height / 2);
    sf::Vector2f center2(bounds2.left + bounds2.width / 2, bounds2.top + bounds2.height / 2);
    sf::Vector2f normal = center2 - center1;
    float distance = std::sqrt(normal.x * normal.x + normal.y * normal.y);
    normal /= distance;

    // Calculate overlap
    float overlap = (bounds1.width + bounds2.width) / 2 - std::abs(center1.x - center2.x);
    overlap = std::min(overlap, (bounds1.height + bounds2.height) / 2 - std::abs(center1.y - center2.y));

    // Calculate relative velocity
    sf::Vector2f relativeVelocity = rigidBody2->velocity - rigidBody1->velocity;

    // Calculate relative velocity along the normal
    float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

    // Do not resolve if velocities are separating
    if (velocityAlongNormal > 0)
        return;


    float resitution = std::min(rigidBody1->restitution, rigidBody2->restitution);

    float impulseScalar = -(1 + resitution) * velocityAlongNormal;
    impulseScalar /= 1 / rigidBody1->mass + 1 / rigidBody2->mass;

    sf::Vector2f impulse = impulseScalar * normal;
    rigidBody1->velocity -= 1 / rigidBody1->mass * impulse;
    rigidBody2->velocity += 1 / rigidBody2->mass * impulse;

   
    const float percent = 1.0f; 
    const float slop = 0.1f; 
    sf::Vector2f correction = std::max(overlap - slop, 0.0f) / (1 / rigidBody1->mass + 1 / rigidBody2->mass) * percent * normal;
    transform1->position -= 1 / rigidBody1->mass * correction;
    transform2->position += 1 / rigidBody2->mass * correction;

    // trigger collision callbacks 
    if (collider1->onCollision) {
        collider1->onCollision(obj2);
    }
    if (collider2->onCollision) {
        collider2->onCollision(obj1);
    }
}



void PhysicsSystem::checkFloorCollision(TransformComponent* transform, RigidBodyComponent* rigidBody, BoxColliderComponent* collider) {
    float floorY = 500.0f;

    sf::FloatRect bounds = collider->getBounds();
    if (bounds.top + bounds.height > floorY) {
        transform->position.y = floorY - bounds.height / 2;
        rigidBody->velocity.y = -rigidBody->velocity.y * rigidBody->restitution;
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