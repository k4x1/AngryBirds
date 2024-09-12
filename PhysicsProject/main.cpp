#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

// Enums
enum class SceneType {
    MAIN_MENU,
    LEVEL_1,
    LEVEL_2,
    BOSS_FIGHT,
    GAME_OVER
};

// Base Component class
class Component {
public:
    virtual ~Component() {}
};

// Forward declaration of GameObject
class GameObject;

// Component Manager
class ComponentManager {
public:
    static ComponentManager& getInstance() {
        static ComponentManager instance;
        return instance;
    }

    template<typename T, typename... Args>
    T* addComponent(GameObject* gameObject, Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        m_components[gameObject].emplace_back(std::move(component));
        return ptr;
    }

    template<typename T>
    T* getComponent(GameObject* gameObject) {
        auto& components = m_components[gameObject];
        for (auto& component : components) {
            if (dynamic_cast<T*>(component.get())) {
                return dynamic_cast<T*>(component.get());
            }
        }
        return nullptr;
    }

    void removeComponents(GameObject* gameObject) {
        m_components.erase(gameObject);
    }

private:
    ComponentManager() {}
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    std::unordered_map<GameObject*, std::vector<std::unique_ptr<Component>>> m_components;
};

// GameObject class
class GameObject {
public:
    static GameObject* create(const sf::Vector2f& position) {
        auto obj = new GameObject(position);
        s_allObjects.push_back(obj);
        return obj;
    }

    void setPosition(const sf::Vector2f& position) {
        m_position = position;
    }

    const sf::Vector2f& getPosition() const {
        return m_position;
    }

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        return ComponentManager::getInstance().addComponent<T>(this, std::forward<Args>(args)...);
    }

    template<typename T>
    T* getComponent() {
        return ComponentManager::getInstance().getComponent<T>(this);
    }

    virtual void update(float deltaTime) {}
    virtual void draw(sf::RenderWindow& window) {}

    static std::vector<GameObject*>& getAllObjects() {
        return s_allObjects;
    }

    void destroy() {
        m_isDestroyed = true;  // Mark the object for deletion
    }

    bool isDestroyed() const {
        return m_isDestroyed;
    }

private:
    GameObject(const sf::Vector2f& position) : m_position(position), m_isDestroyed(false) {}

    sf::Vector2f m_position;
    bool m_isDestroyed;  // Flag to mark the object for deletion
    static std::vector<GameObject*> s_allObjects;
};

std::vector<GameObject*> GameObject::s_allObjects;

// Example components
class TransformComponent : public Component {
public:
    TransformComponent(float x, float y) : position(x, y) {}

    sf::Vector2f position;
};

class RenderComponent : public Component {
public:
    RenderComponent(sf::Color color) : shape(sf::Vector2f(50, 50)), color(color) {}

    sf::RectangleShape shape;
    sf::Color color;
};

// Example system
class RenderSystem {
public:
    void update(sf::RenderWindow& window) {
        for (auto& gameObject : GameObject::getAllObjects()) {
            auto transform = gameObject->getComponent<TransformComponent>();
            auto render = gameObject->getComponent<RenderComponent>();
            if (transform && render) {
                render->shape.setPosition(transform->position);
                render->shape.setFillColor(render->color);
                window.draw(render->shape);
            }
        }
    }
};

// Game class
class Game {
public:
    Game();
    void run();

private:
    void createScene(SceneType scene);
    void update(float deltaTime);
    void draw();
    void handleInput();

    sf::RenderWindow m_window;
    SceneType m_currentScene;
    RenderSystem m_renderSystem;
    GameObject* m_bird; // Pointer to our bird GameObject
};

