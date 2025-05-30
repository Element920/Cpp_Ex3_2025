//israelmor555@gmail.com

#ifndef COUP_SPY_HPP
#define COUP_SPY_HPP

#include "Player.hpp"

namespace coup {
    class Spy : public Player {
    public:
        Spy(const std::string& name);
        void use_special_ability(Player& target); // See coins and block arrest
    };
}

#endif // COUP_SPY_HPP