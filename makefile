MTGStatGraph: MTGStatGraph.o
	g++ -o MTGStatGraph MTGStatGraph.o

MTGStatGraph.o: MTGStatGraph.cpp
	g++ -c MTGStatGraph.cpp

clean:
	rm -f MTGStatGraph *.o
