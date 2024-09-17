#pragma once
#include <SFML/Window/Event.hpp>
#include <vector>

class Component;

class EventSystem {
public:
    static EventSystem& getInstance();
    void addListener(Component* component);
    void removeListener(Component* component);
    void dispatchEvent(const sf::Event& event);

private:
    EventSystem() = default;
    std::vector<Component*> m_listeners;
};
