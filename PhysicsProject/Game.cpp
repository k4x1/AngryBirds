#include "Game.h"
#include "ComponentManager.h"
#include "Systems.h"
#include <algorithm>

Game::Game()
    : m_window(sf::VideoMode(800, 600), "GameObject Game"),
    m_currentScene(SceneType::MAIN_MENU),
    m_bird(nullptr) {

    m_window.setFramerateLimit(60);
    m_renderSystem = new RenderSystem();
    m_physicsSystem = new PhysicsSystem();
}

Game::~Game() {
    delete m_renderSystem;
    delete m_physicsSystem;
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        handleInput();
        update(deltaTime);
        draw();
    }
}
void Game::createScene(SceneType scene) {
    m_currentScene = scene;

    for (auto& object : GameObject::getAllObjects()) {
        object->destroy();
    }

    const std::vector<std::string> spritePaths = {
        "Sprites/chick",
        "Sprites/duck",
        "Sprites/parrot"
    };

    auto createBird = [&](const sf::Vector2f& position, const std::string& spritePath) {
        auto bird = GameObject::create(position, "bird");
        bird->addComponent<TransformComponent>(position.x, position.y);
        bird->addComponent<SpriteRendererComponent>(spritePath + ".png");
        bird->addComponent<RigidBodyComponent>(1.0f,1.0f);
        bird->addComponent<BoxColliderComponent>(bird->getComponent<TransformComponent>(), 50,50);
        return bird;
        };

    switch (scene) {
    case SceneType::MAIN_MENU:
    {
        createBird(sf::Vector2f(375, 275), spritePaths[0]);
        createBird(sf::Vector2f(200, 200), spritePaths[1]);
        createBird(sf::Vector2f(550, 350), spritePaths[2]);
    }
    break;
    case SceneType::LEVEL_1:
    {
        auto player = createBird(sf::Vector2f(100, 100), spritePaths[0]);


        auto enemy = createBird(sf::Vector2f(700, 100), spritePaths[1]);


        auto platform = GameObject::create(sf::Vector2f(375, 500), "platform");
        platform->addComponent<RenderComponent>(sf::Color::Green);

        auto coin = createBird(sf::Vector2f(400, 300), spritePaths[2]);


        m_bird = createBird(sf::Vector2f(200, 200), spritePaths[0]);

        auto fallingObject = createBird(sf::Vector2f(400, 0), spritePaths[1]);
    }
    break;
    case SceneType::LEVEL_2:
    {
        for (int i = 0; i < 5; ++i) {
            createBird(sf::Vector2f(100 + i * 150, 100 + (i % 2) * 100), spritePaths[i % 3]);
        }
    }
    break;
    case SceneType::BOSS_FIGHT:
    {
        auto boss = createBird(sf::Vector2f(400, 300), spritePaths[2]);
        boss->getComponent<TransformComponent>()->scale = sf::Vector2f(2.0f,2.0f);

        for (int i = 0; i < 4; ++i) {
            createBird(sf::Vector2f(100 + i * 200, 500), spritePaths[i % 2]);
        }
    }
    break;
    case SceneType::GAME_OVER:
    {
        auto gameOverObject = createBird(sf::Vector2f(375, 275), spritePaths[1]);

    }
    break;
    }
}


void Game::update(float deltaTime) {
    m_physicsSystem->update(deltaTime);
 
    for (auto& gameObject : GameObject::getAllObjects()) {
        gameObject->update(deltaTime);
        auto transform = gameObject->getComponent<TransformComponent>();
        auto rigidBody = gameObject->getComponent<RigidBodyComponent>();
        if (transform && rigidBody) {
          
            if (transform->position.y > 500) {
                transform->position.y = 500;
                rigidBody->velocity.y = 0;
            }
        }
    }

    auto& objects = GameObject::getAllObjects();
    objects.erase(std::remove_if(objects.begin(), objects.end(), [](GameObject* obj) {
        if (obj->isDestroyed()) {
            ComponentManager::getInstance().removeComponents(obj);
            delete obj;
            return true;
        }
        return false;
        }), objects.end());

    if (m_bird) {
        auto transform = m_bird->getComponent<TransformComponent>();
        if (transform) {
            transform->position.x += 1.0f * deltaTime;
        }
    }
}

void Game::draw() {
    m_window.clear();
    m_renderSystem->update(m_window);
    m_window.display();
}

void Game::handleInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            m_window.close();

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Num1:
                createScene(SceneType::MAIN_MENU);
                break;
            case sf::Keyboard::Num2:
                createScene(SceneType::LEVEL_1);
                break;
            case sf::Keyboard::Num3:
                createScene(SceneType::LEVEL_2);
                break;
            case sf::Keyboard::Num4:
                createScene(SceneType::BOSS_FIGHT);
                break;
            case sf::Keyboard::Num5:
                createScene(SceneType::GAME_OVER);
                break;
            }
        }
    }
}
