#ifndef misc_hh
#define misc_hh
#include <iostream>
#include <fstream>
#include <vector>

//#define NDEBUG
#include <cassert>

using namespace std;

//terminate due to error;
void abort()
{
	cout << "usage: ./predictors FILE_WITH_BRANCH_INFO FILE_OF_RESULTS" << endl;
	exit(EXIT_FAILURE);
}

//reset ifstream object to beginning of file;
void resetIfstream(ifstream& f)
{
	//reset eof bit;
	if(f.eof())
		f.clear();
	f.seekg(0, f.beg);
}

//output results to console and reset correct pred and branches;
void outputAndReset(int& cp,int& b)
{
	cout << cp << "," << b << ";";
	cp = b = 0;
}

//print results to text file;
void printToFile(ofstream& f, int cp, int b)
{
	string output;
	f << cp << "," << b << ";";
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

#endif