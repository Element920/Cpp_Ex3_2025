//israelmor555@gmail.com

// Created by israel on 5/19/25.
//
#include "Game.hpp"
#include <stdexcept>
#include "Player.hpp"

using namespace coup;

constexpr int INITIAL_POT = 50;
Game::Game() : current_turn_index(0), pot(INITIAL_POT) {} // לדוגמה, נתחיל קופה עם 50 מטבעות

void Game::add_player(Player* player) {
    if (players_list.size() >= 6) {
        throw std::runtime_error("Maximum number of players reached");
    }
    player->set_game(this); // מקשר את השחקן למשחק
    players_list.push_back(player);
}

Player& Game::turn() {
    if (players_list.empty()) {
        throw std::runtime_error("No players in the game");
    }
    // דלג על שחקנים שלא פעילים
    size_t attempts = 0;
    while (!players_list[current_turn_index]->is_active() && attempts < players_list.size()) {
        current_turn_index = (current_turn_index + 1) % players_list.size();
        ++attempts;
    }
    return *players_list[current_turn_index];
}

void Game::advance_turn() {
    do {
        current_turn_index = (current_turn_index + 1) % players_list.size();
    } while (!players_list[current_turn_index]->is_active());
}

std::vector<std::string> Game::players() const {
    std::vector<std::string> active_names;
    for (const Player* p : players_list) {
        if (p->is_active()) {
            active_names.push_back(p->get_name());
        }
    }
    return active_names;
}

std::string Game::winner() const {
    std::vector<std::string> active = players();

    if (active.size() == 1) {
        return active.front();
    }

    if (active.empty()) {
        throw std::runtime_error("No players active - invalid game state!");
    }

    // Game is not over - there are still multiple active players
    throw GameNotOverException();
}

int Game::get_pot() const {
    return pot;
}

void Game::add_to_pot(int amount) {
    pot += amount;
}

void Game::remove_from_pot(int amount) {
    if (pot < amount) {
        throw std::runtime_error("not enough coins in pot");
    }
    pot -= amount;
}

void Game::print_action(const std::string& action) {
    last_action = action;
}

// std::ostream& operator<<(std::ostream& os, const Game& game) {
//     os << "Last action: " << game;
//     return os;
// }