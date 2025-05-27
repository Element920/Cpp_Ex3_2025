#ifndef COUP_BARON_HPP
#define COUP_BARON_HPP

#include "Player.hpp"

namespace coup {

    class Baron : public Player {
    public:
        Baron(const std::string& name);

        void gather();
        void tax();
        void bribe();
        void arrest(Player& target);
        void sanction(Player& target) ;
        void coup(Player& target) ;
        void use_special_ability();
    };

} // namespace coup

#endif // COUP_BARON_HPP
