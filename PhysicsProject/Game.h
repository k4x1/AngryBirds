#pragma once

#include <SFML/Graphics.hpp>
#include "GameObject.h"

// Forward declarations
class RenderSystem;
class PhysicsSystem;

enum class SceneType {
    MAIN_MENU,
    LEVEL_1,
    LEVEL_2,
    BOSS_FIGHT,
    GAME_OVER
};

class Game {
public:
    Game();
    ~Game();  // Add a destructor
    void run();

private:
    void createScene(SceneType scene);
    void update(float deltaTime);
    void draw();
    void handleInput();

    sf::RenderWindow m_window;
    SceneType m_currentScene;
    RenderSystem* m_renderSystem;  // Change to pointer
    PhysicsSystem* m_physicsSystem;  // Change to pointer
    GameObject* m_bird;
};
