#include "LevelManager.h"
#include "Game.h"

LevelManager::LevelManager(Game* game) : m_game(game), m_currentLevelIndex(-1) {}

void LevelManager::addLevel(SceneType sceneType) {
    m_levels.push_back(sceneType);
    std::cout << "Level added. Total levels: " << m_levels.size() << std::endl;
}

void LevelManager::startCurrentLevel() {
   
    if (m_currentLevelIndex >= 0 && m_currentLevelIndex < m_levels.size()) {
        m_game->createScene(m_levels[m_currentLevelIndex]);
    }
}


void LevelManager::nextLevel() {
    if (!isLastLevel()) {
        m_currentLevelIndex++;
        startCurrentLevel();
    }
    else {
        m_game->showGameCompleteScreen();
    }
}

void LevelManager::retryCurrentLevel() {
    startCurrentLevel();
}

bool LevelManager::isLastLevel() const {
    return m_currentLevelIndex == m_levels.size() - 1;
}

int LevelManager::getCurrentLevelIndex() const {
    return m_currentLevelIndex;
}
