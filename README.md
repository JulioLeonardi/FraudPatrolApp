This is our main group submission for Project 3 for DSA.
Running the main executable will open up a GUI made using 
SFML based on the template provided in https://github.com/SFML/cmake-sfml-project. The main purpose of this project is to showcase
the speed of two different cycle finding algorithms in a
drag race type test. Each algorithm will be fed a synthetic
transaction graph implemented as an adjacency list that may
or may not have cycles (will almost always have them at 
higher node and edge counts) which may be indicative of money 
laundering in real world scenarios. Then, each algorithm will
begin trying to find a cycle, and the source node of the first 
cycle found will be printed if a cycle is found. The algorithms
*always* find the same source node for a cycle given it exists,
as shown in https://github.com/JulioLeonardi/FraudPatrol. 

Some practical recommendations: Keep node count <= 10000 nodes, 
and edge count <= 500000 nodes. Given my aversion to working
with *THE HEAP* numbers above those may result in the program 
crashing due to stack overflow. 

