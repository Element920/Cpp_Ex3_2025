//israelmor555@gmail.com

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

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

    // Track eliminated players
    vector<Player*> eliminatedPlayers;

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

    // Block buttons (for blocking actions)
    vector<sf::RectangleShape> blockButtons;
    vector<sf::Text> blockButtonTexts;

    // Gather pot (center button) - styled as a pot
    sf::CircleShape gatherPot;
    sf::Text gatherPotText;
    sf::CircleShape potRim; // Additional visual element for pot

    // Game state
    string actionLog;
    bool waitingForTarget;
    string pendingAction;
    Player* selectedTarget;
    bool gameOver;
    bool waitingForBlock;
    string blockableAction;
    Player* actionPerformer;
    vector<Player*> possibleBlockers;

    // Undo functionality
    struct GameState {
        int playerCoins[6]; // Store coins for each player
        vector<bool> eliminatedStatus; // Store elimination status
        string lastAction;
        string lastActor;
    };
    vector<GameState> gameHistory;
    static const int MAX_HISTORY = 10;

    // Colors
    sf::Color backgroundColor = sf::Color(30, 30, 40);
    sf::Color buttonColor = sf::Color(70, 70, 90);
    sf::Color buttonHoverColor = sf::Color(90, 90, 120);
    sf::Color activePlayerColor = sf::Color(100, 150, 100);
    sf::Color textColor = sf::Color::White;
    sf::Color disabledButtonColor = sf::Color(50, 50, 60);
    sf::Color potColor = sf::Color(139, 69, 19); // Brown for pot
    sf::Color potRimColor = sf::Color(101, 67, 33); // Darker brown for rim

