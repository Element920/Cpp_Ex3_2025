#include "Spy.hpp"
#include "Game.hpp"
#include <iostream>

using namespace coup;

Spy::Spy(const std::string& name) : Player(name, Role::Spy) {}

void Spy::use_special_ability(Player& target) {
    // Spy can see target's coins and block their arrest
    std::cout << target.get_name() << " has " << target.get_coins() << " coins." << std::endl;
    target.add_blocked_action("arrest");
    if (game) {
        game->print_action(name + " spied on " + target.get_name());
    }
}