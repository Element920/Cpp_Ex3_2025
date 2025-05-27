#ifndef COUP_PLAYER_HPP
#define COUP_PLAYER_HPP

#include <string>
#include <vector>
#include "Role.hpp"
#include "Game.hpp"


namespace coup {
    class Game;

    class Player {
    protected:
        std::string name;
        Role role;
        int coins;
        bool active;
        std::vector<std::string> blocked_actions;
        void is_must_coup() const;
        coup::Game* game;

    public:
        Player(std::string name, Role role);
        void set_game(coup::Game* g) { game = g; }
        coup::Game* get_game() const { return game; }

        const std::string& get_name() const;
        Role get_role() const;
        int get_coins() const;
        bool is_active() const;
        void set_coins(int coins);

        void gather();
        void tax();
        void bribe();
        void arrest(Player& target);
        void sanction(Player& target);
        void coup(Player& target);

        void use_special_ability(Player &target);

        void pre_turn_ability();

        void receive_coin(int amount);
        void pay_coin(int amount);
        void deactivate();
        void add_blocked_action(const std::string& action);
        void clear_blocked_actions();
        bool is_action_blocked(const std::string& action) const;
    };

} // namespace coup

#endif // COUP_PLAYER_HPP
