//israelmor555@gmail.com

//
// Created by israel on 5/19/25.
//
#include "Player.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>
#include "Role.hpp"

using namespace coup;

Player::Player(std::string name, Role role)
    : name(std::move(name)), role(role), coins(0), active(true){} // Start with 2 coins as per game rules

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
    this->coins = coins; // Set absolute value, not add
}

bool Player::is_active() const {
    return active;
}

void Player::check_turn() const {
    if (game && &game->turn() != this) {
        throw std::runtime_error("Not your turn");
    }
}

void Player::gather() {
    check_turn();
    is_must_coup();
    if (is_action_blocked("gather")) {
        throw std::runtime_error("gather is blocked for this player");
    }
    coins += 1;
    if (game) {
        game->print_action(name + " performed gather");
        game->advance_turn();
    }
}

void Player::tax() {
    check_turn();
    is_must_coup();
    if (is_action_blocked("tax")) {
        throw std::runtime_error("tax is blocked for this player");
    }
    // Only Governor gets tax benefit, others should throw or not be allowed
    if (role == Role::Governor) {
        coins += 3; // Governor gets 3
    } else {
        coins += 2;
    }

    if (game) {
        game->print_action(name + " performed tax");
        game->advance_turn();
    }
}

void Player::bribe() {
    check_turn();
    is_must_coup();

    // Check role-specific requirements for bribe
    // if (role == Role::Governor) {
    //     // Governor should have additional restrictions or throw in certain conditions
    //     throw std::runtime_error("Governor cannot bribe under current conditions");
    // }

    int cost = (role == Role::Governor) ? 2 : 4;
    if (coins < cost) {
        throw std::runtime_error("not enough coins for bribe");
    }

    coins -= cost;

    if (game) {
        game->print_action(name + " performed bribe");
        game->advance_turn();
    }
}

void Player::arrest(Player& target) {
    check_turn();
    is_must_coup();
    if (!target.is_active()) {
        throw std::runtime_error("Cannot arrest " + target.get_name() + ": target is inactive");
    }
    if (is_action_blocked("arrest")) {
        throw std::runtime_error("Cannot arrest: action is blocked for " + name);
    }
    if (target.role == Role::Spy) {
        throw std::runtime_error("Cannot arrest " + target.get_name() + ": they are a Spy.");
    }

    // Actually perform the arrest
    target.deactivate();

    if (game) {
        game->print_action(name + " arrested " + target.get_name());
        game->advance_turn();
    }
}

void Player::sanction(Player& target) {
    check_turn();
    is_must_coup();
    if (coins < 3) {
        throw std::runtime_error("not enough coins for sanction");
    }
    coins -= 3;
    target.add_blocked_action("gather");
    target.add_blocked_action("tax");

    if (target.role == Role::Baron) {
        target.receive_coin(1); // Compensation
    }
    if (target.role == Role::Judge) {
        pay_coin(1); // Additional payment to pot
        if (game) game->add_to_pot(1);
    }

    if (game) {
        game->print_action(name + " performed sanction");
        game->advance_turn();
    }
}

void Player::coup(Player& target) {
    check_turn();
    if (!target.is_active()) {
        throw std::runtime_error("target is already eliminated");
    }
    if (coins < 7) {
        throw std::runtime_error("not enough coins for coup");
    }

    if (target.role == Role::General && target.coins >= 5) {
        target.coins -= 5; // General pays to block
        coins -= 7; // Attacker still pays full cost
        if (game) {
            game->print_action(name + " attempted coup but was blocked by General");
            game->advance_turn();
        }
        return;
    }

    coins -= 7;
    target.deactivate();
    if (game) {
        game->print_action(name + " performed coup");
        game->advance_turn();
    }
}

void Player::use_special_ability() {
    check_turn();
    switch (role) {
        case Role::Baron:
            // Investment: pay 3, get 6
            if (coins < 3) {
                throw std::runtime_error("not enough coins to invest");
            }
            coins -= 3;
            coins += 6;
            break;
        case Role::Spy:
            // Spy should have restrictions on when they can use special ability
            throw std::runtime_error("Spy cannot use special ability under current conditions");
            break;
        default:
            throw std::runtime_error("this role has no special ability");
    }
    if (game) {
        game->print_action(name + " used special ability");
        game->advance_turn();
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
    if (!active) {
        throw std::runtime_error("Player is already deactivated");
    }
    this->active = false;
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

void Player::block_action(Player& target, const std::string& action) {
    if (!is_active()) {
        throw std::runtime_error("Cannot block action: player is not active");
    }
    if (game == nullptr) {
        throw std::runtime_error("Game is not initialized for player");
    }
    if (!target.is_action_blocked(action)) {
        target.add_blocked_action(action);
        if (game) {
            game->print_action(name + " blocked " + action + " performed by " + target.get_name());
        }
    } else {
        throw std::runtime_error("Action " + action + " is already blocked for " + target.get_name());
    }
}