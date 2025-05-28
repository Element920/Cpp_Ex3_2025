#ifndef COUP_GAME_HPP
#define COUP_GAME_HPP

#include <vector>
#include <string>
#include "Player.hpp"


namespace coup {

    class GameNotOverException : public std::exception {
    public:
        const char* what() const noexcept override {
            return "Game is not yet over";
        }
    };

    class Game {
    protected:
        std::vector<Player*> players_list;
        int current_turn_index;
        int pot;
       


    public:
        Game();
        std::string last_action;
        void add_player(Player* player);
        Player& turn();
        std::vector<std::string> players() const;
        std::string winner() const;
        void advance_turn();
        void print_action(const std::string& action);
        friend std::ostream& operator<<(std::ostream& os, const Game& game);
        int get_pot() const;
        void add_to_pot(int amount);
        void remove_from_pot(int amount);
    };

} // namespace coup

#endif // COUP_GAME_HPP
