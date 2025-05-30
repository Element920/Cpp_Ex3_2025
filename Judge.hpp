//israelmor555@gmail.com

#ifndef COUP_JUDGE_HPP
#define COUP_JUDGE_HPP

#include "Player.hpp"

namespace coup {
    class Judge : public Player {
    public:
        Judge(const std::string& name);
        void undo_bribe(Player& target); // Cancel bribe
    };
}

#endif