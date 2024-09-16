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
             for (auto& otherGameObject : GameObject::getAllObjects()) {
                 auto otherCollider = otherGameObject->getComponent<BoxColliderComponent>();
                 if (otherGameObject == gameObject ||!otherCollider) continue;
                 if (collider->intersects(*otherCollider)) {
                     handleCollision(gameObject, otherGameObject);
                 }
             }
             // Check floor collision
             checkFloorCollision(transform, rigidBody, collider);
         }
     }
 }



 void PhysicsSystem::checkFloorCollision(TransformComponent* transform, RigidBodyComponent* rigidBody, BoxColliderComponent* collider) {
    float floorY = 500.0f; 

    sf::FloatRect bounds = collider->getBounds();
    if (bounds.top + bounds.height > floorY) {

        transform->position.y = floorY - bounds.height / 2;
        rigidBody->velocity.y = 0;


    }
    
}
 void PhysicsSystem::handleCollision(GameObject* obj1, GameObject* obj2)
 {
     // Basic collision response
     std::cout << obj1->m_name +" collided with " +obj2->m_name << std::endl;
     auto rb1 = obj1->getComponent<RigidBodyComponent>();
     auto rb2 = obj2->getComponent<RigidBodyComponent>();

     if (rb1 && rb2) {
         // Simple elastic collision
         sf::Vector2f temp = rb1->velocity;
         rb1->velocity = rb2->velocity;
         rb2->velocity = temp;
     }

     // Trigger collision callbacks if set
     auto collider1 = obj1->getComponent<BoxColliderComponent>();
     auto collider2 = obj2->getComponent<BoxColliderComponent>();

     if (collider1 && collider1->onCollision) {
         collider1->onCollision(obj2);
     }
     if (collider2 && collider2->onCollision) {
         collider2->onCollision(obj1);
     }
 }


