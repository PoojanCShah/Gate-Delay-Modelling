# Gate-Delay-Modelling

This project models a general circuit with both combinatorial and sequential elements as a Direct Acyclic Graph to calculate the delat propagation using Topological Sorting and Dynamic Programming. We also apply heuristic algorithms to optimize the area of the circuit while satisfying the delay constraint. Details can be found [here](report.pdf).

The circuit is provided using an edge list in the `circuit.txt` file : 

```
// Format:
// GATE_TYPE Input1 Input2...Output
// 
// Ignore blank lines, lines with spaces only, and lines starting with "//"

PRIMARY_INPUTS A B
PRIMARY_OUTPUTS E F
INTERNAL_SIGNALS C D G
INV A C
AND2 C B D
DFF D G
NAND2 C G E
INV G F
```

The gate delay and area parameters are given in the `gate_delays.txt` file : 

```
// Format:
// GATE_IMPLEMENTATION GATE_TYPE GATE_DELAY GATE_AREA
// Delays in nanoseconds
// Ignore blank lines, lines with spaces only, and lines starting with "//"

NAND2_1 NAND2 3.5 11.2
NAND2_2 NAND2 3 13
NAND2_3 NAND2 4.5 5.3
AND2_1 AND2 16.2 9.5
AND2_2 AND2 7 12
AND2_3 AND2 4 19.6
NOR2_1 NOR2 3.5 10
NOR2_2 NOR2 3 12.5
NOR2_3 NOR2 2.5 15
OR2_1 OR2 4.5 12.8
OR2_2 OR2 7.5 10.3
OR2_3 OR2 3.75 17
INV_1 INV 2 7.33
INV_2 INV 3 4.6
INV_3 INV 3.5 2.5

```

The maximum allowed delay is given in the `delay_constraint.txt` file.
```
7
```

The answers will be generated in the `longest_delay.txt` and `minimum_area.txt` files.

To run the code, use : 

```
g++ -o main main.cpp
./main
```