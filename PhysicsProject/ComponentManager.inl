#pragma once
#include "ComponentManager.h"
#include "GameObject.h"

template<typename T, typename... Args>
T* ComponentManager::addComponent(GameObject* gameObject, Args&&... args) {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = component.get();
    m_components[gameObject].emplace_back(std::move(component));
    return ptr;
}

template<typename T>
T* ComponentManager::getComponent(GameObject* gameObject) {
    auto it = m_components.find(gameObject);
    if (it != m_components.end()) {
        for (auto& component : it->second) {
            if (auto* ptr = dynamic_cast<T*>(component.get())) {
                return ptr;
            }
        }
    }
    return nullptr;
}
