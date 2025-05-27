#ifndef COUP_MERCHANT_HPP
#define COUP_MERCHANT_HPP

#include "Player.hpp"

namespace coup {

    class Merchant : public Player {
    public:
        Merchant(const std::string& name);

        void gather() ;
        void tax() ;
        void bribe() ;
        void arrest(Player& target) ;
        void sanction(Player& target) ;
        void coup(Player& target) ;
        void use_special_ability(Player& target) ;
        void pre_turn_ability() ;
    };

} // namespace coup

#endif // COUP_MERCHANT_HPP
