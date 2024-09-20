#pragma once
#include "Component.h"
#include "Box2d/box2d.h"

TransformComponent::TransformComponent(float x, float y) : position(x, y), scale(0.5f, 0.5f), rotation(0.0f), rigidBody(nullptr) {}

void TransformComponent::setPosition(float x, float y) {
    position = sf::Vector2f(x, y);
    updateBox2DBody();
}

void TransformComponent::setRotation(float angle) {
    rotation = angle;
    updateBox2DBody();
}

void TransformComponent::setScale(float scaleX, float scaleY) {
    scale = sf::Vector2f(scaleX, scaleY);
    updateBox2DBody();
}

void TransformComponent::updateBox2DBody() {
    if (!rigidBody) {
        rigidBody = getOwner()->getComponent<RigidBodyComponent>();
    }
    if (!rigidBody || !rigidBody->GetBody()) return;

    b2Vec2 pos(position.x / 30.0f, position.y / 30.0f);
    rigidBody->GetBody()->SetTransform(pos, rotation * b2_pi / 180.0f);
}
