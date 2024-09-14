#include "ComponentManager.h"

ComponentManager& ComponentManager::getInstance() {
    static ComponentManager instance;
    return instance;
}

void ComponentManager::removeComponents(GameObject* gameObject) {
    m_components.erase(gameObject);
}
