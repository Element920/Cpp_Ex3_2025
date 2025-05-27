//
// Created by israel on 5/27/25.
//
#include "General.hpp"
#include <stdexcept>
#include "Player.hpp"
#include "Role.hpp"

using namespace coup;

General::General(const std::string& name) : Player( name, Role::General) {}

void General::gather() {
    is_must_coup();
    if (is_action_blocked("gather")) {
        throw std::runtime_error("gather is blocked for this player");
    }
    coins += 1;
    if (game) game->print_action(name + " performed gather");
}

void General::tax() {
    is_must_coup();
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
    coins += 2;
    if (game) game->print_action(name + " performed tax");
}

void General::bribe() {
    is_must_coup();
    if (coins < 4) {
        throw std::runtime_error("not enough coins for bribe");
    }
    coins -= 4;
    if (game) game->print_action(name + " performed bribe");
}

void General::arrest(Player& target) {
    is_must_coup();
    if (!target.is_active()) {
        throw std::runtime_error("target is not active");
    }
    if (target.get_coins() < 1) {
        throw std::runtime_error("target has no coins to steal");
    }
    target.pay_coin(1);
    coins += 1;

    // אם גנרל נעצר – מחזיר את המטבע לעצמו
    if (target.get_name() == this->get_name()) {
        target.receive_coin(1);
        coins -= 1;
    }
    if (game) game->print_action(name + " performed arrest");
}

void General::sanction(Player& target) {
    is_must_coup();
    if (coins < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
    coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");
    if (game) game->print_action(name + " performed sanction");
}

void General::coup(Player& target) {
    if (!target.is_active()) {
        throw std::runtime_error("target already eliminated");
    }

    if (target.get_name() == this->get_name()) {
        throw std::runtime_error("cannot coup self");
    }

    if (coins >= 7) {
        coins -= 7;
        target.deactivate();
    } else if (coins >= 5) {
        // מגן על עצמו או על אחר – פעולה נכשלת אבל משלמים
        coins -= 5;
        // לא מבצע הדחה
    } else {
        throw std::runtime_error("not enough coins for coup or defense");
    }
    if (game) game->print_action(name + " performed coup");
}

void General::use_special_ability(Player& target) {
    // לגנרל אין יכולת אקטיבית מיידית
    throw std::runtime_error("General has no direct special ability to activate");

}
