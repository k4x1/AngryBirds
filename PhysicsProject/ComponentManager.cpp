#include "ComponentManager.h"

ComponentManager& ComponentManager::getInstance() {
    static ComponentManager instance;
    return instance;
}

std::vector<Component*> ComponentManager::getAllComponents(GameObject* gameObject)
{
    std::vector<Component*> components;
    auto it = m_components.find(gameObject);
    if (it != m_components.end()) {
        for (auto& component : it->second) {
            components.push_back(component.get());
        }
    }
    return components;
}

void ComponentManager::removeComponents(GameObject* gameObject) {
    m_components.erase(gameObject);
}
