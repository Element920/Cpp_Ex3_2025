//israelmor555@gmail.com

#ifndef COUP_GOVERNOR_HPP
#define COUP_GOVERNOR_HPP

#include "Player.hpp"

namespace coup {

    class Governor : public Player {
    public:
        Governor(const std::string& name);

        // Governor gets 3 coins from tax instead of 2
        void tax() override;
        
        // Governor can undo tax actions of other players
        void undo(Player& target);
    };

} // namespace coup

#endif // COUP_GOVERNOR_HPP