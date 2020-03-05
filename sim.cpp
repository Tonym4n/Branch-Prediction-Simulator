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
	vector<unsigned int>& v,
	ifstream& f,
	int s)
{
	unsigned long long int addr, target;
	string behavior;
	unsigned int tableIndex;
	//all predictions are initially taken("T" = 2);
	v = vector<unsigned int>(s, 2);

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		unsigned int behaviorBit = convert(behavior);

		//get lower bits of the addr to index into prediction table;
		//-1 because index starts at 0 instead of 1;
		tableIndex = addr & (s - 1);
		if(behaviorBit == v.at(tableIndex))
			cp++;
		else
			v.at(tableIndex) = behaviorBit;
	}
}

//prediction based on a 2 bit bimodal prediction table("NN", "NT", "T", "TT");
//NN = strongly not-taken, NT = not-taken, T = taken, and TT = strongly taken;
void twoBitBimodalPredictor(unsigned long long int& cp, 
	unsigned long long int& b,
	vector<unsigned int>& v,
	ifstream& f,
	int s)
{
	unsigned long long int addr, target;
	string behavior;
	unsigned int tableIndex;
	//all predictions are initially strongly taken("TT" = 3);
	v = vector<unsigned int>(s, 3);

	while(f >> std::hex >> addr >> behavior >> target)
	{
		b++;
		unsigned int behaviorBit = convert(behavior);
		//mask msb to determine if its taken(2) or not-taken(0);
		behaviorBit = behaviorBit & 2;
		tableIndex = addr & (s - 1);

		if(behaviorBit == (v.at(tableIndex) & 2))
			cp++;

		//don't go above or below 2 bits;
		if(behaviorBit == 2)
		{
			if(v.at(tableIndex) < 3)
				v.at(tableIndex)++;
		}
		else if(behaviorBit == 0)
		{
			if(v.at(tableIndex) > 0)
				v.at(tableIndex)--;
		}
	}
}

void bimodalPredictor(unsigned long long int& cp, 
	unsigned long long int& b,
	vector<unsigned int>& v,
	ifstream& f,
	int predTableSize,
	int type)
{
	assert(predTableSize > 0 && "prediction table size must be > 0");
	resetIfstream(f);

	if(type == 1)
		oneBitBimodalPredictor(cp, b, v, f, predTableSize);
	else if(type == 2)
		twoBitBimodalPredictor(cp, b, v, f, predTableSize);
	else
		exit(EXIT_FAILURE);
}

void gsharePredictor(unsigned long long int& cp, 
	unsigned long long int& b,
	vector<unsigned int>& v,
	ifstream& f,
	int predTableSize)
{
	assert(predTableSize > 0 && "prediction table size must be > 0");
	resetIfstream(f);
}

int main(int argc, char *argv[]) 
{
	unsigned long long int corrPred, branches;
	corrPred = branches = 0;
	vector<unsigned int> predTable;

//	ifstream file("test_input.txt");
	ifstream file("short_trace1.txt");
	if(!file.is_open())
		exit(EXIT_FAILURE);

	alwaysPredictor(corrPred, branches, file, "T");
	print(corrPred, branches);
	cout << endl;

	alwaysPredictor(corrPred, branches, file, "NT");
	print(corrPred, branches);
	cout << endl;

/*
	bimodalPredictor(corrPred, branches, predTable, file, 16, 1);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 32, 1);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 128, 1);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 256, 1);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 512, 1);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 1024, 1);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 2048, 1);
	print(corrPred, branches);
	cout << endl;

	bimodalPredictor(corrPred, branches, predTable, file, 16, 2);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 32, 2);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 128, 2);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 256, 2);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 512, 2);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 1024, 2);
	print(corrPred, branches);
	bimodalPredictor(corrPred, branches, predTable, file, 2048, 2);
	print(corrPred, branches);
	cout << endl;
*/


	file.close();
	return EXIT_SUCCESS;
}
