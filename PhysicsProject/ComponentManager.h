#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include "Component.h"

class GameObject;

class ComponentManager {
public:
    static ComponentManager& getInstance();

    template<typename T, typename... Args>
    T* addComponent(GameObject* gameObject, Args&&... args);

    template<typename T>
    T* getComponent(GameObject* gameObject);

    void removeComponents(GameObject* gameObject);

private:
    ComponentManager() = default;
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    std::unordered_map<GameObject*, std::vector<std::unique_ptr<Component>>> m_components;
};

// Include template implementations
#include "ComponentManager.inl"
