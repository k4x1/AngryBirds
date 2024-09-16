#include "Systems.h"

void RenderSystem::update(sf::RenderWindow& window) {
    for (auto& gameObject : GameObject::getAllObjects()) {
        auto transform = gameObject->getComponent<TransformComponent>();

\
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
    }
}



 void PhysicsSystem::update(float deltaTime) {
     for (auto& gameObject : GameObject::getAllObjects()) {
         auto transform = gameObject->getComponent<TransformComponent>();
         auto rigidBody = gameObject->getComponent<RigidBodyComponent>();
         auto collider = gameObject->getComponent<BoxColliderComponent>();

         if (transform && rigidBody && collider) {
             // Update RigidBody (which includes gravity)
             rigidBody->update(gameObject, deltaTime);

             // Check floor collision
             checkFloorCollision(transform, rigidBody, collider);
         }
     }
 }



 void PhysicsSystem::checkFloorCollision(TransformComponent* transform, RigidBodyComponent* rigidBody, BoxColliderComponent* collider) {
    float floorY = 500.0f; // Assuming the floor is at y = 500

    sf::FloatRect bounds = collider->getBounds();
    if (bounds.top + bounds.height > floorY) {
        // Collision with floor detected
        transform->position.y = floorY - bounds.height / 2;
        rigidBody->velocity.y = 0;

        // You can add additional logic here, such as setting an "onGround" flag
    }
    
};

