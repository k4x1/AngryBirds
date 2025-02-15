#include "Game.h"
#include "ComponentManager.h"
#include "Systems.h"
#include "EventSystem.h"
#include "LevelManager.h"
#include <algorithm>

Game::Game()
    : m_window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "GameObject Game"),
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
    initializeLevels();
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
        std::cout << object << std::endl;
    }
    m_isLoseScreenActive = false;
    m_isGameCompleteScreenActive = false;

    const std::vector<std::string> spritePaths = {
        "Sprites/chick",
        "Sprites/duck",
        "Sprites/parrot",
        "Sprites/dannyInRealLife"
    };

    auto createChick = [&](const sf::Vector2f& position, const std::string& spritePath) {
        auto bird = GameObject::create(position, "bird");
        bird->addComponent<TransformComponent>(position.x, position.y);
        bird->addComponent<SpriteRendererComponent>(spritePath + ".png");
        bird->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 1.0f);
         bird->addComponent<BoxColliderComponent>(1.0f,1.0f);
         bird->addComponent<DoubleMassAbility>();
  
        return  bird;
        };    
    auto createDuck = [&](const sf::Vector2f& position, const std::string& spritePath) {
        auto bird = GameObject::create(position, "bird");
        bird->addComponent<TransformComponent>(position.x, position.y);
        bird->addComponent<SpriteRendererComponent>(spritePath + ".png");
        bird->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 1.0f);
         bird->addComponent<BoxColliderComponent>(1.0f,1.0f);
         bird->addComponent<BoostAbility>();
  
        return  bird;
        };
    auto createParrot = [&](const sf::Vector2f& position, const std::string& spritePath) {
        auto bird = GameObject::create(position, "bird");
        bird->addComponent<TransformComponent>(position.x, position.y);
        bird->addComponent<SpriteRendererComponent>(spritePath + ".png");
        bird->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 1.0f);
        bird->addComponent<BoxColliderComponent>(1.0f, 1.0f);
        bird->addComponent<SplitAbility>();

        return  bird;
        };
    auto createPig= [&](const sf::Vector2f& position, const std::string& spritePath  = "Sprites/dannyInRealLife") {

        auto pig = GameObject::create(position, "pig");

        auto transform = pig->addComponent<TransformComponent>(position.x, position.y);
        transform->setScale(1.0f, 1.0f);

        pig->addComponent<SpriteRendererComponent>(spritePath + ".png");

        pig->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 1.0f);

        pig->addComponent<CircleColliderComponent>(1.0f, sf::Vector2f(30, 35));

        pig->addComponent<BreakableComponent>(30);

        pig->addComponent<PigComponent>();
        std::cout << "Pig creation completed" << std::endl;
        return pig;
        };
    auto createPlatform = [&](const sf::Vector2f& position, const sf::Vector2f& size = sf::Vector2f(1,1)) {
        auto plat = GameObject::create(position, "platform");
        plat->addComponent<TransformComponent>(position.x, position.y);
        plat->getComponent<TransformComponent>()->setScale(size.x, size.y);
        plat->addComponent<SpriteRendererComponent>("Sprites/ground.png");

        plat->addComponent<RigidBodyComponent>(GetPhysicsWorld(), 1.0f, 1.0f);
        plat->addComponent<BoxColliderComponent>(size.x, size.y);
        plat->addComponent<BreakableComponent>(30);
        return plat;
        };

    auto createLauncher = [&](const float x, const float y, const std::function<GameObject* (const sf::Vector2f&, const std::string&)>& birdCreator, const std::string& spritePath) {
        auto position = sf::Vector2f(x, y);
        auto launcher = GameObject::create(position, "launcher");
        launcher->addComponent<TransformComponent>(x, y);
        launcher->addComponent<RenderComponent>(sf::Color::Red);
        launcher->addComponent<BirdLauncherComponent>(&m_window, GetPhysicsWorld(), position, birdCreator, spritePath);
        };

    switch (scene) {
    case SceneType::MAIN_MENU:
    {
      
     
  
       auto danny =  createPig(sf::Vector2f(550, 350));
   
    }
    break;
    case SceneType::LEVEL_1:
    {
      
        createLauncher(200, 450, createChick, spritePaths[0]);

        createPlatform(sf::Vector2f(400, 550));
        createPlatform(sf::Vector2f(400, 475));
        createPlatform(sf::Vector2f(400, 425)); 
        
  



        

        createPig(sf::Vector2f(633, 500));
   
    }
    break;
    case SceneType::LEVEL_2:
    {
        createLauncher(200, 500, createDuck, spritePaths[1]);
        createPig(sf::Vector2f(700, 375));
        createPlatform(sf::Vector2f(700, 550));
        createPlatform(sf::Vector2f(700, 475));
        createPlatform(sf::Vector2f(700, 425));
    }
    break;
    case SceneType::BOSS_FIGHT:
    {
        createLauncher(200, 500, createParrot, spritePaths[2]);
        createPig(sf::Vector2f(550, 375));
        createPlatform(sf::Vector2f(550, 550));
        createPlatform(sf::Vector2f(550, 475));
        createPlatform(sf::Vector2f(550, 425));

        createPig(sf::Vector2f(625, 550));
        createPig(sf::Vector2f(700, 375));
        createPlatform(sf::Vector2f(700, 550));
        createPlatform(sf::Vector2f(700, 475));
        createPlatform(sf::Vector2f(700, 425));
    }
    break;
    }
    for (size_t i = 0; i < GameObject::getAllObjects().size(); ++i) {
        auto& gameObject = GameObject::getAllObjects()[i];
        if (gameObject == nullptr) {
            std::cout << "Error: Null game object encountered at index " << i << std::endl;
            continue;
        }

        try {
            gameObject->start();         
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
    if (m_isLoseScreenActive) {
        // Update only lose screen objects
        if (m_loseTextObject) {
            m_loseTextObject->update(deltaTime);
        }
        if (m_retryButtonObject) {
            m_retryButtonObject->update(deltaTime);
        }
    }
    else {
        // Normal game update
        m_physicsSystem->update(deltaTime);

        checkGameOver();

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

}

void Game::draw() {
    m_window.clear();

    if (m_isLoseScreenActive) {
        // Only draw lose screen objects
        if (m_loseTextObject) m_renderSystem->drawGameObject(m_window, m_loseTextObject);
        if (m_retryButtonObject) m_renderSystem->drawGameObject(m_window, m_retryButtonObject);
    }
    else {
        m_renderSystem->update(m_window);
    }

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
            }
        }
    }
}
void Game::showLoseScreen() {
    createLoseScreen();
}
void Game::showGameCompleteScreen() {
    m_isGameCompleteScreenActive = true;
    createGameCompleteScreen();
}

