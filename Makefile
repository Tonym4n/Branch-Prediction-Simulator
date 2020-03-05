nums:		nums.o
		g++ -g -Wall -std=c++17 nums.o -o nums

nums.o:		nums.cpp seq.hh dlinkseq.hh arrayseq.hh
		g++ -g -Wall -std=c++17 -c nums.cpp seq.hh dlinkseq.hh arrayseq.hh

clean:	
		rm -f *~ *.o *.gch nums
