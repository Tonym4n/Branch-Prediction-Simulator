#ifndef tournamentPredictor_h
#define tournamentPredictor_h
#include "misc.h"

//prediction based on selecting either a 2 bit bimodal predictor table
//or a gshare predictor table;
void tournamentPredictor(int& cp, 
	int& b,
	ifstream& f,
	int predTableSize,
	unsigned int gHRMask)
{
	assert(predTableSize > 0 && "prediction table size must be > 0");
	resetIfstream(f);
	unsigned long long int addr, target;
	string behavior;

	//gshare and bimodal table initialized to strongly taken;
	vector<unsigned int> gTable = vector<unsigned int>(predTableSize, 3);
	vector<unsigned int> biTable = vector<unsigned int>(predTableSize, 3);
	//selector table initialized to strongly prefer gTable;
	vector<unsigned int> sTable = vector<unsigned int>(predTableSize, 0);
	unsigned int gHR = 0, pcIndex, gTableIndex, biTableIndex, sTableIndex;


//first, sTable chooses which predictor table to use (either gshare or bimodal);
//next, it updates sTable if the predictions differ in both predictor tables;
//finally, it updates both predictor tables and the gHR. Then loops;
	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;

		unsigned int behaviorBit = convert(behavior);
		behaviorBit = behaviorBit & 2;

		pcIndex = addr & (predTableSize - 1);
		gTableIndex = pcIndex ^ (gHR & gHRMask);
		biTableIndex = sTableIndex = pcIndex;

		//pick gshare table;
		if((sTable.at(sTableIndex) & 2) == 0)
		{
			/****update selector table******/
			//if both gTable and biTable pred correct, no change;
			if( (behaviorBit == (gTable.at(gTableIndex) & 2)) && ((gTable.at(gTableIndex) & 2) == (biTable.at(biTableIndex) & 2)) )
				cp++;
			//else if gTable pred correct and biTable not correct, subtract 1 to prefer gTable;
			else if( (behaviorBit == (gTable.at(gTableIndex) & 2)) && ((gTable.at(gTableIndex) & 2) != (biTable.at(biTableIndex) & 2)) )
			{
				cp++;
				if(sTable.at(sTableIndex) > 0)
					sTable.at(sTableIndex)--;
			}
			//else if biTable pred correct, add 1 to prefer biTable;
			else if(behaviorBit == (biTable.at(biTableIndex) & 2))
			{
				if(sTable.at(sTableIndex) < 3)
					sTable.at(sTableIndex)++;
			}
			//else both wrong and no change;
			/*******************************/
		}
		//pick bimodal table;
		else if((sTable.at(sTableIndex) & 2) == 2)
		{
			/****update selector table******/
			//if both gTable and biTable pred correct, no change;
			if( (behaviorBit == (biTable.at(biTableIndex) & 2)) && ((biTable.at(biTableIndex) & 2) == (gTable.at(gTableIndex) & 2)) )
				cp++;
			//if biTable pred correct and gTable not correct, add 1 to prefer biTable;
			else if( (behaviorBit == (biTable.at(biTableIndex) & 2)) && ((biTable.at(biTableIndex) & 2) != (gTable.at(gTableIndex) & 2)) )
			{
				cp++;
				if(sTable.at(sTableIndex) < 3)
					sTable.at(sTableIndex)++;
			}
			//else if gTable pred correct, subtract 1 to prefer gTable;
			else if(behaviorBit == (gTable.at(gTableIndex) & 2))
			{
				if(sTable.at(sTableIndex) > 0)
					sTable.at(sTableIndex)--;
			}
			//else both wrong and no change;
			/*******************************/
		}

		/*********update both pred tables and gHR********/
		gHR = gHR << 1;
		if(behaviorBit == 2)
		{
			gHR = gHR | 1;
			if(gTable.at(gTableIndex) < 3)
				gTable.at(gTableIndex)++;
			if(biTable.at(biTableIndex) < 3)
				biTable.at(biTableIndex)++;
		}
		else if(behaviorBit == 0)
		{
			if(gTable.at(gTableIndex) > 0)
				gTable.at(gTableIndex)--;
			if(biTable.at(biTableIndex) > 0)
				biTable.at(biTableIndex)--;
		}
		/************************************************/
	}
}

#endif