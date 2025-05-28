#ifndef COUP_MERCHANT_HPP
#define COUP_MERCHANT_HPP

#include "Player.hpp"

namespace coup {
    class Merchant : public Player {
    public:
        Merchant(const std::string& name);
        void pre_turn_ability(); // Gets extra coin if has 3+
    };
}

#endif 