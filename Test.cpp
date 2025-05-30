//israelmor555@gmail.com
//
// Created by israel on 5/19/25.
//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Game.hpp"
#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

using namespace coup;

TEST_CASE("Basic Player actions") {
    Governor p("Alice");

    CHECK(p.get_name() == "Alice");
    CHECK(p.get_role() == Role::Governor);
    CHECK(p.get_coins() == 0);

    p.gather();
    CHECK(p.get_coins() == 1);

    p.tax();
    CHECK(p.get_coins() == 4); // 1 + 3 = 4

    p.bribe();
    CHECK(p.get_coins() == 2);


}

TEST_CASE("Bribe should throw if not enough coins") {
    Player p("Bob", Role::Spy);
    CHECK_THROWS_AS(p.bribe(), std::runtime_error);
}

TEST_CASE("Coup and deactivate") {
    Player p1("Alice", Role::General);
    Player p2("Bob", Role::Merchant);

    for (int i = 0; i < 7; ++i) {
        p1.gather();
    }
    CHECK(p1.get_coins() == 7);

    p1.coup(p2);
    CHECK_FALSE(p2.is_active());
    CHECK(p1.get_coins() == 0);
}

TEST_CASE("Game turn logic and winner") {
    Game g;
    Player p1("Alice", Role::Spy);
    Player p2("Bob", Role::Governor);
    Player p3("Charlie", Role::Baron);

    g.add_player(&p1);
    g.add_player(&p2);
    g.add_player(&p3);

    CHECK(g.turn().get_name() == "Alice");
    g.advance_turn();
    CHECK(g.turn().get_name() == "Bob");
    g.advance_turn();
    CHECK(g.turn().get_name() == "Charlie");
    g.advance_turn();
    CHECK(g.turn().get_name() == "Alice");

    SUBCASE("Game not over yet") {
        CHECK_THROWS_AS(g.winner(), GameNotOverException);
    }

    SUBCASE("All players deactivated - game invalid") {
        p1.deactivate();
        p2.deactivate();
        p3.deactivate();
        CHECK_THROWS_AS(g.winner(), std::runtime_error);
    }

    SUBCASE("One winner remains") {
        p1.deactivate();
        p2.deactivate();
        CHECK(g.winner() == "Charlie");
    }
}

TEST_CASE("Basic Governor actions") {
    Governor g("Gov");
    CHECK(g.get_role() == Role::Governor);
    g.gather();
    CHECK(g.get_coins() == 1);
    g.tax();
    CHECK(g.get_coins() == 4); // Governor gets 3 from tax
    // CHECK_THROWS(g.bribe());   // Not enough coins to bribe (needs 2, has 4? This should NOT throw)
}

TEST_CASE("Basic Spy actions") {
    Spy s("SpyGuy");
    CHECK(s.get_role() == Role::Spy);
    s.gather();
    s.gather();
    s.gather();
    s.gather(); // Need 4 coins for Spy bribe
    CHECK(s.get_coins() == 4);
    s.bribe();
    CHECK(s.get_coins() == 0);
}

TEST_CASE("Baron economic actions") {
    Baron b("Baron");
    CHECK(b.get_role() == Role::Baron);
    b.gather();
    b.tax();
    CHECK(b.get_coins() == 3); // 1 + 2 = 3
    CHECK_THROWS(b.bribe()); // Baron needs 4 coins, only has 3
}

TEST_CASE("General coup and block") {
    General gen("Gen");
    Player target("Target", Role::Spy);

    for (int i = 0; i < 7; ++i) gen.gather();
    CHECK(gen.get_coins() == 7);
    gen.coup(target);
    CHECK_FALSE(target.is_active());
    CHECK(gen.get_coins() == 0);
}

TEST_CASE("Judge blocks and legal role") {
    Judge j("Judgey");
    CHECK(j.get_role() == Role::Judge);
    j.gather();
    j.gather();
    j.tax();
    CHECK(j.get_coins() == 4); // 2 + 2 = 4
}

TEST_CASE("Merchant coin collection") {
    Merchant m("Merch");
    CHECK(m.get_role() == Role::Merchant);
    m.gather();
    m.gather();
    CHECK(m.get_coins() == 2);
}

