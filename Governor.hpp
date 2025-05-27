#ifndef COUP_GOVERNOR_HPP
#define COUP_GOVERNOR_HPP

#include "Player.hpp"

namespace coup {

    class Governor : public Player {
    public:
        Governor(const std::string& name);

        void gather() ;
        void tax() ;
        void bribe() ;
        void arrest(Player& target) ;
        void sanction(Player& target) ;
        void coup(Player& target) ;
        void use_special_ability(Player& target) ;
    };

} // namespace coup

#endif // COUP_GOVERNOR_HPP
