#include "Systems.h"

void RenderSystem::update(sf::RenderWindow& window) {
    for (auto& gameObject : GameObject::getAllObjects()) {
        auto transform = gameObject->getComponent<TransformComponent>();

        auto render = gameObject->getComponent<RenderComponent>();
        if (transform && render) {
            render->shape.setPosition(transform->position);
            render->shape.setFillColor(render->color);
            window.draw(render->shape);
        }

        auto spriteRenderer = gameObject->getComponent<SpriteRendererComponent>();
        if (transform && spriteRenderer) {
            spriteRenderer->getSprite().setPosition(transform->position);
            spriteRenderer->setScale(transform->scale.x, transform->scale.y);
            spriteRenderer->getSprite().setRotation(transform->rotation);
            window.draw(spriteRenderer->getSprite());
        }
    }
}


void PhysicsSystem::update(float deltaTime) {
    for (auto& gameObject : GameObject::getAllObjects()) {
        auto rigidBody = gameObject->getComponent<RigidBodyComponent>();
        if (rigidBody) {
            rigidBody->update(gameObject, deltaTime);
        }
    }
}

