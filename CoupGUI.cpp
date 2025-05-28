#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Game.hpp"
#include "Role.hpp"

using namespace std;
using namespace coup;

class CoupGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Game game;

    // Store actual player objects
    Governor* governor;
    Spy* spy;
    Baron* baron;
    General* general;
    Judge* judge;
    Merchant* merchant;

    // UI Elements
    sf::Text titleText;
    sf::Text gameStatusText;
    sf::Text playerInfoText;
    sf::Text actionLogText;

    // Action buttons
    vector<sf::RectangleShape> actionButtons;
    vector<sf::Text> actionButtonTexts;
    vector<string> actionNames;

    // Player selection buttons (for targeted actions)
    vector<sf::RectangleShape> playerButtons;
    vector<sf::Text> playerButtonTexts;

    // Game state
    string actionLog;
    bool waitingForTarget;
    string pendingAction;
    Player* selectedTarget;
    bool gameOver;

    // Colors
    sf::Color backgroundColor = sf::Color(30, 30, 40);
    sf::Color buttonColor = sf::Color(70, 70, 90);
    sf::Color buttonHoverColor = sf::Color(90, 90, 120);
    sf::Color activePlayerColor = sf::Color(100, 150, 100);
    sf::Color textColor = sf::Color::White;
    sf::Color disabledButtonColor = sf::Color(50, 50, 60);

