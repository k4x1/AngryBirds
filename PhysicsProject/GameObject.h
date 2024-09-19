#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

// Forward declarations
class Component;
class ComponentManager;

class GameObject {
public:
    static GameObject* create(const sf::Vector2f& position, std::string name);

    void setPosition(const sf::Vector2f& position);
    const sf::Vector2f& getPosition() const;

    template<typename T, typename... Args>
    T* addComponent(Args&&... args);

    template<typename T>
    T* getComponent();

    virtual void update(float deltaTime);
    virtual void OnCollision(GameObject* other);
    virtual void draw(sf::RenderWindow& window);

    static std::vector<GameObject*>& getAllObjects();

    void destroy();
    bool isDestroyed() const;
    std::string getName();
    std::string m_name;
private:
    GameObject(const sf::Vector2f& position);

    sf::Vector2f m_position;
    bool m_isDestroyed;
    static std::vector<GameObject*> s_allObjects;
};

#include "GameObject.inl"
