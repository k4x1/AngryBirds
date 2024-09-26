#pragma once

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "LevelManager.h"

enum class SceneType {
    MAIN_MENU,
    LEVEL_1,
    LEVEL_2,
    BOSS_FIGHT,
    GAME_OVER
};

class Box2DWorld;
class RenderSystem;
class PhysicsSystem;
class EventSystem;

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

    void showLoseScreen();
    void showGameCompleteScreen();
    void retryLevel();
    void checkLevelCompletion();
    void initializeLevels();
    LevelManager& getLevelManager() { return *m_levelManager; }
    void createScene(SceneType scene);

private:
    void update(float deltaTime);
    void draw();
    void handleInput();
    void checkGameOver();
    void createLoseScreen();
    void destroyLoseScreen();
    void createGameCompleteScreen();

    bool m_isLoseScreenActive;
    bool m_isGameCompleteScreenActive;

    LevelManager* m_levelManager;
    sf::RenderWindow m_window;
    SceneType m_currentScene;
    RenderSystem* m_renderSystem;
    PhysicsSystem* m_physicsSystem;
    EventSystem* m_eventSystem;
    GameObject* m_bird;

 
    sf::Font m_font;
    GameObject* m_loseTextObject;
    GameObject* m_retryButtonObject;
};
