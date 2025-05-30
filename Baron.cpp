//israelmor555@gmail.com
// Created by israel on 5/25/25.
//
#include "Baron.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

using namespace coup;

Baron::Baron(const std::string& name) : Player(name, Role::Baron) {}

void Baron::use_special_ability() {
    check_turn();
    is_must_coup();
    // Investment: pay 3, get 6
    if (this->coins < 3) {
        throw std::runtime_error("not enough coins to invest");
    }
    this->coins -= 3;
    this->coins += 6;
    if (game) {
        game->print_action(name + " performed investment (special ability)");
        game->advance_turn();
    }
}

void Baron::sanction(Player& target) {
    check_turn();
    is_must_coup();
    if (coins < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
    coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");

    // Baron receives compensation when being sanctioned (handled in Player class)
    // This method is for when Baron sanctions others
    if (target.get_role() == Role::Judge) {
        pay_coin(1); // Additional payment to pot when sanctioning Judge
        if (game) game->add_to_pot(1);
    }
    
    if (game) {
        game->print_action(name + " performed sanction");
        game->advance_turn();
    }
}