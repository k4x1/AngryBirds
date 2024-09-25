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
    try {
        if (gameObject == nullptr) {
            std::cout << "Warning: Attempting to remove components from null GameObject" << std::endl;
            return;
        }
        auto it = m_components.find(gameObject);
        if (it != m_components.end()) {
            std::cout << "Removing " << it->second.size() << " components from " << gameObject->getName() << std::endl;
            m_components.erase(it);
        }
        else {
            std::cout << "No components found for " << gameObject->getName() << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << "Exception in removeComponents: " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "Unknown exception in removeComponents" << std::endl;
    }
}
