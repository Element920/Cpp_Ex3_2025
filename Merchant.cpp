//
// Created by israel on 5/25/25.
//
#include "Merchant.hpp"
#include <stdexcept>

using namespace coup;

Merchant::Merchant(const std::string& name) : Player(name , Role::Merchant) {}

void Merchant::gather() {
    is_must_coup();
    if (is_action_blocked("gather")) {
        throw std::runtime_error("gather is blocked for this player");
    }
    coins += 1;
    if (game) game->print_action(name + " performed gather");
}

void Merchant::tax() {
    is_must_coup();
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
    coins += 2;
    if (game) game->print_action(name + " performed tax");
}

void Merchant::bribe() {
    is_must_coup();
    if (coins < 4) {
        throw std::runtime_error("not enough coins for bribe");
    }
    coins -= 4;
    if (game) game->print_action(name + " performed bribe");
}

void Merchant::arrest(Player& target) {
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

void Merchant::sanction(Player& target) {
    is_must_coup();
    if (coins < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
    coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");
    if (game) game->print_action(name + " performed sanction");
}

void Merchant::coup(Player& target) {
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

void Merchant::use_special_ability(Player& target) {
    throw std::runtime_error("Merchant has no active special ability to use manually");
}

void Merchant::pre_turn_ability() {
    if (coins >= 3) {
        coins += 1;
    }
    if (game) game->print_action(name + " performed pre-turn ability");
}
