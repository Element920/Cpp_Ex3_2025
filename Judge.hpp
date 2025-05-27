#ifndef COUP_JUDGE_HPP
#define COUP_JUDGE_HPP

#include "Player.hpp"

namespace coup {

    class Judge : public Player {
    public:
        Judge(const std::string& name);

        void gather() ;
        void tax() ;
        void bribe() ;
        void arrest(Player& target) ;
        void sanction(Player& target) ;
        void coup(Player& target) ;
        void use_special_ability(Player& target) ;
    };

} // namespace coup

#endif // COUP_JUDGE_HPP
