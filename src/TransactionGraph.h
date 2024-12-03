//
// Created by jalg0 on 11/24/2024.
//

#ifndef TRANSACTIONGRAPH_H
#define TRANSACTIONGRAPH_H
#include <random>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <stack>

using namespace std;

//Julio Leonardi's part
class TransactionGraph { // graph will hopefully not feature self loops
    unordered_map<int, vector<int>> adjacencyList;
    int nodeNum;
    int cycleNum; // code will have at least cycleNum cycles, cycle generation may add more
    int edgeNum; // code will have at least this amount of edges, cycle generation may add more
    int minCycleSize;
    int maxCycleSize;

public:
    TransactionGraph(int nm, int en, int cn, int mincs, int maccs) {
        cycleNum = cn;
        nodeNum = nm;
        edgeNum = en;
        minCycleSize = mincs;
        maxCycleSize = maccs;
        makeGraph();
    }

    // src for random generation: https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    void makeGraph() {
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<int> node(1, nodeNum);

        while (adjacencyList.size() < nodeNum) {
            int u = node(rng);
            if (adjacencyList.count(u)==0) {
                adjacencyList[u] = {};
            }
        }

        int edgesAdded = 0;
        while (edgesAdded < edgeNum) {
            int u = node(rng);
            int v = node(rng);

            if (u != v) {
                auto& neighbors = adjacencyList[u];
                if (find(neighbors.begin(), neighbors.end(), v) == neighbors.end()) {
                    adjacencyList[u].emplace_back(v);
                    edgesAdded++;
                }
            }
        }

        set<int> addedNodes;
        int cyclesAdded = 0;

        while (cyclesAdded < cycleNum) {
            int source = node(rng);
            while (addedNodes.find(source) != addedNodes.end()) {
                source = node(rng);
            }

            addCycle(source);
            cyclesAdded++;

            set<int> cycleNodes;
            int curNode = source;

            cycleNodes.insert(curNode);

            while (true) {
                if (adjacencyList[curNode].empty()) break;

                curNode = adjacencyList[curNode].back();
                cycleNodes.insert(curNode);

                if (curNode == source) break;
            }

            addedNodes.insert(cycleNodes.begin(), cycleNodes.end());
        }
    }

    void addCycle(int source) {
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<std::mt19937::result_type> cs(minCycleSize, maxCycleSize);

        int size = cs(rng);

        set<int> cycleNodes;
        cycleNodes.insert(source);

        int past = source;
        int cur;

        for (int i = 0; i < size-1; i++) {
            uniform_int_distribution<std::mt19937::result_type> node(1, nodeNum);
            cur = node(rng);

            while (cycleNodes.find(cur) != cycleNodes.end() || cur == past) {
                cur = node(rng);
            }

            adjacencyList[past].emplace_back(cur);
            cycleNodes.insert(cur);
            past = cur;
        }

        adjacencyList[past].emplace_back(source);
    }

    unordered_map<int, vector<int>> getGraph() {
        return adjacencyList;
    }
};

//Sebastian Garcia's part
class TrajanCycle {
    unordered_map<int, vector<int>> adjacencyList;
    int index; // Tarjan's index counter
    unordered_map<int, int> indexes; // node to index value for map
    unordered_map<int, int> lowlink; // mode to low-link value for map
    unordered_map<int, bool> onStack; // Node to on-stack status
    stack<int> Nstack; // Stack for algo
    vector<vector<int>> cycles; // List of cycles

    public:

    TrajanCycle(const unordered_map<int, vector<int>>& al) {
        adjacencyList = al;
    }

    // Tarjan's Algorithm to detect cycles
    void detectCycles() {
        // Initialize variables
        index = 0;
        indexes.clear();
        lowlink.clear();
        onStack.clear();
        while (!Nstack.empty()) {
            Nstack.pop();
        }
        cycles.clear();

        // DFS for all unvisited nodes
        for (auto& nodepair : adjacencyList) {
            int node = nodepair.first;
            if (indexes.find(node) == indexes.end()) {
                strongConnect(node);
            }
        }

        // Filter SCCs to get cycles
        vector<vector<int>> cycleList;
        for (auto& SCC : cycles) {
            if (SCC.size() > 1) {
                cycleList.push_back(SCC);
            }
        }
        cycles = cycleList;
    }

