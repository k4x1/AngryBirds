#include "GameObject.h"



std::vector<GameObject*> GameObject::s_allObjects;

GameObject* GameObject::create(const sf::Vector2f& position, std::string name) {
    auto obj = new GameObject(position);
    obj->m_name = name;
    s_allObjects.push_back(obj);
    return obj;
}


void GameObject::setPosition(const sf::Vector2f& position) {
    m_position = position;
}

const sf::Vector2f& GameObject::getPosition() const {
    return m_position;
}

void GameObject::update(float deltaTime) {}

void GameObject::draw(sf::RenderWindow& window) {}

std::vector<GameObject*>& GameObject::getAllObjects() {
    return s_allObjects;
}

void GameObject::destroy() {
    m_isDestroyed = true;
}

bool GameObject::isDestroyed() const {
    return m_isDestroyed;
}

GameObject::GameObject(const sf::Vector2f& position) : m_position(position), m_isDestroyed(false) {}
