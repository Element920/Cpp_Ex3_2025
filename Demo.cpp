#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Game.hpp"
#include "Role.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace coup;

int main() {
    Game game_1{};

    Governor governor("Moshe");
    Spy spy("Yossi");
    Baron baron("Meirav");
    General general("Reut");
    Judge judge("Gilad");

    // CRITICAL FIX: Add players to the game
    game_1.add_player(&governor);
    game_1.add_player(&spy);
    game_1.add_player(&baron);
    game_1.add_player(&general);
    game_1.add_player(&judge);

    vector<string> players = game_1.players();
    
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
    cout << game_1.turn().get_name() << endl;

    // Now players take turns properly
    governor.gather();    // Moshe's turn
    spy.gather();         // Yossi's turn  
    baron.gather();       // Meirav's turn
    general.gather();     // Reut's turn
    judge.gather();       // Gilad's turn

    // Expected exception - Not spy's turn (it's Moshe's turn now)
    try{
        spy.gather();
    } catch (const std::exception &e){
        std::cerr << e.what() << '\n';
    }

    governor.gather();    // Moshe's turn
    spy.tax();           // Yossi's turn

    cout << governor.get_coins() << endl; // Expected: 4 (started with 2, gathered twice)
    cout << spy.get_coins() << endl; // Expected: 5 (started with 2, gathered once, tax once)

    baron.tax();         // Meirav's turn
    general.gather();    // Reut's turn
    judge.gather();      // Gilad's turn

    governor.tax();      // Moshe's turn (Governor gets 3 from tax)
    spy.gather();        // Yossi's turn
    baron.use_special_ability(); // Meirav's turn - Baron investment: pay 3, get 6
    general.gather();    // Reut's turn
    judge.gather();      // Gilad's turn
    
    cout << baron.get_coins() << endl; // Expected: 6 (2+2+3-3+6)

    governor.tax();      // Moshe's turn
    spy.gather();        // Yossi's turn
    baron.gather();      // Meirav's turn
    general.gather();    // Reut's turn
    judge.gather();      // Gilad's turn

    governor.tax();      // Moshe's turn
    spy.gather();        // Yossi's turn
    cout << baron.get_coins() << endl; // Expected: 7
    baron.coup(governor); // Meirav's turn - Coup against governor
    general.gather();    // Reut's turn
    judge.gather();      // Gilad's turn
    
    players = game_1.players();
    // Since Governor was eliminated, expected output:
    // Yossi
    // Meirav
    // Reut
    // Gilad
    for (string name : players) {
        cout << name << endl;
    }

    return 0;
}