#include <iostream>
#include <fstream>
#include <string>

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
void alwaysPrediction(unsigned long long int& cp, 
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
void oneBitBimodalPrediction(unsigned long long int& cp, 
	unsigned long long int& b,
	ifstream& f,
	int predTableSize)
{
	assert(predTableSize > 0 && "prediction table size must be > 0");
	
	resetIfstream(f);
	unsigned long long int addr, target;
	string behavior;
	unsigned int table[predTableSize], tableIndex;
	//all predictions are initially taken("T" = 2);
	for(int i = 0; i < predTableSize; i++)
		table[i] = 2;

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		unsigned int behaviorBit = convert(behavior);

		//get lower bits of the addr to index into prediction table;
		//-1 because index starts at 0 instead of 1;
		tableIndex = addr & (predTableSize - 1);
		if(behaviorBit == table[tableIndex])
			cp++;
		//swap if prediction is incorrect since it's only 1 bit;
		else
			table[tableIndex] = behaviorBit;
	}
}

//prediction based on a 2 bit bimodal prediction table("NN", "NT", "T", "TT");
//NN = strongly not-taken, NT = not-taken, T = taken, and TT = strongly taken;
void twoBitBimodalPrediction(unsigned long long int& cp, 
	unsigned long long int& b,
	ifstream& f,
	int predTableSize)
{
	assert(predTableSize > 0 && "prediction table size must be > 0");
	
	resetIfstream(f);
	unsigned long long int addr, target;
	string behavior;
	unsigned int table[predTableSize], tableIndex;

	//all predictions are initially strongly taken("TT" = 3);
	for(int i = 0; i < predTableSize; i++)
		table[i] = 3;

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		unsigned int behaviorBit = convert(behavior);

		//get lower bits of the addr to index into prediction table;
		//-1 because index starts at 0 instead of 1;
		tableIndex = addr & (predTableSize - 1);
		if(behaviorBit == table[tableIndex])
			cp++;
		//swap if prediction is incorrect since it's only 1 bit;
		else
			table[tableIndex] = behaviorBit;
	}
}

int main(int argc, char *argv[]) 
{
	unsigned long long int corrPred, branches;
	corrPred = branches = 0;

	ifstream file("test_input.txt");
//	ifstream file("short_trace1.txt");
	if(!file.is_open())
		exit(EXIT_FAILURE);

	alwaysPrediction(corrPred, branches, file, "T");
	print(corrPred, branches);
	cout << endl;

	alwaysPrediction(corrPred, branches, file, "NT");
	print(corrPred, branches);
	cout << endl;
/*
	oneBitBimodalPrediction(corrPred, branches, file, 16);
	print(corrPred, branches);
	oneBitBimodalPrediction(corrPred, branches, file, 32);
	print(corrPred, branches);
	oneBitBimodalPrediction(corrPred, branches, file, 128);
	print(corrPred, branches);
	oneBitBimodalPrediction(corrPred, branches, file, 256);
	print(corrPred, branches);
	oneBitBimodalPrediction(corrPred, branches, file, 512);
	print(corrPred, branches);
	oneBitBimodalPrediction(corrPred, branches, file, 1024);
	print(corrPred, branches);
	oneBitBimodalPrediction(corrPred, branches, file, 2048);
	print(corrPred, branches);
	cout << endl;
*/
	file.close();
	return EXIT_SUCCESS;
}
