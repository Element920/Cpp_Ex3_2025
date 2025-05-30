//israelmor555@gmail.com

#include "General.hpp"
#include "Game.hpp"
#include <stdexcept>

using namespace coup;

General::General(const std::string& name) : Player(name, Role::General) {}

void General::block_coup(Player& , Player& ) {
    if (coins < 5) {
        throw std::runtime_error("not enough coins to block coup");
    }
    coins -= 5;

    if (game) {
        game->print_action(name + " blocked coup attempt");
    }
}

void General::arrest(Player& target) {
    // General calls parent arrest but gets special treatment when being arrested
    Player::arrest(target);
}