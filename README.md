# SAT_KAMI
## Introduction
  Given a graph (with several different color blocks) and certain number of steps, try to make all blocks be the same color. At each step, we can select a block and assign it an arbitrary color, and this change will influence the surrounding blocks with the same color.  
## Usage
### command line
```
cd test
make
./satTest
```
### input generator
  Use generator to produce test cases with certain properties. We can get a random graph.
```
Type num of nodes|num of color|num of edges
12 3 5
```

## result
  The result will tell you whether the graph can be the same color finally (if you use generator, the result nust be SAT. You can decrease step to see the new result). The result includes UNSAT/SAT, all informations in the process of SAT Solving, and the procedure of the correct way.
```
Type num of nodes|num of color|num of edges
12 4 5
==============================[MINISAT]===============================
| Conflicts |     ORIGINAL     |          LEARNT          | Progress |
|           | Clauses Literals | Clauses Literals  Lit/Cl |          |
======================================================================
|       117 |   28277    70004 |     116     1292    11.1 |  9.280 % |
======================================================================
SAT
C10(00) -> C5(00) -> C3(00) -> C0(00) -> C0(00)
```
 If you want to add test cases created bt yourself, you can add it to satTest.cpp by certain format.
### Input format
```
int num = 8, num_color = 3, num_e = 7, steps = 4;
vector<vertex> graph;
vector<string> colors = {"00", "01", "10", "01", "00", "01", "10", "01"};
vector<vector<int>> adjnodes = {{1}, {0,2}, {1,3}, {2,4}, {3,5,6}, {4}, {4,7}, {6}};
vector<vector<int>> edges = {{1}, {2}, {3}, {4}, {5,6}, {}, {7}, {}};
```

