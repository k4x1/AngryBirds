#pragma once
#include "box2d/box2d.h"
class Box2DWorld {
public:
    Box2DWorld() : m_world(b2Vec2(0.0f, 9.8f)) {}
    void Step(float timeStep, int velocityIterations, int positionIterations) {
        m_world.Step(timeStep, velocityIterations, positionIterations);
    }
    b2Body* CreateBody(const b2BodyDef* def) {
        return m_world.CreateBody(def);
    }
    b2World* GetWorld() { return &m_world; }

private:
    b2World m_world;
};
