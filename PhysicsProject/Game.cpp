#include "Game.h"
#include "ComponentManager.h"
#include "Systems.h"
#include "EventSystem.h"
#include "LevelManager.h"
#include <algorithm>

Game::Game()
    : m_window(sf::VideoMode(800, 600), "GameObject Game"),
    m_currentScene(SceneType::MAIN_MENU),
    m_isLoseScreenActive(false),
    m_isGameCompleteScreenActive(false),
    m_bird(nullptr)
{   
    m_levelManager = new LevelManager(this);
    m_window.setFramerateLimit(60);
    m_renderSystem = new RenderSystem();
    m_physicsSystem = new PhysicsSystem();
    m_eventSystem = &EventSystem::getInstance();
}

Game::~Game() {
    delete m_levelManager;
    delete m_renderSystem;
    delete m_physicsSystem;
}


void Game::run() {
    sf::Clock clock;
    if (!m_physicsSystem || !m_physicsSystem->GetWorld()) {
        std::cout << "Error: Physics world not properly initialized" << std::endl;
        return;
    }
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
    m_isLoseScreenActive = false;
    m_isGameCompleteScreenActive = false;

    const std::vector<std::string> spritePaths = {
        "Sprites/chick",
        "Sprites/duck",
        "Sprites/parrot",
        "Sprites/dannyInRealLife"
    };

    auto createBird = [&](const sf::Vector2f& position, const std::string& spritePath) {
        auto bird = GameObject::create(position, "bird");
        bird->addComponent<TransformComponent>(position.x, position.y);
        bird->addComponent<SpriteRendererComponent>(spritePath + ".png");
        bird->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 1.0f);
         bird->addComponent<BoxColliderComponent>(1.0f,1.0f);
         bird->addComponent<DoubleMassAbility>();
  
        return  bird;
        };
    auto createPig= [&](const sf::Vector2f& position, const std::string& spritePath  = "Sprites/dannyInRealLife") {

        auto pig = GameObject::create(position, "pig");

        auto transform = pig->addComponent<TransformComponent>(position.x, position.y);
        transform->setScale(1.5, 1.5);

        pig->addComponent<SpriteRendererComponent>(spritePath + ".png");

        pig->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 1.0f);

        pig->addComponent<CircleColliderComponent>(1.0f, sf::Vector2f(46, 48));

        pig->addComponent<BreakableComponent>(20);

        pig->addComponent<PigComponent>();
        std::cout << "Pig creation completed" << std::endl;
        return pig;
        };
    auto createPlatform = [&](const sf::Vector2f& position, const sf::Vector2f& size, const sf::Color color) {
        auto plat = GameObject::create(position, "platform");
        plat->addComponent<TransformComponent>(position.x, position.y);
        plat->getComponent<TransformComponent>()->setScale(size.x, size.y);
        plat->addComponent<RenderComponent>(color);
        plat->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 0.0f);
        plat->addComponent<BoxColliderComponent>(size.x, size.y);
        plat->addComponent<BreakableComponent>(20);
        return plat;
        };


    switch (scene) {
    case SceneType::MAIN_MENU:
    {
      
        createBird(sf::Vector2f(375, 275), spritePaths[0]);
        createBird(sf::Vector2f(200, 200), spritePaths[1]);
  
       auto danny =  createPig(sf::Vector2f(550, 350));
   
    }
    break;
    case SceneType::LEVEL_1:
    {

        auto launcher = GameObject::create(sf::Vector2f(300, 300), "launcher");
        launcher->addComponent<TransformComponent>(300, 300);
        launcher->addComponent<RenderComponent>(sf::Color::Red);
        launcher->addComponent<BirdLauncherComponent>(&m_window, GetPhysicsWorld(), sf::Vector2f(300, 300), createBird, spritePaths[2]);



        createPig(sf::Vector2f(233, 233));
   
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
        boss->addComponent<FollowMouseComponent>(&m_window);
      

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
    for (size_t i = 0; i < GameObject::getAllObjects().size(); ++i) {
        auto& gameObject = GameObject::getAllObjects()[i];
        if (gameObject == nullptr) {
            std::cout << "Error: Null game object encountered at index " << i << std::endl;
            continue;
        }

        std::cout << "Starting object at index " << i << std::endl;
        try {
            std::cout << "Object name: " << gameObject->getName() << std::endl;
            gameObject->start();
            std::cout << "Started object: " << gameObject->getName() << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Exception caught while starting object at index " << i << ": " << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown exception caught while starting object at index " << i << std::endl;
        }
    }


}


void Game::update(float deltaTime) {
    m_physicsSystem->update(deltaTime);
 

    auto& objects = GameObject::getAllObjects();
    for (auto& gameObject : objects) {
        gameObject->update(deltaTime);

    }
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

    if (!m_isLoseScreenActive && !m_isGameCompleteScreenActive) {
        checkLevelCompletion();
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

        m_eventSystem->dispatchEvent(event);

        if (event.type == sf::Event::Closed) {
            m_window.close();
        }

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
void Game::showLoseScreen() {
    m_isLoseScreenActive = true;
    createLoseScreen();
}

void Game::showGameCompleteScreen() {
    m_isGameCompleteScreenActive = true;
    createGameCompleteScreen();
}

void Game::retryLevel() {
    m_isLoseScreenActive = false;
    m_levelManager->retryCurrentLevel();
}

void Game::checkLevelCompletion() {
    int pigCount = 0;
    for (auto& object : GameObject::getAllObjects()) {
        if (object->getComponent<PigComponent>()) {
            pigCount++;
        }
    }

    if (pigCount == 0) {
        m_levelManager->nextLevel();
    }
}

void Game::initializeLevels() {
    m_levelManager->addLevel([this](Game* game) {
        this->createScene(SceneType::LEVEL_1);
        });

    m_levelManager->addLevel([this](Game* game) {
        this->createScene(SceneType::LEVEL_2);
        });

    m_levelManager->addLevel([this](Game* game) {
        this->createScene(SceneType::BOSS_FIGHT);
        });

    m_levelManager->nextLevel(); // start the first level
}

void Game::createLoseScreen() {
    // Clear existing objects
    for (auto& object : GameObject::getAllObjects()) {
        object->destroy();
    }

    // Create "You Lose" text
    auto loseText = GameObject::create(sf::Vector2f(400, 200), "loseText");
    auto textRenderer = loseText->addComponent<TextRendererComponent>("You Lose!");
    textRenderer->setCharacterSize(48);
    textRenderer->setFillColor(sf::Color::Red);

    // Create retry button
    auto retryButton = GameObject::create(sf::Vector2f(400, 300), "retryButton");
    auto buttonRenderer = retryButton->addComponent<ButtonComponent>("Retry", [this]() { this->retryLevel(); });
}

void Game::createGameCompleteScreen() {
    // Clear existing objects
    for (auto& object : GameObject::getAllObjects()) {
        object->destroy();
    }

    // Create "Game Complete" text
    auto completeText = GameObject::create(sf::Vector2f(400, 200), "completeText");
    auto textRenderer = completeText->addComponent<TextRendererComponent>("Game Complete!");
    textRenderer->setCharacterSize(48);
    textRenderer->setFillColor(sf::Color::Green);
}
void Game::checkGameOver() {
    bool hasBirds = false;
    bool hasLauncher = false;
    for (auto& object : GameObject::getAllObjects()) {
        if (object->getComponent<AbilityComponent>()) {
            hasBirds = true;
        }
        if (object->getComponent<BirdLauncherComponent>()) {
            hasLauncher = true;
        }
    }

    if (!hasBirds && !hasLauncher) {
        showLoseScreen();
    }
}