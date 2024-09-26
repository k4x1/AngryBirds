
#pragma once

#include "Component.h"
#include "Box2DWorld.h"
#include <SFML/System/Vector2.hpp>
#include "box2d/box2d.h"


RigidBodyComponent::RigidBodyComponent(Box2DWorld* world, float mass, float gravityScale, float restitution, float maxSpeed)
    : m_world(world), m_mass(mass), m_gravityScale(gravityScale), m_restitution(restitution), m_maxSpeed(maxSpeed), m_body(nullptr), m_gravityOn(true) {}

RigidBodyComponent::~RigidBodyComponent() {
    if (m_body && m_world) {
        m_world->GetWorld()->DestroyBody(m_body);
    }
}

void RigidBodyComponent::init(){

    createBody();
}

void RigidBodyComponent::update(float deltaTime) {
    if (!m_body) {
        createBody();
    }

    if (m_body) {
        b2Vec2 position = m_body->GetPosition();
        float angle = m_body->GetAngle();

        auto transform = getOwner()->getComponent<TransformComponent>();
        
        if (transform) {
            transform->position = sf::Vector2f(position.x * 30.0f, position.y * 30.0f);
            transform->rotation = angle * 180.0f / b2_pi;
        }
    }
}
void RigidBodyComponent::createBody() {
    if (m_body) return;

    auto transform = getOwner()->getComponent<TransformComponent>();
    if (!transform) {
        std::cout << "TransformComponent not found, deferring body creation for " << getOwner()->getName() << std::endl;
        return;
    }

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(transform->position.x / 30.0f, transform->position.y / 30.0f);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(getOwner());

    m_body = m_world->GetWorld()->CreateBody(&bodyDef);

    if (!m_body) {
        std::cout << "Failed to create b2Body for " << getOwner()->getName() << std::endl;
        return;
    }

    m_body->SetGravityScale(m_gravityScale);

    // Find and initialize the collider component
    auto circleCollider = getOwner()->getComponent<CircleColliderComponent>();
    if (circleCollider) {
        circleCollider->init();
    }
    else {
        auto boxCollider = getOwner()->getComponent<BoxColliderComponent>();
        if (boxCollider) {
            boxCollider->init();
        }
        else {
            std::cout << "No ColliderComponent found for " << getOwner()->getName() << std::endl;
        }
    }
}

void RigidBodyComponent::applyForce(const sf::Vector2f& force) {
    if (m_body) {
        m_body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
    }
}

void RigidBodyComponent::toggleGravity(bool turnedOn) {

    if (!m_body) {
        std::cout << "Error: m_body is null in toggleGravity" << std::endl;
        return;
    }
    if (turnedOn) {
        SetGravityScale(m_gravityScale);
    }
    else {
        SetGravityScale(0);
    }

}

void RigidBodyComponent::applyImpulse(const sf::Vector2f& impulse) {
    if (m_body) {
        m_body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), m_body->GetWorldCenter(), true);
    }
}


void RigidBodyComponent::setVelocity(const sf::Vector2f& velocity) {
    if (m_body) {
        m_body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
        return;
    }
    std::cout << "No body found" << std::endl;
}


sf::Vector2f RigidBodyComponent::getVelocity() const {
    if (m_body) {
        b2Vec2 vel = m_body->GetLinearVelocity();
        return sf::Vector2f(vel.x, vel.y);
    }
    return sf::Vector2f(0, 0);
}

void RigidBodyComponent::moveTowards(const sf::Vector2f& targetPosition, float speed) {
    if (!m_body) return;

    b2Vec2 currentPosition = m_body->GetPosition();
    b2Vec2 target(targetPosition.x / 30.0f, targetPosition.y / 30.0f);
    b2Vec2 direction = target - currentPosition;
    float distance = direction.Length();

    if (distance > 0) {
        direction.Normalize();
        b2Vec2 desiredVelocity = speed * direction;
        b2Vec2 currentVelocity = m_body->GetLinearVelocity();
        b2Vec2 steeringForce = desiredVelocity - currentVelocity;

        m_body->ApplyForceToCenter(steeringForce, true);

        // Apply max speed limit
        if (m_body->GetLinearVelocity().Length() > m_maxSpeed) {
            b2Vec2 limitedVelocity = m_body->GetLinearVelocity();
            limitedVelocity.Normalize();
            limitedVelocity *= m_maxSpeed;
            m_body->SetLinearVelocity(limitedVelocity);
        }
    }
}

float RigidBodyComponent::getSpeed() const {
    return m_body ? m_body->GetLinearVelocity().Length() : 0.0f;
}

b2Body* RigidBodyComponent::GetBody() { return m_body; }

float RigidBodyComponent::GetMass() const { return m_mass; }
float RigidBodyComponent::GetGravityScale() const { return m_gravityScale; }
float RigidBodyComponent::GetRestitution() const { return m_restitution; }
float RigidBodyComponent::GetMaxSpeed() const { return m_maxSpeed; }

void RigidBodyComponent::SetMass(float mass) {
    m_mass = mass;
    if (m_body) {
        b2MassData massData;
        m_body->GetMassData(&massData);
        massData.mass = mass;
        m_body->SetMassData(&massData);
    }
}

Box2DWorld* RigidBodyComponent::GetWorld()
{
    return m_world;
}

void RigidBodyComponent::SetGravityScale(float scale) {
    if (m_body) {
        m_body->SetGravityScale(scale);
    }
  
}

void RigidBodyComponent::SetRestitution(float restitution) {
    m_restitution = restitution;
    if (m_body) {
        for (b2Fixture* f = m_body->GetFixtureList(); f; f = f->GetNext()) {
            f->SetRestitution(restitution);
        }
    }
}

void RigidBodyComponent::SetMaxSpeed(float maxSpeed) {
    m_maxSpeed = maxSpeed;
}