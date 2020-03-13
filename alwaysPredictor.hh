#ifndef	alwaysPredictor_hh
#define	alwaysPredictor_hh
#include "misc.hh"

//prediction based on always taken("T"), or always not-taken("NT");
void alwaysPredictor(int& cp, 
	int& b,
	ifstream& f,
	string tempBehavior)
{
	resetIfstream(f);
	unsigned long long int addr, target;
	string behavior;
	//read one line in the text file each iteration;
	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		if(behavior == tempBehavior)
			cp++;
	}
}

#endif