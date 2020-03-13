#ifndef bimodalPredictor_hh
#define bimodalPredictor_hh
#include "misc.hh"

//prediction based on a 1 bit bimodal prediction table("T", "NT");
void oneBitBimodalPredictor(int& cp, 
	int& b,
	ifstream& f,
	int s)
{
	unsigned long long int addr, target;
	string behavior;
	unsigned int pcIndex;
	//all predictions are initially taken("T" = 2);
	vector<unsigned int> biTable = vector<unsigned int>(s, 2);

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		unsigned int behaviorBit = convert(behavior);

		//get lower bits of the addr to index into prediction table;
		pcIndex = addr & (s - 1);
		if(behaviorBit == biTable.at(pcIndex))
			cp++;
		else
			biTable.at(pcIndex) = behaviorBit;
	}
}

//prediction based on a 2 bit bimodal prediction table("NN", "NT", "T", "TT");
//NN = strongly not-taken, NT = not-taken, T = taken, and TT = strongly taken;
void twoBitBimodalPredictor(int& cp, 
	int& b,
	ifstream& f,
	int s)
{
	unsigned long long int addr, target;
	string behavior;
	unsigned int pcIndex;
	//all predictions are initially strongly taken("TT" = 3);
	vector<unsigned int> biTable = vector<unsigned int>(s, 3);

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		//mask msb to determine if its taken(2) or not-taken(0);
		unsigned int behaviorBit = convert(behavior) & 2;
		pcIndex = addr & (s - 1);

		if(behaviorBit == (biTable.at(pcIndex) & 2))
			cp++;

		//don't go above or below 2 bits;
		if(behaviorBit == 2)
		{
			if(biTable.at(pcIndex) < 3)
				biTable.at(pcIndex)++;
		}
		else if(behaviorBit == 0)
		{
			if(biTable.at(pcIndex) > 0)
				biTable.at(pcIndex)--;
		}
	}
}

void bimodalPredictor(int& cp, 
	int& b,
	ifstream& f,
	int predTableSize,
	int type)
{
	assert(predTableSize > 0 && "prediction table size must be > 0");
	resetIfstream(f);

	if(type == 1)
		oneBitBimodalPredictor(cp, b, f, predTableSize);
	else if(type == 2)
		twoBitBimodalPredictor(cp, b, f, predTableSize);
	else
		exit(EXIT_FAILURE);
}

#endif