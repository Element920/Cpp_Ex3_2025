#ifndef COUP_SPY_HPP
#define COUP_SPY_HPP

#include "Player.hpp"

namespace coup {

    class Spy : public Player {
    public:
        Spy(const std::string& name);

        void gather() ;
        void tax() ;
        void bribe() ;
        void arrest(Player& target) ;
        void sanction(Player& target) ;
        void coup(Player& target) ;
        void use_special_ability(Player& target) ;
    };

} // namespace coup

#endif // COUP_SPY_HPP
