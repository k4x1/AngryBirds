#include "GameObject.h"



std::vector<GameObject*> GameObject::s_allObjects;

GameObject* GameObject::create(const sf::Vector2f& position, std::string name) {
    auto obj = new GameObject(position);
    obj->m_name = name;
    if (obj->m_name.empty()) {
        std::cout << "Warning: Empty name provided for GameObject" << std::endl;
        obj->m_name = "Unnamed Object";
    }
    s_allObjects.push_back(obj);
    std::cout << "Created GameObject: " << obj->getName() << ", Total objects: " << s_allObjects.size() << std::endl;
    return obj;
}
GameObject::~GameObject() {
    try {
        std::cout << "Destroying GameObject: " << m_name << std::endl;
        // Clean up any resources, if necessary
    }
    catch (const std::exception& e) {
        std::cout << "Exception in GameObject destructor: " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "Unknown exception in GameObject destructor" << std::endl;
    }
}

void GameObject::setPosition(const sf::Vector2f& position) {
    auto transform = getComponent<TransformComponent>();
    if (transform) {
        transform->setPosition(position.x, position.y);
        m_position = position;
    }
    
}

const sf::Vector2f& GameObject::getPosition() const {
    return m_position;
}

void GameObject::start() {
    std::cout << "Starting components for: " << (m_name.empty() ? "Unnamed Object" : m_name) << std::endl;
    auto& componentManager = ComponentManager::getInstance();
    auto components = componentManager.getAllComponents(this);
    std::cout << "Number of components: " << components.size() << std::endl;
    for (auto& component : components) {
        if (component == nullptr) {
            std::cout << "Error: Null component encountered" << std::endl;
            continue;
        }
        std::cout << "Starting component: " << typeid(*component).name() << std::endl;
        component->start();
        std::cout << "Started component: " << typeid(*component).name() << std::endl;
    }
    std::cout << "Finished starting components for: " << (m_name.empty() ? "Unnamed Object" : m_name) << std::endl;
}


void GameObject::update(float deltaTime) {
    for (auto& component : ComponentManager::getInstance().getAllComponents(this)) {
        component->update(deltaTime);
    }
}

void GameObject::OnCollision(GameObject* other)
{
    for (auto& component : ComponentManager::getInstance().getAllComponents(this)) {
        auto* collider = dynamic_cast<ICollider*>(component);
        if (!collider) {
            component->onCollision(other);    
        }
    }
}


void GameObject::draw(sf::RenderWindow& window) {}

std::vector<GameObject*>& GameObject::getAllObjects() {
    return s_allObjects;
}

void GameObject::destroy() {
    m_isDestroyed = true;
}

bool GameObject::isDestroyed() const {
    try {
        return m_isDestroyed;
    }
    catch (...) {
        std::cout << "Exception in isDestroyed() for object: " << (this ? m_name : "Unknown") << std::endl;
        return true; // Assume it's destroyed if we can't check safely
    }
}

std::string GameObject::getName()
{
    return "Name is bugged so this is all you get";

}

GameObject::GameObject(const sf::Vector2f& position) : m_position(position), m_isDestroyed(false) {}
