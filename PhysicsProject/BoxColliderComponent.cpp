#pragma once
/*
#include "Component.h"
#include <SFML/Graphics.hpp>

BoxColliderComponent::BoxColliderComponent(TransformComponent* transform, float width, float height)
    : m_transform(transform), m_width(width), m_height(height), m_visible(true), m_fixtureCreated(false) {
    updateSFMLShape();
}


void BoxColliderComponent::createBox2dShape() {
    auto rigidBody = m_transform->getOwner()->getComponent<RigidBodyComponent>();
    if (!rigidBody) {
        std::cout << "RigidBodyComponent not found, deferring fixture creation" << std::endl;
        return;
    }

    b2Body* body = rigidBody->GetBody();
    if (!body) {
        std::cout << "b2Body not created yet, deferring fixture creation" << std::endl;
        return;
    }

    b2PolygonShape shape;
    shape.SetAsBox(m_width / 2, m_height / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;

    body->CreateFixture(&fixtureDef);
    m_fixtureCreated = true;
}

sf::FloatRect BoxColliderComponent::getBounds() const {
    float scaledWidth = m_width * m_transform->scale.x;
    float scaledHeight = m_height * m_transform->scale.y;
    return sf::FloatRect(
        m_transform->position.x - scaledWidth / 2,
        m_transform->position.y - scaledHeight / 2,
        scaledWidth,
        scaledHeight
    );
}

void BoxColliderComponent::update(float deltaTime) {
    if (!m_fixtureCreated) {
        createBox2dShape();
    }
    updateSFMLShape();
}

bool BoxColliderComponent::intersects(const BoxColliderComponent& other) const {
    return getBounds().intersects(other.getBounds());
}

void BoxColliderComponent::setSize(float width, float height) {
    m_width = width;
    m_height = height;
    createBox2dShape();
    updateSFMLShape();
}


void BoxColliderComponent::setVisible(bool visible) {
    m_visible = visible;
}

void BoxColliderComponent::draw(sf::RenderWindow& window) {
    if (m_visible) {
        window.draw(m_shape);
    }
}


void BoxColliderComponent::onCollision(GameObject* other) {
     std::cout << "Collision detected between " << getOwner()->m_name<< " and " << other->m_name << std::endl;
    getOwner()->OnCollision(other);
}

void BoxColliderComponent::updateSFMLShape() {
    sf::FloatRect bounds = getBounds();
    m_shape.setSize(sf::Vector2f(bounds.width, bounds.height));
    m_shape.setOrigin(bounds.width / 2, bounds.height / 2);
    m_shape.setPosition(m_transform->position);
    m_shape.setRotation(m_transform->rotation);
    m_shape.setFillColor(sf::Color::Transparent);
    m_shape.setOutlineColor(sf::Color::Green);
    m_shape.setOutlineThickness(2);
}
*/