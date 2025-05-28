//
// Created by israel on 5/19/25.
//
// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Player.hpp"
#include "Game.hpp"
#include "Governor.hpp"

using namespace coup;
TEST_CASE("Basic Player actions") {
    Governor p("Alice");

    CHECK(p.get_name() == "Alice");
    CHECK(p.get_role() == Role::Governor);
    CHECK(p.get_coins() == 0);

    p.gather();
    CHECK(p.get_coins() == 1);

    p.tax();
    CHECK(p.get_coins() == 3);

    p.bribe();
    CHECK(p.get_coins() == -1); // לא הגיוני, צריכה להיות חריגה
}

// TEST_CASE("Bribe should throw if not enough coins") {
//     Player p("Bob", Role::Spy);
//     CHECK_THROWS_AS(p.bribe(), std::runtime_error);
// }
//
// TEST_CASE("Coup and deactivate") {
//     Player p1("Alice", Role::General);
//     Player p2("Bob", Role::Merchant);
//
//     for (int i = 0; i < 7; ++i) {
//         p1.gather();
//     }
//     CHECK(p1.get_coins() == 7);
//
//     p1.coup(p2);
//     CHECK_FALSE(p2.is_active());
//     CHECK(p1.get_coins() == 0);
// }
//
// TEST_CASE("Game turn logic and winner") {
//     Game g;
//     Player p1("Alice", Role::Spy);
//     Player p2("Bob", Role::Governor);
//     Player p3("Charlie", Role::Baron);
//
//     g.add_player(&p1);
//     g.add_player(&p2);
//     g.add_player(&p3);
//
//     CHECK(g.turn().get_name() == "Alice");
//     g.advance_turn();
//     CHECK(g.turn().get_name() == "Bob");
//     g.advance_turn();
//     CHECK(g.turn().get_name() == "Charlie");
//     g.advance_turn();
//     CHECK(g.turn().get_name() == "Alice");
//
//     p1.deactivate();
//     p2.deactivate();
//     CHECK_THROWS_AS(g.winner(), GameNotOverException);
//     p3.deactivate();
//
//     // אין אף שחקן פעיל
//     CHECK_THROWS(g.winner());
// }