//
// Created by israel on 5/25/25.
//
#include "Baron.hpp"
#include <stdexcept>
#include <iostream>
#include "Game.hpp"

using namespace coup;

Baron::Baron(const std::string& name) : Player(name, Role::Baron) {}

void Baron::gather() {
    is_must_coup();
    if (is_action_blocked("gather")) {
        throw std::runtime_error("gather is blocked for this player");
    }
    coins += 1;
    if (game) game->print_action(name + " performed gather");

}

void Baron::tax() {
 is_must_coup();
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
   coins += 2;
    if (game) game->print_action(name + " performed tax");

}

void Baron::bribe() {
    is_must_coup();
    if (this->get_coins() < 4) {
        throw std::runtime_error("not enough coins for bribe");
    }
    coins -= 4;
    if (game) game->print_action(name + " performed bribe");

}

void Baron::arrest(Player& target) {
    is_must_coup();
    if (!target.is_active()) {
        throw std::runtime_error("target is not active");
    }
    if (target.get_coins() < 1) {
        throw std::runtime_error("target has no coins to steal");
    }
    target.pay_coin(1);
    coins += 1;
    if (game) game->print_action(name + " performed arrest");

}

void Baron::sanction(Player& target) {
    is_must_coup();
    if (target.get_coins() < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
   coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");

    // ברון מקבל מטבע כפיצוי
    receive_coin(1);
    if (game) game->print_action(name + " performed sanction");
}

void Baron::coup(Player& target) {
    if (!target.is_active()) {
        throw std::runtime_error("target already eliminated");
    }
    if (coins < 7) {
        throw std::runtime_error("not enough coins for coup");
    }
    coins -= 7;
    target.deactivate();
    if (game) game->print_action(name + " performed coup");
}

void Baron::use_special_ability() {
    // השקעה: משלם 3 ומקבל 6
    if (this->coins < 3) {
        throw std::runtime_error("not enough coins to invest");
    }
    this->coins -= 3;
    this->coins += 6;
    if (game) game->print_action(name + " performed special ability");
}
