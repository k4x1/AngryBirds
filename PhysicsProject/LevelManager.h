#pragma once
#include <vector>
#include <functional>

class Game;
class LevelManager {
public:
    using LevelSetupFunction = std::function<void(Game*)>;

    LevelManager(Game* game);
    void addLevel(LevelSetupFunction setupFunc);
    void startCurrentLevel();
    void nextLevel();
    void retryCurrentLevel();
    bool isLastLevel() const;
    int getCurrentLevelIndex() const;

private:
    Game* m_game;
    std::vector<LevelSetupFunction> m_levels;
    int m_currentLevelIndex;
};
