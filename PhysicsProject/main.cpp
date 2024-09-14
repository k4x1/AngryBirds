#include "Game.h"

int main() {
    Game game;
    game.run();

    // Clean up all GameObjects
    for (auto& object : GameObject::getAllObjects()) {
        delete object;
    }
    return 0;
}