    // Member function for Tarjan's strong connect
    void strongConnect(int node) {
        indexes[node] = index;
        lowlink[node] = index;
        index++;
        Nstack.push(node);
        onStack[node] = true;

        // Finding all successors of the node
        for (int neighbor : adjacencyList[node]) {
            // If neighbor is not yet indexed, then we need to recurse on it
            if (indexes.find(neighbor) == indexes.end()) {
                strongConnect(neighbor);
                lowlink[node] = min(lowlink[node], lowlink[neighbor]);
            } else if (onStack[neighbor]) {
                // Update the lowlink value if neighbor is in the stack
                lowlink[node] = min(lowlink[node], indexes[neighbor]);
            }
        }

        // If node is a root node, pop the stack and make a strongly connected component
        if (lowlink[node] == indexes[node]) {
            vector<int> component;
            int curNode;
            do {
                curNode = Nstack.top();
                Nstack.pop();
                onStack[curNode] = false;
                component.push_back(curNode);
            } while (curNode != node);
            cycles.push_back(component);
        }
    }

    vector<vector<int>> getCyclePaths() {
        return cycles;
    }

    int getCycleNumber() {
        return cycles.size();
    }
};

//Alejandra Alzamora's part
class UnionFindCycle {
    unordered_map<int, vector<int>> adjacencyList;
    unordered_map<int, int> parentNode;  // To store the parent node for each node
    unordered_map<int, int> level;    // Level map for union by rank
    unordered_map<int, bool> visitedNodes; // Marks visited nodes
    set<pair<int, int>> cycleConnections;  // Set of edges that form cycles

    public:

    UnionFindCycle(unordered_map<int, vector<int>> al) {
        adjacencyList = al;
    }

    // Union-Find Functions
    // Rank method inspired by https://www.geeksforgeeks.org/introduction-to-disjoint-set-data-structure-or-union-find-algorithm/?ref=oin_asr1
    int findSet(int node) {
        // check if node exists in parentNode
        if (parentNode.find(node) == parentNode.end()) {
            // parent node is node itself and initialize a level (aka rank) of 0
            parentNode[node] = node;
            level[node] = 0;
        }

        // path compression helps make future find operations faster
        if (parentNode[node] != node) {
            parentNode[node] = findSet(parentNode[node]);
        }

        return parentNode[node];
    }

    // Union operation
    void unionSets(int node1, int node2) {
        // find roots of node1 and node2
        int root1 = findSet(node1);
        int root2 = findSet(node2);

        if (root1 != root2) {
            // union by rank
            if (level[root1] > level[root2]) {
                // attach root2 under root1
                parentNode[root2] = root1;
            } else if (level[root1] < level[root2]) {
                // attach root1 under root2
                parentNode[root1] = root2;
            } else {
                // same rank: attach root2 under root1
                // increase level (aka rank) of root1
                parentNode[root2] = root1;
                level[root1]++;
            }
        }
    }

    // Function to detect cycles using Union-Find
    bool detectCycles() {
        bool cycleFound = false;

        for (auto& nodes : adjacencyList) {
            int node = nodes.first;

            for (int neighbor : nodes.second) {
                // self-loop edge case
                if (node == neighbor) {
                    cycleConnections.insert({node, neighbor});
                    cycleFound = true;
                    continue;  // continues along the next neighbor
                }

                // If findSet is true, cycle is detected
                if (findSet(node) == findSet(neighbor)) {
                    // Add the cycle-causing edge to cycleConnections set
                    cycleConnections.insert({node, neighbor});
                    cycleFound = true;
                }

                // union the sets of the two nodes if not already together
                unionSets(node, neighbor);
            }
        }

        return cycleFound;
    }

    set<pair<int, int>> getCycleEdges() {
        return cycleConnections;
    }

    //added functions for results (Julio)
    int getCycleNumber() const {
        return cycleConnections.size();
    }

    void resetVisitedNodes() {
        for (auto& node : visitedNodes) {
            node.second = false;
        }
    }

    unordered_map<int, vector<int>> getCyclePaths() {
        unordered_map<int, vector<int>> cyclePaths;

        for (const auto& edge : cycleConnections) {
            int startNode = edge.first;
            int current = startNode;
            vector<int> path;

            while (visitedNodes[current] == false) {
                visitedNodes[current] = true;
                path.push_back(current);
                current = parentNode[current];
            }

            cyclePaths[startNode] = path;
            resetVisitedNodes();
        }
        return cyclePaths;
    }
};

#endif //TRANSACTIONGRAPH_H
