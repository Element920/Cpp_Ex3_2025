//israelmor555@gmail.com
#include "Governor.hpp"
#include "Game.hpp"
#include <stdexcept>

using namespace coup;

Governor::Governor(const std::string& name) : Player(name, Role::Governor) {}

void Governor::tax() {
    check_turn();
    is_must_coup();
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
    // Governor gets 3 coins from tax
    coins += 3;
    if (game) {
        game->print_action(name + " performed tax (Governor bonus)");
        game->advance_turn();
    }
}

void Governor::undo(Player& target) {
    // This doesn't use a turn - it's a reactive ability
    // Can only undo tax actions
    if (target.get_coins() < 2) {
        throw std::runtime_error("Target doesn't have enough coins to undo tax");
    }
    target.pay_coin(2);
    if (game) {
        game->print_action(name + " undid " + target.get_name() + "'s tax");
    }
}