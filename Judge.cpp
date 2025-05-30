//israelmor555@gmail.com

#include "Judge.hpp"
#include "Game.hpp"
#include <stdexcept>

using namespace coup;

Judge::Judge(const std::string& name) : Player(name, Role::Judge) {}

void Judge::undo_bribe(Player& target) {
    // Judge can cancel bribe - target loses the 4 coins they paid
    if (target.get_coins() < 4) {
        throw std::runtime_error("Target doesn't have 4 coins to lose from bribe");
    }
    target.pay_coin(4);
    if (game) {
        game->print_action(name + " cancelled " + target.get_name() + "'s bribe");
    }
}
