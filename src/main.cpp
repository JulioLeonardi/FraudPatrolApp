#include <SFML/Graphics.hpp>

#include <iostream>
#include <chrono>
#include "TransactionGraph.h"

using namespace std;

void displayMenu() {
    cout << "====== Cycle Detection Program ======\n";
    cout << "1. Input graph parameters\n";
    cout << "2. Run cycle detection algorithms\n";
    cout << "3. Exit\n";
    cout << "=====================================\n";
    cout << "Select an option: ";
}

int main() {
    int nodeNum, edgeNum, cycleNum, minCycleSize, maxCycleSize;

    while (true) {
        displayMenu();
        int option;
        cin >> option;

        switch (option) {
        case 1:
            cout << "Enter number of nodes: ";
            cin >> nodeNum;
            cout << "Enter number of edges: ";
            cin >> edgeNum;
            cout << "Enter number of cycles: ";
            cin >> cycleNum;
            cout << "Enter minimum cycle size: ";
            cin >> minCycleSize;
            cout << "Enter maximum cycle size: ";
            cin >> maxCycleSize;
            break;

        case 2: {
            cout << "Generating graph...\n";
            TransactionGraph graph(nodeNum, edgeNum, cycleNum, minCycleSize, maxCycleSize);
            unordered_map<int, vector<int>> adjacencyList = graph.getGraph();

            cout << "Running Tarjan's Algorithm...\n";
            auto tarjanStart = chrono::high_resolution_clock::now();
            TrajanCycle tarjan(adjacencyList);
            tarjan.detectCycles();
            auto tarjanEnd = chrono::high_resolution_clock::now();
            auto tarjanDuration = chrono::duration_cast<chrono::milliseconds>(tarjanEnd - tarjanStart);

            cout << "Tarjan's Algorithm completed.\n";
            cout << "Cycles detected: " << tarjan.getCycleNumber() << "\n";
            cout << "Execution time: " << tarjanDuration.count() << " ms\n";

            cout << "\nRunning Union-Find Algorithm...\n";
            auto unionFindStart = chrono::high_resolution_clock::now();
            UnionFindCycle uf(adjacencyList);
            bool hasCycles = uf.detectCycles();
            auto unionFindEnd = chrono::high_resolution_clock::now();
            auto unionFindDuration = chrono::duration_cast<chrono::milliseconds>(unionFindEnd - unionFindStart);

            cout << "Union-Find Algorithm completed.\n";
            cout << "Cycles detected: " << uf.getCycleNumber() << "\n";
            cout << "Execution time: " << unionFindDuration.count() << " ms\n";

            break;
        }

        case 3:
            cout << "Exiting program. Goodbye!\n";
            return 0;

        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
    }

    return 0;
}
