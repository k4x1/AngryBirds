#pragma once
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Component.h"

class RenderSystem {
public:
    void update(sf::RenderWindow& window);
};

class PhysicsSystem {
public:
    void update(float deltaTime);
};
