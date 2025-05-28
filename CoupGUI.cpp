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
    
    // Colors
    sf::Color backgroundColor = sf::Color(30, 30, 40);
    sf::Color buttonColor = sf::Color(70, 70, 90);
    sf::Color buttonHoverColor = sf::Color(90, 90, 120);
    sf::Color activePlayerColor = sf::Color(100, 150, 100);
    sf::Color textColor = sf::Color::White;
    
public:
    CoupGUI() : window(sf::VideoMode(1200, 800), "Coup Game"), waitingForTarget(false) {
        // Load default font
        if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
            // Try alternative font paths for different systems
            if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
                if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                    cout << "Warning: Could not load font, using default" << endl;
                }
            }
        }
        
        setupUI();
        initializeGame();
    }
    
    void setupUI() {
        // Title
        titleText.setFont(font);
        titleText.setString("COUP GAME");
        titleText.setCharacterSize(36);
        titleText.setFillColor(textColor);
        titleText.setPosition(50, 20);
        
        // Game status
        gameStatusText.setFont(font);
        gameStatusText.setCharacterSize(18);
        gameStatusText.setFillColor(textColor);
        gameStatusText.setPosition(50, 80);
        
        // Player info
        playerInfoText.setFont(font);
        playerInfoText.setCharacterSize(16);
        playerInfoText.setFillColor(textColor);
        playerInfoText.setPosition(50, 130);
        
        // Action log
        actionLogText.setFont(font);
        actionLogText.setCharacterSize(14);
        actionLogText.setFillColor(sf::Color(200, 200, 200));
        actionLogText.setPosition(650, 130);
        
        // Setup action buttons
        actionNames = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup", "Special"};
        
        for (size_t i = 0; i < actionNames.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(120, 40));
            button.setPosition(50 + (i % 4) * 130, 450 + (i / 4) * 50);
            button.setFillColor(buttonColor);
            actionButtons.push_back(button);
            
            sf::Text buttonText;
            buttonText.setFont(font);
            buttonText.setString(actionNames[i]);
            buttonText.setCharacterSize(16);
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
        // Create players with different roles
        static Governor governor("Alice");
        static Spy spy("Bob");
        static Baron baron("Carol");
        static General general("David");
        static Judge judge("Eve");
        
        game.add_player(&governor);
        game.add_player(&spy);
        game.add_player(&baron);
        game.add_player(&general);
        game.add_player(&judge);
        
        addToLog("Game started! Players: Alice, Bob, Carol, David, Eve");
        updatePlayerButtons();
    }
    
    void updatePlayerButtons() {
        playerButtons.clear();
        playerButtonTexts.clear();
        
        vector<string> players = game.players();
        for (size_t i = 0; i < players.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(100, 35));
            button.setPosition(50 + i * 110, 580);
            
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
            buttonText.setCharacterSize(14);
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
        // Update game status
        string status = "Current Turn: " + game.turn().get_name();
        if (waitingForTarget) {
            status += " | Select target for " + pendingAction;
        }
        gameStatusText.setString(status);
        
        // Update player info
        stringstream playerInfo;
        vector<string> players = game.players();
        for (const string& playerName : players) {
            // Find the player object to get coin count
            for (size_t i = 0; i < players.size(); ++i) {
                if (players[i] == playerName) {
                    // This is a simplified way - you might need to store player pointers
                    playerInfo << playerName << ": " << "Coins: ?" << " | ";
                    break;
                }
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
    
    void handleActionButton(int buttonIndex) {
        if (waitingForTarget) return;
        
        string action = actionNames[buttonIndex];
        Player& currentPlayer = game.turn();
        
        try {
            if (action == "Gather") {
                currentPlayer.gather();
                addToLog(currentPlayer.get_name() + " gathered coins");
            }
            else if (action == "Tax") {
                currentPlayer.tax();
                addToLog(currentPlayer.get_name() + " collected tax");
            }
            else if (action == "Bribe") {
                currentPlayer.bribe();
                addToLog(currentPlayer.get_name() + " used bribe");
            }
            else if (action == "Special") {
                currentPlayer.use_special_ability();
                addToLog(currentPlayer.get_name() + " used special ability");
            }
            else if (action == "Arrest" || action == "Sanction" || action == "Coup") {
                // These actions need a target
                waitingForTarget = true;
                pendingAction = action;
                addToLog("Select target for " + action);
                return; // Don't advance turn yet
            }
            
            updatePlayerButtons();
        }
        catch (const exception& e) {
            addToLog("Error: " + string(e.what()));
        }
    }
    
    void handlePlayerButton(int playerIndex) {
        if (!waitingForTarget) return;
        
        vector<string> players = game.players();
        if (playerIndex >= players.size()) return;
        
        string targetName = players[playerIndex];
        Player& currentPlayer = game.turn();
        
        // Find target player (this is simplified - you might need better player management)
        try {
            if (pendingAction == "Coup") {
                // For coup, we need the actual player object
                addToLog(currentPlayer.get_name() + " attempted to coup " + targetName);
            }
            else if (pendingAction == "Arrest") {
                addToLog(currentPlayer.get_name() + " attempted to arrest " + targetName);
            }
            else if (pendingAction == "Sanction") {
                addToLog(currentPlayer.get_name() + " attempted to sanction " + targetName);
            }
            
            waitingForTarget = false;
            pendingAction = "";
            updatePlayerButtons();
        }
        catch (const exception& e) {
            addToLog("Error: " + string(e.what()));
            waitingForTarget = false;
            pendingAction = "";
        }
    }
    
    void handleMouseClick(sf::Vector2f mousePos) {
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