void Game::retryLevel() {
    destroyLoseScreen();
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
        if (m_levelManager->isLastLevel()) {
            showGameCompleteScreen();
        }
        else {
            m_levelManager->nextLevel();
        }
    }
}


void Game::initializeLevels() {
    m_levelManager->addLevel(SceneType::LEVEL_1);
    m_levelManager->addLevel(SceneType::LEVEL_2);
    m_levelManager->addLevel(SceneType::BOSS_FIGHT);

    m_levelManager->nextLevel();
}


void Game::createLoseScreen() {
    m_isLoseScreenActive = true;

    // Load font
    if (!m_font.loadFromFile("font.ttf")) {
        std::cout << "Error loading font" << std::endl;
        return;
    }

    // Create "You Lose" text
    m_loseTextObject = GameObject::create(sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50), "loseText");
    auto textTransform = m_loseTextObject->addComponent<TransformComponent>(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50);
    auto textRenderer = m_loseTextObject->addComponent<TextRendererComponent>("You Lose!");
    textRenderer->setFont(m_font);
    textRenderer->setCharacterSize(48);
    textRenderer->setFillColor(sf::Color::Red);
    textRenderer->setOrigin(TextOrigin::Center);

    // Create retry button
    m_retryButtonObject = GameObject::create(sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 ), "retryButton");
    auto buttonTransform = m_retryButtonObject->addComponent<TransformComponent>(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 +50);
    auto buttonRenderer = m_retryButtonObject->addComponent<ButtonComponent>("Retry", [this]() { this->retryLevel(); });
    buttonRenderer->setFont(m_font);
    buttonRenderer->setCharacterSize(24);
    buttonRenderer->setButtonColor(sf::Color(100, 100, 100));
    buttonRenderer->setButtonColor(sf::Color(100, 100, 100));
    buttonRenderer->setTextColor(sf::Color::White);
    buttonRenderer->setButtonSize(sf::Vector2f(120, 40));
}
void Game::destroyLoseScreen() {
    m_isLoseScreenActive = false;
    if (m_loseTextObject) {
        m_loseTextObject->destroy();
        m_loseTextObject = nullptr;
    }
    if (m_retryButtonObject) {
        m_retryButtonObject->destroy();
        m_retryButtonObject = nullptr;
    }
}
void Game::createGameCompleteScreen() {
    // Clear existing objects
    for (auto& object : GameObject::getAllObjects()) {
        object->destroy();
    }

    // Create "Game Complete" text
    auto completeText = GameObject::create(sf::Vector2f(600, 1000), "completeText");
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
    }

  
    if (!hasBirds) {
        showLoseScreen();
    }
}