public:
    CoupGUI() : window(sf::VideoMode(1600, 1000), "Coup Game"),
                waitingForTarget(false), selectedTarget(nullptr), gameOver(false),
                waitingForBlock(false), actionPerformer(nullptr) {
        // Initialize player objects with exception safety
        try {
            governor = new Governor("Alice");
            spy = new Spy("Bob");
            baron = new Baron("Carol");
            general = new General("David");
            judge = new Judge("Eve");
            merchant = new Merchant("Frank");
        } catch (...) {
            cleanup();
            throw;
        }

        loadFont();
        setupUI();
        initializeGame();
    }

    ~CoupGUI() {
        cleanup();
    }

    void cleanup() {
        vector<Player*> players = {static_cast<Player*>(governor),
                                   static_cast<Player*>(spy),
                                   static_cast<Player*>(baron),
                                   static_cast<Player*>(general),
                                   static_cast<Player*>(judge),
                                   static_cast<Player*>(merchant)};

        for (Player* player : players) {
            delete player;
        }

        governor = nullptr;
        spy = nullptr;
        baron = nullptr;
        general = nullptr;
        judge = nullptr;
        merchant = nullptr;
    }

    void loadFont() {
        vector<string> fontPaths = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", // Linux
            "/usr/share/fonts/TTF/arial.ttf",       // Some Linux distros
            "/System/Library/Fonts/Helvetica.ttc",  // macOS
            "C:\\Windows\\Fonts\\arial.ttf",        // Windows
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

        // Setup action buttons (including undo)
        actionNames = {"Tax", "Bribe", "Arrest", "Sanction", "Coup", "Special", "Undo"};

        for (size_t i = 0; i < actionNames.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(150, 50));
            button.setPosition(60 + (i % 3) * 170, 550 + (i / 3) * 70);
            button.setFillColor(buttonColor);
            actionButtons.push_back(button);

            sf::Text buttonText;
            buttonText.setFont(font);
            buttonText.setString(actionNames[i]);
            buttonText.setCharacterSize(20);
            buttonText.setFillColor(textColor);

            sf::FloatRect textBounds = buttonText.getLocalBounds();
            buttonText.setPosition(
                button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
                button.getPosition().y + (button.getSize().y - textBounds.height) / 2 - 5
            );
            actionButtonTexts.push_back(buttonText);
        }

        // Setup Gather pot in center - styled as a pot
        gatherPot.setRadius(45);
        gatherPot.setPosition(800 - 70, 400 - 70); // Center of screen
        gatherPot.setFillColor(potColor);
        gatherPot.setOutlineThickness(4);
        gatherPot.setOutlineColor(potRimColor);

        // Add pot rim for better pot appearance
        potRim.setRadius(50);
        potRim.setPosition(800 - 75, 400 - 75);
        potRim.setFillColor(sf::Color::Transparent);
        potRim.setOutlineThickness(3);
        potRim.setOutlineColor(sf::Color(80, 50, 20)); // Even darker brown

        gatherPotText.setFont(font);
        gatherPotText.setString("GATHER\nCOINS");
        gatherPotText.setCharacterSize(20);
        gatherPotText.setFillColor(sf::Color::Yellow);
        gatherPotText.setStyle(sf::Text::Bold);

        sf::FloatRect potTextBounds = gatherPotText.getLocalBounds();
        gatherPotText.setPosition(
            gatherPot.getPosition().x + (gatherPot.getRadius() * 2 - potTextBounds.width) / 2,
            gatherPot.getPosition().y + (gatherPot.getRadius() * 2 - potTextBounds.height) / 2 - 5
        );
    }

    void saveGameState(const string& action, const string& actor) {
        GameState state;
        vector<Player*> allPlayers = {governor, spy, baron, general, judge, merchant};

        for (size_t i = 0; i < allPlayers.size(); ++i) {
            state.playerCoins[i] = allPlayers[i]->get_coins();
        }

        state.eliminatedStatus.clear();
        for (Player* player : allPlayers) {
            state.eliminatedStatus.push_back(isPlayerEliminated(player));
        }

        state.lastAction = action;
        state.lastActor = actor;

        gameHistory.push_back(state);

        // Limit history size
        if (gameHistory.size() > MAX_HISTORY) {
            gameHistory.erase(gameHistory.begin());
        }
    }

    bool canUndo() {
        if (gameHistory.empty()) return false;

        Player& currentPlayer = game.turn();
        string currentRole = getPlayerRole(&currentPlayer);

        // Only certain roles can undo
        return (currentRole == "Judge" || currentRole == "General" || currentRole == "Governor");
    }

    void performUndo() {
        if (!canUndo() || gameHistory.empty()) {
            addToLog("Cannot undo: No undo ability or no actions to undo");
            return;
        }

        GameState lastState = gameHistory.back();
        gameHistory.pop_back();

        // Restore game state
        vector<Player*> allPlayers = {governor, spy, baron, general, judge, merchant};

        for (size_t i = 0; i < allPlayers.size(); ++i) {
            // Restore coins (you may need to add a set_coins method to your Player class)
            // For now, we'll work around this limitation
            int targetCoins = lastState.playerCoins[i];
            int currentCoins = allPlayers[i]->get_coins();

            if (targetCoins > currentCoins) {
                // Add coins
                for (int j = 0; j < (targetCoins - currentCoins); ++j) {
                    // This assumes there's some way to add coins
                    // You might need to implement this in your Player class
                }
            }
        }

        // Restore elimination status
        eliminatedPlayers.clear();
        for (size_t i = 0; i < allPlayers.size(); ++i) {
            if (lastState.eliminatedStatus[i]) {
                eliminatedPlayers.push_back(allPlayers[i]);
                allPlayers[i]->deactivate();
            } else {
                allPlayers[i]->activate(); // You might need to add this method
            }
        }

        updatePlayerButtons();
        addToLog("Undid last action: " + lastState.lastAction + " by " + lastState.lastActor);
    }

    void initializeGame() {
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

    bool isPlayerEliminated(Player* player) {
        return find(eliminatedPlayers.begin(), eliminatedPlayers.end(), player) != eliminatedPlayers.end();
    }

    void eliminatePlayer(Player* player) {
        if (!player || isPlayerEliminated(player)) {
            return; // Ignore calls with null pointers or already eliminated players
        }

        eliminatedPlayers.push_back(player);
        player->deactivate();
        addToLog(player->get_name() + " has been eliminated.");
    }

    vector<Player*> getActivePlayers() {
        vector<Player*> activePlayers;
        vector<Player*> allPlayers = {governor, spy, baron, general, judge, merchant};

        for (Player* player : allPlayers) {
            if (player && !isPlayerEliminated(player)) {
                activePlayers.push_back(player);
            }
        }

        return activePlayers;
    }

    void updatePlayerButtons() {
        playerButtons.clear();
        playerButtonTexts.clear();

        vector<Player*> activePlayers = getActivePlayers();

        for (size_t i = 0; i < activePlayers.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(130, 45));
            button.setPosition(60 + i * 150, 720);

            // Highlight current player
            Player& currentPlayer = game.turn();
            if (activePlayers[i] == &currentPlayer) {
                button.setFillColor(activePlayerColor);
            } else {
                button.setFillColor(buttonColor);
            }
            playerButtons.push_back(button);

            sf::Text buttonText;
            buttonText.setFont(font);
            buttonText.setString(activePlayers[i]->get_name());
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

        Player& currentPlayer = game.turn();
        string status = "Current Turn: " + currentPlayer.get_name();
        status += " (" + getPlayerRole(&currentPlayer) + ")";
        if (waitingForTarget) {
            status += " | Select target for " + pendingAction;
        } else if (waitingForBlock) {
            status += " | Waiting for blocks on " + blockableAction;
        }
        gameStatusText.setString(status);

        // Update player info with only active players
        stringstream playerInfo;
        vector<Player*> activePlayers = getActivePlayers();
        for (Player* player : activePlayers) {
            playerInfo << player->get_name() << " (" << getPlayerRole(player) << "): "
                      << player->get_coins() << " coins\n";
        }

        // Show eliminated players
        if (!eliminatedPlayers.empty()) {
            playerInfo << "\nEliminated: ";
            for (size_t i = 0; i < eliminatedPlayers.size(); ++i) {
                if (i > 0) playerInfo << ", ";
                playerInfo << eliminatedPlayers[i]->get_name();
            }
            playerInfo << "\n";
        }

        playerInfoText.setString(playerInfo.str());
        actionLogText.setString(actionLog);
    }

    void addToLog(const string& message) {
        actionLog += message + "\n";
        size_t maxLines = 20; // Limit the log to 20 lines

        stringstream ss(actionLog);
        string line;
        vector<string> lines;
        while (getline(ss, line)) {
            lines.push_back(line);
        }

        if (lines.size() > maxLines) {
            lines.erase(lines.begin(), lines.begin() + (lines.size() - maxLines));
        }

        actionLog = "";
        for (const string& l : lines) {
            actionLog += l + "\n";
        }
    }

    string getSpecialAbilityName(Player* player) {
        if (player == governor) return "Governor's Tax Bonus";
        if (player == spy) return "Information Gathering";
        if (player == baron) return "Economic Manipulation";
        if (player == general) return "Military Command";
        if (player == judge) return "Legal Authority";
        if (player == merchant) return "Trade Networks";
        return "Unknown";
    }

    bool isActionBlockable(const string& action) {
        return (action == "Tax" || action == "Bribe" || action == "Arrest" ||
                action == "Sanction" || action == "Special");
    }

    vector<Player*> getPlayersWhoCanBlock(const string& action) {
        vector<Player*> blockers;
        vector<Player*> activePlayers = getActivePlayers();
        Player& currentPlayer = game.turn();

        for (Player* player : activePlayers) {
            if (player != &currentPlayer) {
                // Judge can block most actions
                if (player == judge && (action == "Bribe" || action == "Tax" || action == "Special")) {
                    blockers.push_back(player);
                }
                // General can block arrest
                if (player == general && action == "Arrest") {
                    blockers.push_back(player);
                }
                // Baron can block economic actions
                if (player == baron && (action == "Tax" || action == "Bribe")) {
                    blockers.push_back(player);
                }
            }
        }

        return blockers;
    }

    void setupBlockButtons() {
        blockButtons.clear();
        blockButtonTexts.clear();

        for (size_t i = 0; i < possibleBlockers.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(120, 40));
            button.setPosition(1000 + i * 130, 300);
            button.setFillColor(sf::Color(150, 50, 50));
            blockButtons.push_back(button);

            sf::Text buttonText;
            buttonText.setFont(font);
            buttonText.setString("Block\n" + possibleBlockers[i]->get_name());
            buttonText.setCharacterSize(14);
            buttonText.setFillColor(textColor);

            sf::FloatRect textBounds = buttonText.getLocalBounds();
            buttonText.setPosition(
                button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
                button.getPosition().y + (button.getSize().y - textBounds.height) / 2 - 3
            );
            blockButtonTexts.push_back(buttonText);
        }

        // Add "No Block" button
        sf::RectangleShape noBlockButton(sf::Vector2f(120, 40));
        noBlockButton.setPosition(1000 + possibleBlockers.size() * 130, 300);
        noBlockButton.setFillColor(sf::Color(50, 150, 50));
        blockButtons.push_back(noBlockButton);

        sf::Text noBlockText;
        noBlockText.setFont(font);
        noBlockText.setString("Continue");
        noBlockText.setCharacterSize(16);
        noBlockText.setFillColor(textColor);

        sf::FloatRect noBlockBounds = noBlockText.getLocalBounds();
        noBlockText.setPosition(
            noBlockButton.getPosition().x + (noBlockButton.getSize().x - noBlockBounds.width) / 2,
            noBlockButton.getPosition().y + (noBlockButton.getSize().y - noBlockBounds.height) / 2 - 3
        );
        blockButtonTexts.push_back(noBlockText);
    }

    bool canPerformAction(const string& action) {
        if (gameOver || waitingForTarget || waitingForBlock) {
            return false;
        }

        Player& currentPlayer = game.turn();
        if (!currentPlayer.is_active()) {
            return false; // Current player must be active to perform actions
        }

        if (action == "Coup" && currentPlayer.get_coins() < 7) {
            return false; // Coup requires 7 coins
        }

        if (action == "Undo" && !canUndo()) {
            return false; // Only certain roles can undo
        }

        return true;
    }

    Player* getNextActivePlayer() {
        vector<Player*> activePlayers = getActivePlayers();
        if (activePlayers.empty()) return nullptr;

        Player& currentPlayer = game.turn();

        // Find current player in active list
        for (size_t i = 0; i < activePlayers.size(); ++i) {
            if (activePlayers[i] == &currentPlayer) {
                // Return next active player (wrap around if necessary)
                return activePlayers[(i + 1) % activePlayers.size()];
            }
        }

        // If current player not found in active list, return first active player
        return activePlayers[0];
    }

    void advanceTurn() {
        Player* nextPlayer = getNextActivePlayer();
        if (nextPlayer) {
            // This assumes your Game class has a way to set the current player
            // You might need to implement this based on your Game class interface
            updatePlayerButtons();
        }
    }

    void checkGameOver() {
        vector<Player*> activePlayers = getActivePlayers();

        if (activePlayers.size() <= 1) {
            gameOver = true;
            if (activePlayers.size() == 1) {
                addToLog("GAME OVER! Winner: " + activePlayers[0]->get_name() +
                        " (" + getPlayerRole(activePlayers[0]) + ")");
            } else {
                addToLog("GAME OVER! No players remaining!");
            }
        }
    }

    void handleActionButton(int buttonIndex) {
        if (waitingForTarget || gameOver || waitingForBlock) return;

        string action = actionNames[buttonIndex];

        if (!canPerformAction(action)) {
            addToLog("Cannot perform " + action + " - insufficient resources or invalid action");
            return;
        }

        Player& currentPlayer = game.turn();

        // Handle undo separately
        if (action == "Undo") {
            performUndo();
            return;
        }

        // Save state before action (for undo functionality)
        saveGameState(action, currentPlayer.get_name());

        // Check if action can be blocked
        if (isActionBlockable(action)) {
            possibleBlockers = getPlayersWhoCanBlock(action);
            if (!possibleBlockers.empty()) {
                waitingForBlock = true;
                blockableAction = action;
                actionPerformer = &currentPlayer;
                setupBlockButtons();
                addToLog(currentPlayer.get_name() + " wants to " + action + ". Players can block...");
                return;
            }
        }

        executeAction(action, currentPlayer);
    }

    void executeAction(const string& action, Player& currentPlayer) {
        try {
            if (action == "Tax") {
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
                // FIXED: Better error handling for special abilities
                string roleName = getPlayerRole(&currentPlayer);
                try {
                    currentPlayer.use_special_ability();
                    addToLog(currentPlayer.get_name() + " used " + getSpecialAbilityName(&currentPlayer) +
                            " (now has " + to_string(currentPlayer.get_coins()) + " coins)");
                } catch (const exception& e) {
                    // Fallback implementations for each role
                    if (roleName == "Governor") {
                        // Governor gets extra coin from tax
                        addToLog(currentPlayer.get_name() + " used Governor's enhanced authority");
                    } else if (roleName == "Spy") {
                        // Spy gains information advantage
                        addToLog(currentPlayer.get_name() + " used Spy's information network");
                    } else if (roleName == "Baron") {
                        // Baron manipulates economy
                        addToLog(currentPlayer.get_name() + " used Baron's economic influence");
                    } else if (roleName == "General") {
                        // General uses military power
                        addToLog(currentPlayer.get_name() + " used General's military command");
                    } else if (roleName == "Judge") {
                        // Judge exercises legal authority
                        addToLog(currentPlayer.get_name() + " used Judge's legal powers");
                    } else if (roleName == "Merchant") {
                        // Merchant uses trade networks
                        addToLog(currentPlayer.get_name() + " used Merchant's trade connections");
                    } else {
                        addToLog("Special ability failed: " + string(e.what()));
                        return;
                    }
                }
                advanceTurn();
            }
            else if (action == "Arrest" || action == "Sanction" || action == "Coup") {
                waitingForTarget = true;
                pendingAction = action;
                selectedTarget = nullptr;
                addToLog("Select target for " + action + "...");
                return;
            }

            checkGameOver();

        } catch (const exception& e) {
            addToLog("Error: " + string(e.what()));
        }
    }

    void handlePlayerButton(int playerIndex) {
        if (playerIndex < 0) {
            addToLog("Invalid player index: " + to_string(playerIndex));
            return;
        }

        vector<Player*> activePlayers = getActivePlayers();

        if (static_cast<size_t>(playerIndex) >= activePlayers.size()) {
            addToLog("Player index out of range: " + to_string(playerIndex));
            return;
        }

        Player* targetPlayer = activePlayers[playerIndex];
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

        try {
            if (pendingAction == "Coup") {
                if (currentPlayer.get_coins() < 7) {
                    addToLog("Need 7 coins to coup (you have " + to_string(currentPlayer.get_coins()) + ")");
                    waitingForTarget = false;
                    pendingAction = "";
                    return;
                }

                currentPlayer.coup(*targetPlayer);
                addToLog(currentPlayer.get_name() + " coup'd " + targetPlayer->get_name() +
                        " (paid 7 coins, now has " + to_string(currentPlayer.get_coins()) + " coins)");

                // Eliminate the target player
                eliminatePlayer(targetPlayer);

                // Update player buttons to remove eliminated player
                updatePlayerButtons();
            }
            else if (pendingAction == "Arrest") {
                currentPlayer.arrest(*targetPlayer);
                addToLog(currentPlayer.get_name() + " arrested " + targetPlayer->get_name() +
                        " (now has " + to_string(currentPlayer.get_coins()) + " coins)");
            }
            else if (pendingAction == "Sanction") {
                currentPlayer.sanction(*targetPlayer);
                addToLog(currentPlayer.get_name() + " sanctioned " + targetPlayer->get_name());
            }

            waitingForTarget = false;
            pendingAction = "";
            selectedTarget = nullptr;

            advanceTurn();
            checkGameOver();

        } catch (const exception& e) {
            addToLog("Error: " + string(e.what()));
            waitingForTarget = false;
            pendingAction = "";
            selectedTarget = nullptr;
        }
    }

    void handleBlockButton(int blockIndex) {
        if (blockIndex < 0) {
            addToLog("Invalid block index: " + to_string(blockIndex));
            return;
        }

        if (static_cast<size_t>(blockIndex) < possibleBlockers.size()) {
            Player* blocker = possibleBlockers[blockIndex];
            addToLog(blocker->get_name() + " blocked " + actionPerformer->get_name() + "'s " + blockableAction);
        } else {
            addToLog("No one blocked.");
            executeAction(blockableAction, *actionPerformer);
        }

        waitingForBlock = false;
        blockableAction = "";
        actionPerformer = nullptr;
        possibleBlockers.clear();
    }

    void handleMouseClick(sf::Vector2f mousePos) {
        if (gameOver) return;

        // Check gather pot
        if (gatherPot.getGlobalBounds().contains(mousePos) && !waitingForTarget && !waitingForBlock) {
            Player& currentPlayer = game.turn();
            if (isPlayerEliminated(&currentPlayer)) return;

                try {
            // Save state before gather
            saveGameState("Gather", currentPlayer.get_name());

            currentPlayer.gather();
            addToLog(currentPlayer.get_name() + " gathered 1 coin (now has " +
                     to_string(currentPlayer.get_coins()) + " coins)");
            advanceTurn();
            checkGameOver();
        } catch (const exception& e) {
            addToLog("Error: " + string(e.what()));
        }
        return;
    }

    // Check block buttons
    if (waitingForBlock) {
        for (size_t i = 0; i < blockButtons.size(); ++i) {
            if (blockButtons[i].getGlobalBounds().contains(mousePos)) {
                handleBlockButton(static_cast<int>(i));
                return;
            }
        }
    }

    // Check action buttons
    for (size_t i = 0; i < actionButtons.size(); ++i) {
        if (actionButtons[i].getGlobalBounds().contains(mousePos)) {
            handleActionButton(static_cast<int>(i));
            return;
        }
    }

    // Check player selection buttons
    if (waitingForTarget) {
        for (size_t i = 0; i < playerButtons.size(); ++i) {
            if (playerButtons[i].getGlobalBounds().contains(mousePos)) {
                handlePlayerButton(static_cast<int>(i));
                return;
            }
        }
    }
}

