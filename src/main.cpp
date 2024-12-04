#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "TransactionGraph.h"

using namespace std;
using namespace sf;

class TextField {
public:
    RectangleShape box;
    Text text;
    bool isActive = false;
    bool isTyping = false;
    string inputString;
    Clock cursorClock;
    int cursorIndex = 0;

    TextField(float x, float y, float width, float height, Font& font, const string& initialText = "") {
        box.setPosition(x, y);
        box.setSize(Vector2f(width, height));
        box.setFillColor(Color::White);
        box.setOutlineColor(Color::Black);
        box.setOutlineThickness(2);

        text.setFont(font);
        text.setCharacterSize(16);
        text.setString(initialText);
        text.setFillColor(Color::Black);
        text.setPosition(x + 5, y + 5); // Small padding inside box

        inputString = initialText;
    }

    void handleEvent(Event event) {
        if (isActive) {
            if (event.type == Event::TextEntered) {
                if (event.text.unicode == '\b' && inputString.length() > 0) {
                    inputString.pop_back();  // Handle backspace
                }
                else if (event.text.unicode < 128) {
                    inputString += static_cast<char>(event.text.unicode);  // Add the typed character
                }
                text.setString(inputString);
                cursorClock.restart();  // Reset cursor blink timer on each keypress
            }
        }
    }

    void draw(RenderWindow& window) {
        window.draw(box);
        window.draw(text);

        // Display cursor if the text field is active
        if (isActive && cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            int cursorPos = text.getLocalBounds().width;
            RectangleShape cursor(Vector2f(2, text.getCharacterSize())); // Cursor width and height
            cursor.setPosition(text.getPosition().x + cursorPos, text.getPosition().y);
            cursor.setFillColor(Color::Black);
            window.draw(cursor);
            if (cursorClock.getElapsedTime().asSeconds() > 1.0f) {
                cursorClock.restart();  // Reset cursor blink cycle
            }
        }
    }

    bool contains(float x, float y) {
        return box.getGlobalBounds().contains(x, y);
    }

    string getText() {
        return inputString;
    }

    void setActive(bool active) {
        isActive = active;
    }

    void reset() {
        inputString.clear();
        text.setString(inputString);
    }
};

class ScrollableTextArea {
    RectangleShape background;
    Font font;
    Text content;
    View view;
    RenderWindow& window;

public:
    ScrollableTextArea(RenderWindow& win, FloatRect bounds) : window(win) {
        background.setPosition(bounds.left, bounds.top);
        background.setSize(Vector2f(bounds.width, bounds.height));
        background.setFillColor(Color(220, 220, 220));

        font.loadFromFile("Arial.ttf"); // Replace with your font path
        content.setFont(font);
        content.setCharacterSize(14);
        content.setFillColor(Color::Black);

        view.reset(FloatRect(0, 0, bounds.width, bounds.height));
        view.setViewport(FloatRect(bounds.left / win.getSize().x, bounds.top / win.getSize().y,
                                   bounds.width / win.getSize().x, bounds.height / win.getSize().y));
    }

    void addText(const string& text) {
        content.setString(content.getString() + text + "\n");
    }

    void draw() {
        window.setView(view);
        window.draw(background);
        window.draw(content);
        window.setView(window.getDefaultView());
    }

    void handleScroll(float delta) {
        view.move(0, delta);
        window.setView(view);
    }
};

int main() {
    RenderWindow window(VideoMode(800, 600), "Fraud Patrol");
    window.setFramerateLimit(60);

    // Font and Text Fields
    Font font;
    font.loadFromFile("Arial.ttf"); // Replace with your font path

    // Create input fields with labels
    TextField nodesField(150, 50, 150, 30, font, "10000");
    TextField edgesField(150, 100, 150, 30, font, "100000");

    // Labels
    Text labels[5];
    string inputNames[5] = { "Nodes", "Edges"};
    for (int i = 0; i < 2; ++i) {
        labels[i].setFont(font);
        labels[i].setString(inputNames[i] + ":");
        labels[i].setPosition(50, 50 + i * 50);
        labels[i].setCharacterSize(16);
        labels[i].setFillColor(Color::Black);
    }

    // Run button
    RectangleShape runButton(Vector2f(100, 40));
    runButton.setPosition(50, 150);
    runButton.setFillColor(Color(100, 200, 100));

    Text runText("Run", font, 20);
    runText.setPosition(75, 160);
    runText.setFillColor(Color::Black);

    // Scrollable results area
    ScrollableTextArea resultsArea(window, FloatRect(400, 50, 350, 500));

    // Keep track of the active text field
    TextField* activeField = nullptr;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::TextEntered) {
                if (activeField != nullptr) {
                    activeField->handleEvent(event);
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                // Set active text field based on mouse click
                if (nodesField.contains(event.mouseButton.x, event.mouseButton.y)) {
                    nodesField.setActive(true);
                    edgesField.setActive(false);
                    activeField = &nodesField;
                }
                else if (edgesField.contains(event.mouseButton.x, event.mouseButton.y)) {
                    edgesField.setActive(true);
                    nodesField.setActive(false);
                    activeField = &edgesField;
                }

                // Run button click detection
                if (runButton.getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window)))) {
                    int nodes = stoi(nodesField.getText());
                    int edges = stoi(edgesField.getText());

                    // Generate graph
                    TransactionGraph graph(nodes, edges);
                    auto adjacencyList = graph.getGraph();

                    // Run Tarjan's algorithm
                    auto start = chrono::high_resolution_clock::now();
                    TarjanCycle tarjan(adjacencyList);
                    tarjan.detectCycles();
                    auto tarjanTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();

                    // Run Union-Find algorithm
                    start = chrono::high_resolution_clock::now();
                    UnionFindCycle unionFind(adjacencyList);
                    bool hasCycle = unionFind.detectCycles();
                    auto unionFindTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();

                    int srcNode = tarjan.detectCycleSourceNode();
                    // Display results
                    resultsArea.addText("Tarjan Cycle Detection Time: " + to_string(tarjanTime) + " ms");
                    resultsArea.addText("Union-Find Cycle Detection Time: " + to_string(unionFindTime) + " ms");
                    resultsArea.addText("Graph has cycle: " + string(hasCycle ? "Yes" : "No"));
                    resultsArea.addText("Cycle source node " + to_string(srcNode));
                    resultsArea.addText("");
                }
            }

            // Handle scrolling
            if (event.type == Event::MouseWheelScrolled) {
                resultsArea.handleScroll(event.mouseWheelScroll.delta * 10);
            }
        }

        window.clear(Color::White);

        // Draw labels and text fields
        for (auto& label : labels) {
            window.draw(label);
        }
        nodesField.draw(window);
        edgesField.draw(window);

        // Draw the Run button and text
        window.draw(runButton);
        window.draw(runText);

        // Draw results
        resultsArea.draw();

        window.display();
    }

    return 0;
}
