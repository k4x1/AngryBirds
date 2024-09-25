#pragma once

#include <SFML/Graphics.hpp>
#include "GameObject.h"



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
    ~Game();  
    void run();
    Box2DWorld* GetPhysicsWorld() {
        if (m_physicsSystem == nullptr || m_physicsSystem->GetWorld() == nullptr) {
            std::cout << "Error: Physics world is null" << std::endl;
            // Handle the error appropriately, maybe throw an exception or return a default world
        }
        return m_physicsSystem->GetWorld();
    }
private:
    void createScene(SceneType scene);
    void update(float deltaTime);
    void draw();  
    void handleInput();

    sf::RenderWindow m_window;
    SceneType m_currentScene;
    RenderSystem* m_renderSystem;  
    PhysicsSystem* m_physicsSystem; 
    EventSystem* m_eventSystem;
    GameObject* m_bird;
};
