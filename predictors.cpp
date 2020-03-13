//misc contains all the relevant standard libraries such as <iostream>;
#include "misc.hh"
#include "alwaysPredictor.hh"
#include "bimodalPredictor.hh"
#include "gSharePredictor.hh"
#include "tournamentPredictor.hh"
#include "btb.hh"

//driver function to test all the different kinds of predictors;
int main(int argc, char *argv[]) 
{
	int corrPred = 0;
	int branches = 0;

	ifstream inFile(argv[1]);
	if(!inFile.is_open())
		abort();

	ofstream outFile(argv[2]);
	if(!outFile.is_open())
		abort();

	cout << "# correct predictions,# total branches;" << endl;

//always taken predictor;
	alwaysPredictor(corrPred, branches, inFile, "T");
	printToFile(outFile, corrPred, branches);
	outputAndReset(corrPred, branches);
	cout << " <- Always taken predictor" << endl;
	outFile << endl;

//never taken predictor;
	alwaysPredictor(corrPred, branches, inFile, "NT");
	printToFile(outFile, corrPred, branches);
	outputAndReset(corrPred, branches);
	cout << " <- Always not taken predictor" << endl;
	outFile << endl;

/*
//1bit bimodal predictor for pred table size:
//16, 32, 128, 256, 512, 1024, and 2048;
	for(int i = 16; i < 64; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, inFile, i, 1);
		printToFile(outFile, corrPred, branches);
		outputAndReset(corrPred, branches);
		cout << " ";
		outFile << " ";
	}
	for(int i = 128; i < 4096; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, inFile, i, 1);
		printToFile(outFile, corrPred, branches);
		outputAndReset(corrPred, branches);
		if(i != 2048)
		{
			cout << " ";
			outFile << " ";
		}
	}
	cout << " <- Single bit bimodal predictor" << endl;
	outFile << endl;

//2bit bimodal predictor for pred table sizes given above;
	for(int i = 16; i < 64; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, inFile, i, 2);
		printToFile(outFile, corrPred, branches);
		outputAndReset(corrPred, branches);
		cout << " ";
		outFile << " ";
	}
	for(int i = 128; i < 4096; i = i * 2)
	{
		bimodalPredictor(corrPred, branches, inFile, i, 2);
		printToFile(outFile, corrPred, branches);
		outputAndReset(corrPred, branches);
		if(i != 2048)
		{
			cout << " ";
			outFile << " ";
		}
	}
	cout << " <- Two bit saturating bimodal predictor" << endl;
	outFile << endl;

//gshare predictor (where gHR is 3 - 11 bits);
	unsigned int mask = 7;
	for(unsigned int i = 4; mask < 2048; mask = mask + (i * 2), i = i * 2)
	{
		gsharePredictor(corrPred, branches, inFile, 2048, mask);
		printToFile(outFile, corrPred, branches);
		outputAndReset(corrPred, branches);
		if(mask != 2047)
		{
			cout << " ";
			outFile << " ";
		}
	}
	cout << " <- Gshare predictor" << endl;
	outFile << endl;

//tournament predictor using 11 bit tables and 11 bit gHR;
	tournamentPredictor(corrPred, branches, inFile, 2048, 2047);
	printToFile(outFile, corrPred, branches);
	outputAndReset(corrPred, branches);
	cout << " <- Tournament predictor" << endl;
	outFile << endl;
*/

//branch target buffer predictor using a 7 bit buffer and 9 bit bimodal table;
	branchTargetBufferPredictor(corrPred, branches, inFile, 512, 512);
	printToFile(outFile, corrPred, branches);
	outputAndReset(corrPred, branches);
	cout << " <- Branch target buffer predictor" << endl;

	inFile.close();
	outFile.close();
	return EXIT_SUCCESS;
}