TEST_CASE("Bribe with insufficient coins throws") {
    Player p("Bob", Role::Spy);
    CHECK_THROWS_AS(p.bribe(), std::runtime_error);
}

TEST_CASE("Coup and deactivate logic") {
    Player p1("Alice", Role::General);
    Player p2("Bob", Role::Merchant);
    for (int i = 0; i < 7; ++i) p1.gather();
    CHECK(p1.get_coins() == 7);
    p1.coup(p2);
    CHECK_FALSE(p2.is_active());
    CHECK(p1.get_coins() == 0);
}

TEST_CASE("Game turn and winner logic") {
    Game g;
    Player p1("Alice", Role::Spy);
    Player p2("Bob", Role::Governor);
    Player p3("Charlie", Role::Baron);
    g.add_player(&p1);
    g.add_player(&p2);
    g.add_player(&p3);

    CHECK(g.turn().get_name() == "Alice");
    g.advance_turn();
    CHECK(g.turn().get_name() == "Bob");
    g.advance_turn();
    CHECK(g.turn().get_name() == "Charlie");
    g.advance_turn();
    CHECK(g.turn().get_name() == "Alice");

    // Game not over with all players active
    CHECK_THROWS_AS(g.winner(), GameNotOverException);

    // One player remains - game over
    p1.deactivate();
    p2.deactivate();
    CHECK(g.winner() == "Charlie");
}

TEST_CASE("Special abilities do not crash") {
    Governor g("Gov");
    Spy s("Spy");
    Baron b("Baron");
    General gen("Gen");
    Judge j("Judge");
    Merchant m("Merch");

    CHECK_THROWS(g.use_special_ability());
    CHECK_THROWS(b.use_special_ability()); // Baron needs 3 coins
    CHECK_THROWS(gen.use_special_ability());
    CHECK_THROWS(j.use_special_ability());
    CHECK_THROWS(m.use_special_ability());
}

// TEST_CASE("Blocking logic simulation") {
//     Game g;
//     General gen("General");
//     Judge judge("Judge");
//     Spy spy("Spy");
//
//     g.add_player(&gen);
//     g.add_player(&judge);
//     g.add_player(&spy);
//
//     // Give spy enough coins for coup
//     for (int i = 0; i < 7; ++i) spy.gather();
//     spy.coup(judge);
//     CHECK_FALSE(judge.is_active());
//     CHECK(spy.get_coins() == 0);
//
//     // Give general coins for coup
//     for (int i = 0; i < 7; ++i) gen.gather();
//     gen.coup(spy);
//     CHECK_FALSE(spy.is_active());
//     CHECK(gen.get_coins() == 0);
// }

TEST_CASE("Edge cases - deactivate twice") {
    Player p("Deadman", Role::Baron);
    p.deactivate();
    CHECK_FALSE(p.is_active());
    CHECK_THROWS(p.deactivate()); // Should throw when trying to deactivate again
}

TEST_CASE("Edge cases - illegal actions") {
    Player p("P", Role::Spy);
    CHECK_THROWS(p.bribe()); // not enough coins

    for (int i = 0; i < 3; ++i) p.gather();
    CHECK(p.get_coins() == 3);
    CHECK_THROWS(p.bribe()); // Still not enough (needs 4)
    CHECK(p.get_coins() == 3); // Coins unchanged after failed bribe
}

TEST_CASE("Tax differences per role") {
    Governor g("G");
    g.tax();
    CHECK(g.get_coins() == 3); // Governor gets 3

    Player normal("N", Role::Merchant);
    normal.tax();
    CHECK(normal.get_coins() == 2); // Non-Governor gets 2
}

// Additional comprehensive tests
TEST_CASE("Baron special ability - Investment") {
    Baron b("RichBaron");

    // Give Baron enough coins for investment
    b.gather();
    b.gather();
    b.gather(); // 3 coins

    b.use_special_ability(); // Pay 3, get 6, net +3
    CHECK(b.get_coins() == 6);
}

TEST_CASE("Multiple player game simulation") {
    Game g;
    Governor gov("Governor");
    Spy spy("Spy");
    Baron baron("Baron");

    g.add_player(&gov);
    g.add_player(&spy);
    g.add_player(&baron);

    // Governor's turn
    CHECK(g.turn().get_name() == "Governor");
    gov.tax(); // Gets 3 coins
    CHECK(gov.get_coins() == 3);

    // Spy's turn
    CHECK(g.turn().get_name() == "Spy");
    spy.gather(); // Gets 1 coin
    CHECK(spy.get_coins() == 1);

    // Baron's turn
    CHECK(g.turn().get_name() == "Baron");
    baron.gather(); // Gets 1 coin
    CHECK(baron.get_coins() == 1);
}

