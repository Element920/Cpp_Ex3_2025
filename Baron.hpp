//israelmor555@gmail.com

#ifndef COUP_BARON_HPP
#define COUP_BARON_HPP

#include "Player.hpp"

namespace coup {

    class Baron : public Player {
    public:
        Baron(const std::string& name);

        // Override special ability (investment)
        void use_special_ability() override;
        
        // Override sanction to handle compensation
        void sanction(Player& target) override;
    };

} // namespace coup

#endif // COUP_BARON_HPP