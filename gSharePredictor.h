#ifndef	gSharePredictor_h
#define	gSharePredictor_h
#include "misc.h"

//prediction based on a 2 bit bimodal prediction table;
//using the global history register to index it;
void gsharePredictor(int& cp, 
	int& b,
	ifstream& f,
	int predTableSize,
	unsigned int gHRMask)
{
	assert(predTableSize > 0 && "prediction table size must be > 0");
	resetIfstream(f);

	unsigned long long int addr, target;
	string behavior;
	vector<unsigned int> biTable = vector<unsigned int>(predTableSize, 3);
	unsigned int gHR = 0, gTableIndex;

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;

		unsigned int behaviorBit = convert(behavior) & 2;
		//pred table index = PC bits XOR gHR bits;
		gTableIndex = (addr & (predTableSize - 1)) ^ (gHR & gHRMask);

		if(behaviorBit == (biTable.at(gTableIndex) & 2))
			cp++;

		//update gHR;
		gHR = gHR << 1;
		if(behaviorBit == 2)
		{
			gHR = gHR | 1;
			if(biTable.at(gTableIndex) < 3)
				biTable.at(gTableIndex)++;
		}
		else if(behaviorBit == 0)
		{
			if(biTable.at(gTableIndex) > 0)
				biTable.at(gTableIndex)--;
		}
	}
}

#endif