TEST_CASE("Coin limits and must coup rule") {
    Player p("Rich", Role::Governor);

    // Give player 10+ coins
    for (int i = 0; i < 10; ++i) {
        p.gather();
    }
    CHECK(p.get_coins() == 10);

    // Should be forced to coup
    CHECK_THROWS(p.gather()); // Should throw because must coup
    CHECK_THROWS(p.tax());    // Should throw because must coup
}

TEST_CASE("Arrest functionality") {
    Game g;
    Judge judge("Judge");
    Spy spy("Spy");
    Baron baron("Baron");

    g.add_player(&judge);
    g.add_player(&spy);
    g.add_player(&baron);

    // Judge tries to arrest Baron (should work)
    judge.arrest(baron);
    CHECK_FALSE(baron.is_active());

    // Judge tries to arrest Spy (should fail)
    CHECK_THROWS(judge.arrest(spy)); // Can't arrest Spy
}

// TEST_CASE("Sanction with blocking") {
//     Game g;
//     Player p1("Sanctioner", Role::Judge);
//     Player p2("Target", Role::Merchant);
//
//     g.add_player(&p1);
//     g.add_player(&p2);
//
//     // Give p1 enough coins for sanction
//     for (int i = 0; i < 3; ++i) {
//         p1.gather();
//     }
//
//     p1.sanction(p2);
//     CHECK(p1.get_coins() == 0); // Paid 3 for sanction
//
//     // p2 should now be blocked from gather and tax
//     CHECK_THROWS(p2.gather()); // Should be blocked
//     CHECK_THROWS(p2.tax());    // Should be blocked
// }
//
// TEST_CASE("General blocking coup") {
//     Game g;
//     Player attacker("Attacker", Role::Spy);
//     General defender("Defender");
//
//     g.add_player(&attacker);
//     g.add_player(&defender);
//
//     // Give both players enough coins
//     for (int i = 0; i < 7; ++i) {
//         attacker.gather();
//     }
//     for (int i = 0; i < 5; ++i) {
//         defender.gather();
//     }
//
//     CHECK(attacker.get_coins() == 7);
//     CHECK(defender.get_coins() == 5);
//
//     // Attacker tries to coup General with 5+ coins
//     attacker.coup(defender);
//     CHECK(attacker.get_coins() == 0); // Paid 7 for coup
//     CHECK(defender.get_coins() == 0); // Paid 5 to block
//     CHECK(defender.is_active()); // Should still be active (blocked coup)
// }

TEST_CASE("Role-specific bribe costs") {
    Governor gov("Gov");
    Spy spy("Spy");

    // Governor bribe costs 2
    gov.gather();
    gov.gather(); // 2 coins
    gov.bribe(); // Should work
    CHECK(gov.get_coins() == 0);

    // Spy bribe costs 4
    for (int i = 0; i < 4; ++i) {
        spy.gather();
    }
    spy.bribe(); // Should work
    CHECK(spy.get_coins() == 0);
}

TEST_CASE("Invalid actions on inactive players") {
    Player p1("Active", Role::Governor);
    Player p2("Inactive", Role::Spy);

    p2.deactivate();

    // Can't target inactive players
    for (int i = 0; i < 7; ++i) {
        p1.gather();
    }
    CHECK_THROWS(p1.coup(p2)); // Can't coup inactive player

    // Inactive players can't perform actions
    CHECK_FALSE(p2.is_active());
}

TEST_CASE("Game winner with edge cases") {
    Game g;
    Player p1("P1", Role::Governor);
    Player p2("P2", Role::Spy);

    g.add_player(&p1);
    g.add_player(&p2);

    // Game not over with 2 active players
    CHECK_THROWS_AS(g.winner(), GameNotOverException);

    // Game over with 1 active player
    p1.deactivate();
    CHECK(g.winner() == "P2");

    // Invalid game state with 0 active players
    p2.deactivate();
    CHECK_THROWS(g.winner());
}