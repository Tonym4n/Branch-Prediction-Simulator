#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

//#define NDEBUG
#include <cassert>

using namespace std;

//reset ifstream object to beginning of file;
void resetIfstream(ifstream& f)
{
	if(f.eof())
		f.clear();
	f.seekg(0, f.beg);
}

//to output file + reset # of correct predictions and branches;
void print(unsigned long long int& cp,unsigned long long int& b)
{
	cout << cp << "," << b << "; ";
	cp = b = 0;
}

//convert string behavior to a 2 bit integer;
unsigned int convert(string behavior)
{
	//strongly not-taken;
	if(behavior == "NN")
		return 0;
	//not-taken;
	else if(behavior == "NT")
		return 1;
	//taken;
	else if(behavior == "T")
		return 2;
	//strongly taken;
	else if(behavior == "TT")
		return 3;
	else
		exit(EXIT_FAILURE);
}

//prediction based on always taken("T"), or always not-taken("NT");
void alwaysPredictor(unsigned long long int& cp, 
	unsigned long long int& b,
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

//prediction based on a 1 bit bimodal prediction table("T", "NT");
void oneBitBimodalPredictor(unsigned long long int& cp, 
	unsigned long long int& b,
	ifstream& f,
	int s)
{
	unsigned long long int addr, target;
	string behavior;
	unsigned int pcIndex;
	//all predictions are initially taken("T" = 2);
	vector<unsigned int> v = vector<unsigned int>(s, 2);

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		unsigned int behaviorBit = convert(behavior);

		//get lower bits of the addr to index into prediction table;
		//-1 because index starts at 0 instead of 1;
		pcIndex = addr & (s - 1);
		if(behaviorBit == v.at(pcIndex))
			cp++;
		else
			v.at(pcIndex) = behaviorBit;
	}
}

//prediction based on a 2 bit bimodal prediction table("NN", "NT", "T", "TT");
//NN = strongly not-taken, NT = not-taken, T = taken, and TT = strongly taken;
void twoBitBimodalPredictor(unsigned long long int& cp, 
	unsigned long long int& b,
	ifstream& f,
	int s)
{
	unsigned long long int addr, target;
	string behavior;
	unsigned int pcIndex;
	//all predictions are initially strongly taken("TT" = 3);
	vector<unsigned int> v = vector<unsigned int>(s, 3);

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		unsigned int behaviorBit = convert(behavior);
		//mask msb to determine if its taken(2) or not-taken(0);
		behaviorBit = behaviorBit & 2;
		pcIndex = addr & (s - 1);

		if(behaviorBit == (v.at(pcIndex) & 2))
			cp++;

		//don't go above or below 2 bits;
		if(behaviorBit == 2)
		{
			if(v.at(pcIndex) < 3)
				v.at(pcIndex)++;
		}
		else if(behaviorBit == 0)
		{
			if(v.at(pcIndex) > 0)
				v.at(pcIndex)--;
		}
	}
}

void bimodalPredictor(unsigned long long int& cp, 
	unsigned long long int& b,
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

//prediction based on a 2 bit bimodal prediction table;
//using the global history register to index it;
void gsharePredictor(unsigned long long int& cp, 
	unsigned long long int& b,
	ifstream& f,
	int predTableSize,
	unsigned int gHRMask)
{
	assert(predTableSize > 0 && gHRMask >= 0 && "prediction table size must be > 0");
	resetIfstream(f);

	unsigned long long int addr, target;
	string behavior;
	vector<unsigned int> v = vector<unsigned int>(predTableSize, 3);
	unsigned int gHR = 0, gTableIndex;

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;

		unsigned int behaviorBit = convert(behavior);
		behaviorBit = behaviorBit & 2;
		//pred table index = PC bits XOR gHR bits;
		gTableIndex = (addr & (predTableSize - 1)) ^ (gHR & gHRMask);

		if(behaviorBit == (v.at(gTableIndex) & 2))
			cp++;

		//update gHR;
		gHR = gHR << 1;
		if(behaviorBit == 2)
		{
			gHR = gHR | 1;
			if(v.at(gTableIndex) < 3)
				v.at(gTableIndex)++;
		}
		else if(behaviorBit == 0)
		{
			if(v.at(gTableIndex) > 0)
				v.at(gTableIndex)--;
		}
	}
}

//prediction based on selecting either a 2bit bimodal predictor table
//or a gshare predictor table;
void tournamentPredictor(unsigned long long int& cp, 
	unsigned long long int& b,
	ifstream& f,
	int predTableSize,
	unsigned int gHRMask)
{
	assert(predTableSize > 0 && gHRMask >= 0 && "prediction table size must be > 0");
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

int main(int argc, char *argv[]) 
{
	unsigned long long int corrPred, branches;
	corrPred = branches = 0;
	vector<unsigned int> predTable;

//	ifstream file("test_input.txt");
	ifstream file("short_trace2.txt");
	if(!file.is_open())
		exit(EXIT_FAILURE);

//always taken predictor;
	alwaysPredictor(corrPred, branches, file, "T");
	print(corrPred, branches);
	cout << endl;

//never taken predictor;
	alwaysPredictor(corrPred, branches, file, "NT");
	print(corrPred, branches);
	cout << endl;

///*
//1bit bimodal predictor for pred table size:
//16, 32, 128, 256, 512, 1024, and 2048;
	for(int i = 16; i < 64; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, file, i, 1);
		print(corrPred, branches);
	}
	for(int i = 128; i < 4096; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, file, i, 1);
		print(corrPred, branches);
	}
	cout << endl;

//2bit bimodal predictor for pred table sizes given above;
	for(int i = 16; i < 64; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, file, i, 2);
		print(corrPred, branches);
	}
	for(int i = 128; i < 4096; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, file, i, 2);
		print(corrPred, branches);
	}
	cout << endl;

//gshare predictor (where gHR is 3 - 11 bits);
	unsigned int mask = 7;
	for(unsigned int i = 4; mask < 2048; mask = mask + (i * 2), i = i * 2)
	{
		gsharePredictor(corrPred, branches, file, 2048, mask);
		print(corrPred, branches);
	}
	cout << endl;

//tournament predictor using 11 bit tables and 11 bit gHR;
	tournamentPredictor(corrPred, branches, file, 2048, 2047);
	print(corrPred, branches);
	cout << endl;
//*/

	file.close();
	return EXIT_SUCCESS;
}
