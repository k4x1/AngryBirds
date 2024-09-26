#pragma once
#include <vector>

// forward declarations fixes things
class Game;
enum class SceneType;

class LevelManager {
public:
    LevelManager(Game* game);
    void addLevel(SceneType sceneType);
    void startCurrentLevel();
    void nextLevel();
    void retryCurrentLevel();
    bool isLastLevel() const;
    int getCurrentLevelIndex() const;

private:
    Game* m_game;
    std::vector<SceneType> m_levels;
    int m_currentLevelIndex;
};
