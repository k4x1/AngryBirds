#pragma once
#include "GameObject.h"
#include "ComponentManager.h"

template<typename T, typename... Args>
T* GameObject::addComponent(Args&&... args) {
    return ComponentManager::getInstance().addComponent<T>(this, std::forward<Args>(args)...);
}

template<typename T>
T* GameObject::getComponent() {
    return ComponentManager::getInstance().getComponent<T>(this);
}

