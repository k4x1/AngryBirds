#pragma once
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Component.h"
#include <vector>
#include "EventSystem.h"


class GameObject;
class TransformComponent;
class RigidBodyComponent;
class BoxColliderComponent;

class RenderSystem {
public:
    void update(sf::RenderWindow& window);
};

class PhysicsSystem {
public:
    void update(float deltaTime);
    void resolveCollision(GameObject* obj1, GameObject* obj2);
    void checkFloorCollision(TransformComponent* transform, RigidBodyComponent* rigidBody, BoxColliderComponent* collider);
};
