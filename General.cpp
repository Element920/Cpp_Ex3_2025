#include "General.hpp"
#include "Game.hpp"
#include <stdexcept>

using namespace coup;

General::General(const std::string& name) : Player(name, Role::General) {}

void General::block_coup(Player& attacker, Player& target) {
    if (coins < 5) {
        throw std::runtime_error("not enough coins to block coup");
    }
    coins -= 5;
    // The coup is blocked - attacker loses their coins but target survives
    if (game) {
        game->print_action(name + " blocked coup attempt");
    }
}

void General::arrest(Player& target) {
    // General calls parent arrest but gets special treatment when being arrested
    Player::arrest(target);
}