#include "Merchant.hpp"
#include "Game.hpp"

using namespace coup;

Merchant::Merchant(const std::string& name) : Player(name, Role::Merchant) {}

void Merchant::pre_turn_ability() {
    if (coins >= 3) {
        coins += 1;
        if (game) {
            game->print_action(name + " received merchant bonus");
        }
    }
}