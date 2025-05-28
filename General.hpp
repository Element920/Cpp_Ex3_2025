#ifndef COUP_GENERAL_HPP
#define COUP_GENERAL_HPP

#include "Player.hpp"

namespace coup {
    class General : public Player {
    public:
        General(const std::string& name);
        void block_coup(Player& attacker, Player& target); // Block coup for 5 coins
        void arrest(Player& target) override; // Gets coin back when arrested
    };
}

#endif 