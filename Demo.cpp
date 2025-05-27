
#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Game.hpp"
#include "Role.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;
using namespace coup;

int main() {
    Game game_1{};

    Governor governor("Moshe");
    Spy spy( "Yossi");
    Baron baron("Meirav");
    General general( "Reut");
    Judge judge( "Gilad");

    vector<string> players = game_1.players();
    governor.set_game(&game_1);
    spy.set_game(&game_1);
    baron.set_game(&game_1);
    general.set_game(&game_1);
    judge.set_game(&game_1);
    // Expected output:
    // Moshe
    // Yossi
    // Meirav
    // Reut
    // Gilad
    for(string name : players){
        cout << name << endl;
    }

    // Expected output: Moshe
    cout << game_1.turn() << endl;

    governor.gather();
    spy.gather();
    baron.gather();
    general.gather();
    judge.gather();

    // Expected exception - Not spy's turn
    try{
        spy.gather();
    } catch (const std::exception &e){
        std::cerr << e.what() << '\n';
    }

    governor.gather();
    spy.tax();

    // Expected exception - Judge cannot undo tax
    // try{
    //     judge.undo(governor);
    // } catch (const std::exception &e) {
    //     std::cerr << e.what() << '\n';
    // }

    cout << governor.get_coins() << endl; // Expected: 2
    cout << spy.get_coins() << endl; // Expected: 3

   // governor.undo(spy); // Governor undo tax
     //cout << spy.get_coins() << endl; // Expected: 1

    baron.tax();
    general.gather();
    judge.gather(); 

    governor.tax();
    spy.gather();
    baron.use_special_ability(); // Baron traded its 3 coins and got 6
    general.gather();
    judge.gather();
    
    cout << baron.get_coins() << endl; // Expected: 6

    governor.tax();
    spy.gather();
    baron.gather();
    general.gather();
    judge.gather();

    governor.tax();
    spy.gather();
    cout << baron.get_coins() << endl; // Expected: 7
    baron.coup(governor); // Coup against governor
    general.gather();
    judge.gather();
    
    players = game_1.players();
    // Since no one blocked the Baron, the expected output is:
    // Yossi
    // Meirav
    // Reut
    // Gilad
    for (string name : players) {
        cout << name << endl;
    }

}
