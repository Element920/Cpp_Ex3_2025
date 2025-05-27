//
// Created by israel on 5/25/25.
//
#include "Judge.hpp"
#include <stdexcept>

using namespace coup;

Judge::Judge(const std::string& name) : Player(name , Role::Judge) {}

void Judge::gather() {
    is_must_coup();
    if (is_action_blocked("gather")) {
        throw std::runtime_error("gather is blocked for this player");
    }
    coins += 1;
    if (game) game->print_action(name + " performed gather");
}

void Judge::tax() {
    is_must_coup();
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
    coins += 2;
    if (game) game->print_action(name + " performed tax");
}

void Judge::bribe() {
    is_must_coup();
    if (coins < 4) {
        throw std::runtime_error("not enough coins for bribe");
    }
    coins -= 4;
    if (game) game->print_action(name + " performed bribe");
}

void Judge::arrest(Player& target) {
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

void Judge::sanction(Player& target) {
    is_must_coup();
    if (coins < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
    coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");

    // התוקף (כלומר this) משלם עוד מטבע לקופה
    pay_coin(1);
    if (game) game->print_action(name + " performed sanction");
}

void Judge::coup(Player& target) {
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

void Judge::use_special_ability(Player& target) {
    // מבטל שוחד – השחקן מאבד את ה־4 מטבעות ששילם
    target.receive_coin(-4);
    if (game) game->print_action(name + " performed special ability");
}
