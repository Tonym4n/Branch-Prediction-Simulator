#ifndef btb_hh
#define btb_hh
#include "misc.hh"

//prediction based on a 1 bit bimodal predictor table;
//and a buffer that holds the target addresses if the behavior is taken;
void branchTargetBufferPredictor(int& cp, 
	int& bufferAccesses,
	ifstream& f,
	int predTableSize,
	int bufferSize)
{
	assert(predTableSize > 0 && bufferSize > 0 && "prediction table and buffer size must be > 0");
	resetIfstream(f);
	unsigned long long int addr, target;
	string behavior;
	unsigned int pcIndex, bufferIndex;
	vector<unsigned int> biTable = vector<unsigned int>(predTableSize, 2);
	vector<unsigned long long int> buffer = vector<unsigned long long int>(bufferSize);

	while(f >> std::hex >> addr >> behavior >> target)
	{
		unsigned int behaviorBit = convert(behavior);
		pcIndex = addr & (predTableSize - 1);
		bufferIndex = addr & (bufferSize - 1);
		
		//if prediction is taken;
		if(biTable.at(pcIndex) == 2)
		{
			bufferAccesses++;
			if(behaviorBit == biTable.at(pcIndex))
				cp++;
			//if actual behavior is taken;
			if(behaviorBit == 2)
			{
//				if(target == buffer.at(bufferIndex))
//					cp++;
				buffer.at(bufferIndex) = target;
			}
		}

		//update prediction table;
		if(behaviorBit != biTable.at(pcIndex))
			biTable.at(pcIndex) = behaviorBit;
	}
}

#endif
