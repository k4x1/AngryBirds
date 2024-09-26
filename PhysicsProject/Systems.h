#pragma once
#ifndef SYSTEMS_H
#define SYSTEMS_H

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
const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 600.0f;
const float PIXELS_PER_METER = 30.0f;
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
    void createWall(float x1, float y1, float x2, float y2);
    Box2DWorld m_world;
    b2Body* m_groundBody;
};
#endif 