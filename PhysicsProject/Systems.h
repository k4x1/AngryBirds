// Systems.h
#pragma once
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Component.h"
#include <vector>
#include "EventSystem.h"
#include "Box2DWorld.h"

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
    PhysicsSystem();
    void update(float deltaTime);
    void resolveCollision(b2Contact* contact);


    Box2DWorld* GetWorld() { return &m_world; }

private:
    Box2DWorld m_world;
    b2Body* m_groundBody;
};