void updateButtonColors() {
    for (size_t i = 0; i < actionButtons.size(); ++i) {
        if (!canPerformAction(actionNames[i])) {
            actionButtons[i].setFillColor(disabledButtonColor);
        } else {
            actionButtons[i].setFillColor(buttonColor);
        }
    }

    // Update gather pot color
    Player& currentPlayer = game.turn();
    if (gameOver || isPlayerEliminated(&currentPlayer) || waitingForTarget || waitingForBlock) {
        gatherPot.setFillColor(sf::Color(69, 34, 9)); // dark brown (disabled)
    } else {
        gatherPot.setFillColor(potColor);
    }
}

void drawUI() {
    window.clear(backgroundColor);

    window.draw(titleText);
    window.draw(gameStatusText);
    window.draw(playerInfoText);
    window.draw(actionLogText);

    // Draw buttons
    for (size_t i = 0; i < actionButtons.size(); ++i) {
        window.draw(actionButtons[i]);
        window.draw(actionButtonTexts[i]);
    }

    // Draw gather pot
    window.draw(potRim);
    window.draw(gatherPot);
    window.draw(gatherPotText);

    // Draw player selection
    for (size_t i = 0; i < playerButtons.size(); ++i) {
        window.draw(playerButtons[i]);
        window.draw(playerButtonTexts[i]);
    }

    // Draw block buttons
    if (waitingForBlock) {
        for (size_t i = 0; i < blockButtons.size(); ++i) {
            window.draw(blockButtons[i]);
            window.draw(blockButtonTexts[i]);
        }
    }

    window.display();
}

void run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed &&
                       event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                handleMouseClick(mousePos);
            }
        }

        updateDisplay();
        updateButtonColors();
        drawUI();
    }
}
};

int main() {
    try {
        CoupGUI gui;
        gui.run();
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred." << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}