public:
    CoupGUI() : window(sf::VideoMode(1600, 1000), "Coup Game"),
                waitingForTarget(false), selectedTarget(nullptr), gameOver(false) {
        // Initialize player objects with exception safety
        try {
            governor = new Governor("Alice");
            spy = new Spy("Bob");
            baron = new Baron("Carol");
            general = new General("David");
            judge = new Judge("Eve");
            merchant = new Merchant("Frank");
        } catch (...) {
            // Clean up any allocated memory if construction fails
            cleanup();
            throw;
        }

        // Load font with better fallback handling
        loadFont();
        setupUI();
        initializeGame();
    }

    ~CoupGUI() {
        cleanup();
    }

    void cleanup() {
        delete governor; governor = nullptr;
        delete spy; spy = nullptr;
        delete baron; baron = nullptr;
        delete general; general = nullptr;
        delete judge; judge = nullptr;
        delete merchant; merchant = nullptr;
    }

    void loadFont() {
        // Try multiple font paths for cross-platform compatibility
        vector<string> fontPaths = {
            "/System/Library/Fonts/Helvetica.ttc",  // macOS
            "C:\\Windows\\Fonts\\arial.ttf",        // Windows
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", // Linux
            "/usr/share/fonts/TTF/arial.ttf",       // Some Linux distros
            "arial.ttf"                             // Local file
        };

        bool fontLoaded = false;
        for (const string& path : fontPaths) {
            if (font.loadFromFile(path)) {
                fontLoaded = true;
                break;
            }
        }

        if (!fontLoaded) {
            cout << "Warning: Could not load any font, using default system font" << endl;
        }
    }

    void setupUI() {
        // Title
        titleText.setFont(font);
        titleText.setString("COUP GAME");
        titleText.setCharacterSize(48);
        titleText.setFillColor(textColor);
        titleText.setPosition(60, 30);

        // Game status
        gameStatusText.setFont(font);
        gameStatusText.setCharacterSize(22);
        gameStatusText.setFillColor(textColor);
        gameStatusText.setPosition(60, 100);

        // Player info
        playerInfoText.setFont(font);
        playerInfoText.setCharacterSize(20);
        playerInfoText.setFillColor(textColor);
        playerInfoText.setPosition(60, 160);

        // Action log
        actionLogText.setFont(font);
        actionLogText.setCharacterSize(18);
        actionLogText.setFillColor(sf::Color(200, 200, 200));
        actionLogText.setPosition(850, 160);

        // Setup action buttons
        actionNames = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup", "Special"};

        for (size_t i = 0; i < actionNames.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(150, 50));
            button.setPosition(60 + (i % 4) * 170, 550 + (i / 4) * 70);
            button.setFillColor(buttonColor);
            actionButtons.push_back(button);

            sf::Text buttonText;
            buttonText.setFont(font);
            buttonText.setString(actionNames[i]);
            buttonText.setCharacterSize(20);
            buttonText.setFillColor(textColor);

            // Center text in button
            sf::FloatRect textBounds = buttonText.getLocalBounds();
            buttonText.setPosition(
                button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
                button.getPosition().y + (button.getSize().y - textBounds.height) / 2 - 5
            );
            actionButtonTexts.push_back(buttonText);
        }
    }

    void initializeGame() {
        // Add players to the game
        game.add_player(governor);
        game.add_player(spy);
        game.add_player(baron);
        game.add_player(general);
        game.add_player(judge);
        game.add_player(merchant);

        addToLog("Game started!");
        addToLog("Players: Alice (Governor), Bob (Spy), Carol (Baron)");
        addToLog("David (General), Eve (Judge), Frank (Merchant)");
        addToLog("Each player starts with 2 coins.");
        updatePlayerButtons();
    }

    Player* getPlayerByName(const string& name) {
        if (governor && name == governor->get_name()) return governor;
        if (spy && name == spy->get_name()) return spy;
        if (baron && name == baron->get_name()) return baron;
        if (general && name == general->get_name()) return general;
        if (judge && name == judge->get_name()) return judge;
        if (merchant && name == merchant->get_name()) return merchant;
        return nullptr;
    }

    string getPlayerRole(Player* player) {
        if (player == governor) return "Governor";
        if (player == spy) return "Spy";
        if (player == baron) return "Baron";
        if (player == general) return "General";
        if (player == judge) return "Judge";
        if (player == merchant) return "Merchant";
        return "Unknown";
    }

    void updatePlayerButtons() {
        playerButtons.clear();
        playerButtonTexts.clear();

        vector<string> players = game.players();
        for (size_t i = 0; i < players.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(130, 45));
            button.setPosition(60 + i * 150, 720);

            // Highlight current player
            if (players[i] == game.turn().get_name()) {
                button.setFillColor(activePlayerColor);
            } else {
                button.setFillColor(buttonColor);
            }
            playerButtons.push_back(button);

            sf::Text buttonText;
            buttonText.setFont(font);
            buttonText.setString(players[i]);
            buttonText.setCharacterSize(18);
            buttonText.setFillColor(textColor);

            sf::FloatRect textBounds = buttonText.getLocalBounds();
            buttonText.setPosition(
                button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
                button.getPosition().y + (button.getSize().y - textBounds.height) / 2 - 3
            );
            playerButtonTexts.push_back(buttonText);
        }
    }

    void updateDisplay() {
        if (gameOver) return;

        // Update game status
        string status = "Current Turn: " + game.turn().get_name();
        status += " (" + getPlayerRole(&game.turn()) + ")";
        if (waitingForTarget) {
            status += " | Select target for " + pendingAction;
        }
        gameStatusText.setString(status);

        // Update player info with actual coin counts and roles
        stringstream playerInfo;
        vector<string> players = game.players();
        for (const string& playerName : players) {
            Player* player = getPlayerByName(playerName);
            if (player) {
                playerInfo << playerName << " (" << getPlayerRole(player) << "): "
                          << player->get_coins() << " coins";
                if (!player->is_active()) {
                    playerInfo << " [ELIMINATED]";
                }
                playerInfo << "\n";
            }
        }
        playerInfoText.setString(playerInfo.str());

        // Update action log
        actionLogText.setString(actionLog);
    }

    void addToLog(const string& message) {
        actionLog += message + "\n";

        // Keep only last 20 lines
        stringstream ss(actionLog);
        string line;
        vector<string> lines;
        while (getline(ss, line)) {
            lines.push_back(line);
        }

        if (lines.size() > 20) {
            lines.erase(lines.begin(), lines.begin() + (lines.size() - 20));
        }

        actionLog = "";
        for (const string& l : lines) {
            actionLog += l + "\n";
        }
    }

    bool canPerformAction(const string& action) {
        if (gameOver) return false;

        Player& currentPlayer = game.turn();

        if (action == "Coup") {
            return currentPlayer.get_coins() >= 7;
        }
        // Add other action validations as needed
        return true;
    }

    void advanceTurn() {
        // This assumes your Game class has a next_turn() method
        // If it doesn't, you might need to implement turn management differently
        try {
            // game.next_turn(); // Uncomment if this method exists

            // Alternative: manually advance to next active player
            // This is a placeholder - implement based on your Game class interface
            updatePlayerButtons();
        } catch (const exception& e) {
            addToLog("Error advancing turn: " + string(e.what()));
        }
    }

    void checkGameOver() {
        vector<string> remainingPlayers = game.players();
        int activePlayers = 0;
        string winner = "";

        for (const string& playerName : remainingPlayers) {
            Player* player = getPlayerByName(playerName);
            if (player && player->is_active()) {
                activePlayers++;
                winner = playerName;
            }
        }

        if (activePlayers <= 1) {
            gameOver = true;
            if (activePlayers == 1) {
                addToLog("GAME OVER! Winner: " + winner);
            } else {
                addToLog("GAME OVER! No players remaining!");
            }
        }
    }

    void handleActionButton(int buttonIndex) {
        if (waitingForTarget || gameOver) return;

        string action = actionNames[buttonIndex];

        if (!canPerformAction(action)) {
            addToLog("Cannot perform " + action + " - insufficient resources or invalid action");
            return;
        }

        Player& currentPlayer = game.turn();

        try {
            if (action == "Gather") {
                currentPlayer.gather();
                addToLog(currentPlayer.get_name() + " gathered 1 coin (now has " +
                        to_string(currentPlayer.get_coins()) + " coins)");
                advanceTurn();
            }
            else if (action == "Tax") {
                currentPlayer.tax();
                addToLog(currentPlayer.get_name() + " collected tax (" +
                        getPlayerRole(&currentPlayer) + " gets " +
                        (getPlayerRole(&currentPlayer) == "Governor" ? "3" : "2") +
                        " coins, now has " + to_string(currentPlayer.get_coins()) + " coins)");
                advanceTurn();
            }
            else if (action == "Bribe") {
                currentPlayer.bribe();
                addToLog(currentPlayer.get_name() + " used bribe (now has " +
                        to_string(currentPlayer.get_coins()) + " coins)");
                advanceTurn();
            }
            else if (action == "Special") {
                currentPlayer.use_special_ability();
                addToLog(currentPlayer.get_name() + " used " + getPlayerRole(&currentPlayer) +
                        " special ability (now has " + to_string(currentPlayer.get_coins()) + " coins)");
                advanceTurn();
            }
            else if (action == "Arrest" || action == "Sanction" || action == "Coup") {
                // These actions need a target
                waitingForTarget = true;
                pendingAction = action;
                selectedTarget = nullptr;
                addToLog("Select target for " + action + "...");
                return; // Don't advance turn yet
            }

            checkGameOver();

        }
        catch (const exception& e) {
            addToLog("Error: " + string(e.what()));
        }
    }

    void handlePlayerButton(int playerIndex) {
        if (!waitingForTarget || gameOver) return;

        vector<string> players = game.players();
        if (playerIndex >= players.size()) return;

        string targetName = players[playerIndex];
        Player* targetPlayer = getPlayerByName(targetName);
        Player& currentPlayer = game.turn();

        if (!targetPlayer) {
            addToLog("Error: Could not find target player");
            waitingForTarget = false;
            pendingAction = "";
            return;
        }

        // Can't target yourself for most actions
        if (targetPlayer == &currentPlayer && pendingAction != "Special") {
            addToLog("Cannot target yourself for " + pendingAction);
            return;
        }

        // Can't target eliminated players
        if (!targetPlayer->is_active()) {
            addToLog("Cannot target eliminated player " + targetName);
            return;
        }

        try {
            if (pendingAction == "Coup") {
                if (currentPlayer.get_coins() < 7) {
                    addToLog("Need 7 coins to coup (you have " + to_string(currentPlayer.get_coins()) + ")");
                    waitingForTarget = false;
                    pendingAction = "";
                    return;
                }

                currentPlayer.coup(*targetPlayer);
                addToLog(currentPlayer.get_name() + " coup'd " + targetName +
                        " (paid 7 coins, now has " + to_string(currentPlayer.get_coins()) + " coins)");
                if (!targetPlayer->is_active()) {
                    addToLog(targetName + " has been eliminated!");
                }
            }
            else if (pendingAction == "Arrest") {
                currentPlayer.arrest(*targetPlayer);
                addToLog(currentPlayer.get_name() + " arrested " + targetName +
                        " (now has " + to_string(currentPlayer.get_coins()) + " coins)");
            }
            else if (pendingAction == "Sanction") {
                currentPlayer.sanction(*targetPlayer);
                addToLog(currentPlayer.get_name() + " sanctioned " + targetName);
            }

            waitingForTarget = false;
            pendingAction = "";
            selectedTarget = nullptr;

            advanceTurn();
            checkGameOver();

        }
        catch (const exception& e) {
            addToLog("Error: " + string(e.what()));
            waitingForTarget = false;
            pendingAction = "";
            selectedTarget = nullptr;
        }
    }

    void handleMouseClick(sf::Vector2f mousePos) {
        if (gameOver) return;

        // Check action buttons
        for (size_t i = 0; i < actionButtons.size(); ++i) {
            if (actionButtons[i].getGlobalBounds().contains(mousePos)) {
                handleActionButton(i);
                return;
            }
        }

        // Check player buttons
        for (size_t i = 0; i < playerButtons.size(); ++i) {
            if (playerButtons[i].getGlobalBounds().contains(mousePos)) {
                handlePlayerButton(i);
                return;
            }
        }
    }

    void updateButtonColors() {
        // Update action button colors based on availability
        for (size_t i = 0; i < actionButtons.size(); ++i) {
            if (gameOver || !canPerformAction(actionNames[i]) || waitingForTarget) {
                actionButtons[i].setFillColor(disabledButtonColor);
            } else {
                actionButtons[i].setFillColor(buttonColor);
            }
        }
    }

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                        handleMouseClick(mousePos);
                    }
                }
            }

            updateDisplay();
            updateButtonColors();

            // Render
            window.clear(backgroundColor);

            window.draw(titleText);
            window.draw(gameStatusText);
            window.draw(playerInfoText);
            window.draw(actionLogText);

            // Draw action buttons
            for (size_t i = 0; i < actionButtons.size(); ++i) {
                window.draw(actionButtons[i]);
                window.draw(actionButtonTexts[i]);
            }

            // Draw player buttons
            for (size_t i = 0; i < playerButtons.size(); ++i) {
                window.draw(playerButtons[i]);
                window.draw(playerButtonTexts[i]);
            }

            window.display();
        }
    }
};

int main() {
    try {
        CoupGUI gui;
        gui.run();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}