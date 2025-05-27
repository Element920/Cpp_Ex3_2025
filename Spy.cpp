//
// Created by israel on 5/25/25.
//
#include "Spy.hpp"
#include <stdexcept>
#include <iostream>

using namespace coup;

Spy::Spy(const std::string& name) : Player(name , Role::Spy) {}

void Spy::gather() {
    is_must_coup();
    if (is_action_blocked("gather")) {
        throw std::runtime_error("gather is blocked for this player");
    }
    coins += 1;
}

void Spy::tax() {
    is_must_coup();
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
    coins += 2;
}

void Spy::bribe() {
    is_must_coup();
    if (coins < 4) {
        throw std::runtime_error("not enough coins for bribe");
    }
    coins -= 4;
}

void Spy::arrest(Player& target) {
    is_must_coup();
    if (!target.is_active()) {
        throw std::runtime_error("target is not active");
    }
    if (target.get_coins() < 1) {
        throw std::runtime_error("target has no coins to steal");
    }
    target.pay_coin(1);
    coins += 1;
}

void Spy::sanction(Player& target) {
    is_must_coup();
    if (coins < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
    coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");
}

void Spy::coup(Player& target) {
    if (!target.is_active()) {
        throw std::runtime_error("target already eliminated");
    }
    if (coins < 7) {
        throw std::runtime_error("not enough coins for coup");
    }
    coins -= 7;
    target.deactivate();
}

void Spy::use_special_ability(Player& target) {
    std::cout << target.get_name() << " has " << target.get_coins() << " coins." << std::endl;
    target.add_blocked_action("arrest");
}
