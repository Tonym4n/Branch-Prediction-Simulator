sim:		sim.o
		g++ -g -Wall -std=c++17 sim.o -o sim

sim.o:		sim.cpp
		g++ -g -Wall -std=c++17 -c sim.cpp

clean:	
		rm -f *~ *.o *.gch sim
