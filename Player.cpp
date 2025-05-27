//
// Created by israel on 5/19/25.
//
#include "Player.hpp"
#include <stdexcept>
#include <iostream>
#include "Role.hpp"

using namespace coup;

Player::Player(std::string name, Role role)
    : name(std::move(name)), role(role), coins(0), active(true), game(nullptr){}

const std::string& Player::get_name() const {
    return name;
}


Role Player::get_role() const {
    return role;
}

int Player::get_coins() const {
    return coins;
}

void Player::set_coins(int coins) {
    this->coins += coins;
}


bool Player::is_active() const {
    return active;
}

void Player::gather() {
    if (is_action_blocked("gather")) {
        throw std::runtime_error("gather is blocked for this player");
    }
    coins += 1;
}

void Player::tax() {
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
    if (role == Role::Governor) {
        coins += 3;
    } else {
        coins += 2;
    }
}

void Player::bribe() {
    if (coins < 4) {
        throw std::runtime_error("not enough coins for bribe");
    }
    coins -= 4;
    // פעולה נוספת מתבצעת מחוץ למחלקה
}

void Player::arrest(Player& target) {
    if (!target.is_active()) {
        throw std::runtime_error("target is not active");
    }
    if (target.role == Role::Spy) {
        throw std::runtime_error("cannot arrest a Spy");
    }
    if (target.role == Role::Merchant) {
        if (target.coins < 2) {
            throw std::runtime_error("Merchant has insufficient coins for penalty");
        }
        target.coins -= 2; // משלם לקופה
    } else if (target.role == Role::General) {
        if (target.coins < 1) {
            throw std::runtime_error("General has no coins to refund");
        }
        // לא לוקחים ממנו מטבע, או מחזירים אותו מיד
    } else {
        if (target.coins < 1) {
            throw std::runtime_error("target has no coins to steal");
        }
        target.coins -= 1;
        coins += 1;
    }
}

void Player::sanction(Player& target) {
    if (coins < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
    coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");

    if (target.role == Role::Baron) {
        target.receive_coin(1); // פיצוי
    }
    if (target.role == Role::Judge) {
        pay_coin(1); // תשלום נוסף לקופה
    }
}

void Player::coup(Player& target) {
    if (!target.is_active()) {
        throw std::runtime_error("target is already eliminated");
    }
    if (coins < 7) {
        throw std::runtime_error("not enough coins for coup");
    }

    if (target.role == Role::General && coins >= 5) {
        coins -= 5; // הגנה מצליחה, הפעולה נכשלת
        return;
    }

    coins -= 7;
    target.deactivate();
}






void Player::use_special_ability(Player& target) {
    switch (role) {
        case Role::Spy:
            std::cout << target.get_name() << " has " << target.get_coins() << " coins." << std::endl;
            target.add_blocked_action("arrest");
            break;
        case Role::Baron:
            if (coins < 3) {
                throw std::runtime_error("not enough coins to invest");
            }
            coins -= 3;
            coins += 6;
            break;
        case Role::Judge:
            target.receive_coin(-4); // ביטול שוחד
            break;
        default:
            throw std::runtime_error("this role has no active ability");
    }
}

void Player::pre_turn_ability() {
    if (role == Role::Merchant && coins >= 3) {
        coins += 1;
    }
}

void Player::receive_coin(int amount) {
    coins += amount;
}

void Player::pay_coin(int amount) {
    if (coins < amount) {
        throw std::runtime_error("not enough coins");
    }
    coins -= amount;
}

void Player::deactivate() {
    active = false;
}

void Player::add_blocked_action(const std::string& action) {
    blocked_actions.push_back(action);
}

void Player::clear_blocked_actions() {
    blocked_actions.clear();
}

bool Player::is_action_blocked(const std::string& action) const {
    for (const std::string& a : blocked_actions) {
        if (a == action) return true;
    }
    return false;
}

void Player::is_must_coup() const {
    if (coins >= 10) {
        throw std::runtime_error("Must perform coup when holding 10 or more coins.");
    }
}
