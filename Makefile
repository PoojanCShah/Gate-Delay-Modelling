A : main.cpp
	g++ -o main main.cpp
	./main A $(circuit) $(gate_delay) $(longest_delay)

B : main.cpp
	g++ -o main main.cpp
	./main B $(circuit) $(gate_delay) $(longest_delay)