Game::Game()
    : m_window(sf::VideoMode(800, 600), "GameObject Game"),
    m_currentScene(SceneType::MAIN_MENU),
    m_bird(nullptr) {}

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

    // Clear all objects from the previous scene
    for (auto& object : GameObject::getAllObjects()) {
        object->destroy();
    }

    switch (scene) {
    case SceneType::MAIN_MENU:
        // Create menu objects
    {
        auto menuObject = GameObject::create(sf::Vector2f(375, 275));
        menuObject->addComponent<RenderComponent>(sf::Color::Green);
    }
    break;
    case SceneType::LEVEL_1:
        // Create level 1 objects
    {
        auto player = GameObject::create(sf::Vector2f(100, 100));
        player->addComponent<TransformComponent>(100, 100);
        player->addComponent<RenderComponent>(sf::Color::Blue);

        auto enemy = GameObject::create(sf::Vector2f(700, 100));
        enemy->addComponent<TransformComponent>(700, 100);
        enemy->addComponent<RenderComponent>(sf::Color::Red);

        auto platform = GameObject::create(sf::Vector2f(375, 500));
        platform->addComponent<RenderComponent>(sf::Color::Green);

        auto coin = GameObject::create(sf::Vector2f(400, 300));
        coin->addComponent<RenderComponent>(sf::Color::Yellow);

        // Create the bird GameObject
        m_bird = GameObject::create(sf::Vector2f(200, 200));
        m_bird->addComponent<TransformComponent>(200, 200);
        m_bird->addComponent<RenderComponent>(sf::Color::Cyan);
    }
    break;
    case SceneType::LEVEL_2:
        // Create level 2 objects
    {
        auto player = GameObject::create(sf::Vector2f(100, 100));
        player->addComponent<TransformComponent>(100, 100);
        player->addComponent<RenderComponent>(sf::Color::Blue);

        auto enemy1 = GameObject::create(sf::Vector2f(700, 100));
        enemy1->addComponent<TransformComponent>(700, 100);
        enemy1->addComponent<RenderComponent>(sf::Color::Red);

        auto enemy2 = GameObject::create(sf::Vector2f(700, 300));
        enemy2->addComponent<TransformComponent>(700, 300);
        enemy2->addComponent<RenderComponent>(sf::Color::Red);

        auto platform = GameObject::create(sf::Vector2f(375, 500));
        platform->addComponent<RenderComponent>(sf::Color::Green);

        auto coin1 = GameObject::create(sf::Vector2f(400, 300));
        coin1->addComponent<RenderComponent>(sf::Color::Yellow);

        auto coin2 = GameObject::create(sf::Vector2f(450, 300));
        coin2->addComponent<RenderComponent>(sf::Color::Yellow);

        // Create the bird GameObject
        m_bird = GameObject::create(sf::Vector2f(200, 200));
        m_bird->addComponent<TransformComponent>(200, 200);
        m_bird->addComponent<RenderComponent>(sf::Color::Cyan);
    }
    break;
    case SceneType::BOSS_FIGHT:
        // Create boss fight objects
    {
        auto player = GameObject::create(sf::Vector2f(100, 100));
        player->addComponent<TransformComponent>(100, 100);
        player->addComponent<RenderComponent>(sf::Color::Blue);

        auto boss = GameObject::create(sf::Vector2f(375, 275));
        boss->addComponent<TransformComponent>(375, 275);
        boss->addComponent<RenderComponent>(sf::Color::Red); // Boss

        auto platform = GameObject::create(sf::Vector2f(375, 500));
        platform->addComponent<RenderComponent>(sf::Color::Green);

        // Create the bird GameObject
        m_bird = GameObject::create(sf::Vector2f(200, 200));
        m_bird->addComponent<TransformComponent>(200, 200);
        m_bird->addComponent<RenderComponent>(sf::Color::Cyan);
    }
    break;
    case SceneType::GAME_OVER:
        // Create game over objects
    {
        auto gameOverObject = GameObject::create(sf::Vector2f(375, 275));
        gameOverObject->addComponent<RenderComponent>(sf::Color::Green);
    }
    break;
    }
}

void Game::update(float deltaTime) {
    // Update all objects
    for (auto& object : GameObject::getAllObjects()) {
        object->update(deltaTime);
    }

    // Deferred deletion: Remove and delete all objects marked for deletion
    auto& objects = GameObject::getAllObjects();
    objects.erase(std::remove_if(objects.begin(), objects.end(), [](GameObject* obj) {
        if (obj->isDestroyed()) {
            ComponentManager::getInstance().removeComponents(obj);
            delete obj;
            return true;
        }
        return false;
        }), objects.end());

    // You can now access and update the bird GameObject specifically
    if (m_bird) {
        auto transform = m_bird->getComponent<TransformComponent>();
        if (transform) {
            // Update bird's position or other properties
            transform->position.x += 1.0f * deltaTime;
        }
    }
}

void Game::draw() {
    m_window.clear();
    m_renderSystem.update(m_window);
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

int main() {
    Game game;
    game.run();
    // Clean up all GameObjects
    for (auto& object : GameObject::getAllObjects()) {
        delete object;
    }
    return 0;
}