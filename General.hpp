#ifndef COUP_GENERAL_HPP
#define COUP_GENERAL_HPP

#include "Player.hpp"

namespace coup {

    class General : public Player {
    public:
        General(const std::string& name);

        void gather() ;
        void tax() ;
        void bribe() ;
        void arrest(Player& target) ;
        void sanction(Player& target) ;
        void coup(Player& target) ;
        void use_special_ability(Player& target) ;
    };

} // namespace coup

#endif // COUP_GENERAL_HPP
